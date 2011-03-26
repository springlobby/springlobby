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

#include <assert.h>
#include <AL/alc.h>
#include <QDebug>

/* some pieces cobbled together from spring's SoundSource.cpp  */

AudioManager::AudioManager(QObject *parent) :
	QThread(parent)
{
}

AudioManager::~AudioManager()
{
	alDeleteSources(1, &ogg_stream_id_);
	CheckError("CSoundSource::~CSoundSource");
}

void AudioManager::enqueue( const QString& fn )
{
	filename = fn;
}

void AudioManager::run()
{
	qDebug() << "AudioManager ctor";
	alGetError();

	ALCdevice* device = alcOpenDevice(0);//default device

	if (device == NULL)
	{
		qDebug()  <<  "Could not open a sound device, disabling sounds";
		CheckError("CSound::InitAL");
	}
	else
	{
		ALCcontext *context = alcCreateContext(device, NULL);
		if (context != NULL)
		{
			alcMakeContextCurrent(context);
			CheckError("CSound::CreateContext");
		}
		else
		{
			alcCloseDevice(device);
			qDebug()  << "Could not create OpenAL audio context";
		}
	}

	alGenSources(1, &ogg_stream_id_);
	if (!CheckError("CSoundSource::CSoundSource")) {
		ogg_stream_id_ = 0;
	} else {
		alSourcef(ogg_stream_id_, AL_REFERENCE_DISTANCE, 2.f );
		CheckError("CSoundSource::CSoundSource");
	}
	ogg_stream_ = new COggStream(ogg_stream_id_);

	const float volume = 50.5f;

	assert( ogg_stream_ );

	alSource3f(ogg_stream_id_,	AL_POSITION,       0.0f, 0.0f, 0.0f);
	alSourcef(ogg_stream_id_,	AL_GAIN,            volume);
	alSource3f(ogg_stream_id_,	AL_VELOCITY,       0.0f,  0.0f,  0.0f);
	alSource3f(ogg_stream_id_,	AL_DIRECTION,      0.0f,  0.0f,  0.0f);
	alSourcef(ogg_stream_id_,	AL_ROLLOFF_FACTOR,  0.0f);
	alSourcei(ogg_stream_id_,	AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(ogg_stream_id_,	AL_BUFFER, AL_NONE);

	ogg_stream_->Play( filename.toStdString(), volume );
	ogg_stream_->Update();

	CheckError("AudioManager::play");

	while( true ) {
		msleep( 50 );
		ogg_stream_->Update();
	}
	qDebug() << "playback finished";
}
