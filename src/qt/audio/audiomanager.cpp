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
	alGetError();
	//*

	//set up 3d sound
	ALfloat	alpos[3];
	ALfloat	alvel[3];
	ALfloat	alori[6];
	alpos[0] = 0; alpos[1] = 0; alpos[2] = 0;
	alvel[0] = 0; alvel[1] = 0; alvel[2] = 0;
	alori[0] = 0; alori[1] = 0; alori[2] = -1;
	alori[3] = 0; alori[4] = 1; alori[5] = 0;

	alListenerfv(AL_POSITION,alpos);
	alListenerfv(AL_VELOCITY,alvel);
	alListenerfv(AL_ORIENTATION,alori);
	//*

		const ALchar* deviceName = NULL;
		std::string configDeviceName = "";

		ALCdevice* device = alcOpenDevice(deviceName);

		if ((device == NULL) && (deviceName != NULL))
		{
			qDebug() << "Could not open the sound device \""
					<< deviceName << "\", trying the default device ...";
			configDeviceName = "";
			deviceName = NULL;
			device = alcOpenDevice(deviceName);
		}

		if (device == NULL)
		{
			qDebug()  <<  "Could not open a sound device, disabling sounds";
			CheckError("CSound::InitAL");
			return;
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
				return;
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
	assert( ogg_stream_ );

	ogg_stream_->Play( filename.toStdString(), 70.5f );

	CheckError("AudioManager::play");

	while( !ogg_stream_->IsFinished() )
		sleep( 5000 );
}
