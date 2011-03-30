/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


#include "audiomanager.h"
#include "OggStream.h"
#include "ALShared.h"
#include "SoundBuffer.h"
#include "../FileHandler.h"
#include <assert.h>
#include <AL/alc.h>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <customizations.h>
#include <settings.h>

/* some pieces cobbled together from spring's SoundSource.cpp  */

AudioManager::AudioManager(QObject *parent) :
	QThread(parent),
	ogg_stream_( 0 ),
	no_busy_sources_( 0 ),
	master_volume_( 0.5f ),
	device_( 0 ),
	active_( sett().Get( _T("sasi/audio_enabled"), true ) ),
	quitting_time_( false )
{
	SoundBuffer::Initialise();
	qDebug() << "AudioManager ctor";
	alGetError();

	device_ = alcOpenDevice(0);//default device

	if (device_ == NULL)
	{
		qDebug()  <<  "Could not open a sound device, disabling sounds";
		CheckError("CSound::InitAL");
	}
	else
	{
		ALCcontext *context = alcCreateContext(device_, NULL);
		if (context != NULL)
		{
			alcMakeContextCurrent(context);
			CheckError("CSound::CreateContext");
		}
		else
		{
			alcCloseDevice(device_);
			qDebug()  << "Could not create OpenAL audio context";
		}
	}

	alGenSources(8, sources_);
	ogg_stream_id_ = sources_[0];
	if (!CheckError("CSoundSource::CSoundSource")) {
		ogg_stream_id_ = 0;
	}

	setupAlSource( ogg_stream_id_, master_volume_*1 );
	for ( int i = 1; i < int(max_sounds_); ++i )
		setupAlSource( sources_[i], master_volume_*0.3 );
	alListenerf(AL_GAIN, master_volume_ );

	loadAllSounds();
	getMusicFilenames();
	if ( music_filenames.size() == 0 )
	{
		qDebug() << "no music found";
		return;
	}

	ogg_stream_ = new COggStream(ogg_stream_id_);
	assert( ogg_stream_ );
}

AudioManager::~AudioManager()
{
	alDeleteSources(1, &ogg_stream_id_);
	alcCloseDevice( device_ );
	CheckError("CSoundSource::~CSoundSource");
	sett().Set( _T("sasi/audio_enabled"), active_ );
}

void AudioManager::setupAlSource( const ALuint id, const float volume )
{
	alSourcef(id,	AL_REFERENCE_DISTANCE, 2.f );
	alSource3f(id,	AL_POSITION,       0.0f, 0.0f, 0.0f);
	alSourcef(id,	AL_GAIN,            volume);
	alSource3f(id,	AL_VELOCITY,       0.0f,  0.0f,  0.0f);
	alSource3f(id,	AL_DIRECTION,      0.0f,  0.0f,  0.0f);
	alSourcef(id,	AL_ROLLOFF_FACTOR,  0.0f);
	alSourcei(id,	AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(id,	AL_BUFFER, AL_NONE);
}

void AudioManager::run()
{
	if ( active_ && ogg_stream_ )
	{
		ogg_stream_->Play( music_filenames[0].toStdString(), master_volume_ /*this has no effect actually*/ );
		no_busy_sources_++;
		music_filenames.pop_front();
		ogg_stream_->Update();
		CheckError("AudioManager::play");
	}

	while( !quitting_time_ ) {
		if ( !active_ )// no need to update that often if we're nto active
		{
			sleep( 1 );
			continue;
		}

		msleep( 50 );
		if( ogg_stream_ )
			ogg_stream_->Update();
		if ( ogg_stream_ && ogg_stream_->IsFinished() )
		{
			if ( music_filenames.size() == 0 ) //meaning we've played all music once
				getMusicFilenames();//refill queue
			if ( music_filenames.size() == 0 ) //someone removed music at runtime, bad
			{
				no_busy_sources_--;
				return;
			}
			ogg_stream_->Play( music_filenames[0].toStdString(), master_volume_ );
			music_filenames.pop_front();
		}
	}
	no_busy_sources_--;
	qDebug() << "playback finished";
}


void AudioManager::getMusicFilenames()
{
	music_filenames.clear();

	QDir music_dir ( SLcustomizations().MusicDir() );
	QFileInfo current_file;
	QStringList nameFilter;
	nameFilter << "*.ogg" ;
	foreach( current_file, music_dir.entryInfoList( nameFilter, QDir::Files ) )
	{
		qDebug() << current_file.absoluteFilePath() ;
		music_filenames.append( current_file.absoluteFilePath() );
	}
}

size_t AudioManager:: loadSound( const QString& q_path )
{
	const std::string path = q_path.toStdString();
	const size_t id = SoundBuffer::GetId(path);

	if (id > 0) {
		return id; // file is loaded already
	}

	CFileHandler file(path);
	if (!file.FileExists()) {
		qDebug() << "Unable to open audio file: " << path.c_str();
		return 0;
	}

	std::vector<boost::uint8_t> buf(file.FileSize());
	file.Read(&buf[0], file.FileSize());

	boost::shared_ptr<SoundBuffer> buffer(new SoundBuffer());
	bool success = false;
	const std::string ending = file.GetFileExt();
	if (ending == "wav")
		success = buffer->LoadWAV(path, buf, false);
	else if (ending == "ogg")
		success = buffer->LoadVorbis(path, buf, false);
	else
		qDebug() << "CSound::LoadALBuffer: unknown audio format: " << ending.c_str();

	CheckError("CSound::LoadALBuffer");
	if (!success) {
		qDebug() << "Failed to load file: " << path.c_str();
		return 0;
	}

	return SoundBuffer::Insert(buffer);
}


void AudioManager::playSound( const QString filename )
{
	if ( !active_ || no_busy_sources_ >= max_sounds_ )
		return;
	no_busy_sources_++;
	ALuint tmp_id2 = sources_[1];
	BufferIdMapType::const_iterator buffer_it = fn_to_bufferID_map.find( filename );
	if ( buffer_it == fn_to_bufferID_map.end() )
	{
		qDebug() << "cannot find (maybe not loaded?) sound: " << filename;
		return;
	}
	boost::shared_ptr<SoundBuffer> buffer = SoundBuffer::GetById( buffer_it->second );

	alSourcei(tmp_id2, AL_BUFFER, buffer->GetId());
	alSourcePlay(tmp_id2);
	CheckError("alSourcePlay(tmp_id)");
	no_busy_sources_--;
}

void AudioManager::loadAllSounds()
{
	QDir sound_dir( SLcustomizations().SoundsDir() );
	QStringList nameFilter;
		nameFilter << "*.wav" << "*.raw";
	QFileInfo current_file;
	foreach( current_file, sound_dir.entryInfoList( nameFilter, QDir::Files ) )
	{
		fn_to_bufferID_map[current_file.fileName()] = loadSound( current_file.absoluteFilePath() );
	}
}

void AudioManager::pause()
{
	if ( active_ && ogg_stream_ )
		ogg_stream_->TogglePause();
	for ( int i = 0; i < int(max_sounds_); ++i )
		alSourcePause(i);
	active_ = false;
}

void AudioManager::resume()
{
	if ( !active_ && ogg_stream_ )
		ogg_stream_->TogglePause();
	for ( int i = 0; i < int(max_sounds_); ++i )
		alSourcePlay(i);
	active_ = true;
}

bool AudioManager::toggleActive()
{
	if ( active_ )
		pause();
	else
		resume();
	return active_;
}
