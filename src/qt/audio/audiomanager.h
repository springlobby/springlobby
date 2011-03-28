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


#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QThread>
#include <AL/al.h>
#include <AL/alc.h>
#include <QList>
#include <map>

class COggStream;

class AudioManager : public QThread
{
    Q_OBJECT
public:
    explicit AudioManager(QObject *parent = 0);
	~AudioManager();

	void run();

signals:

public slots:
	void playSound( const QString filename ) ;

private:
	size_t loadSound( const QString& path );
	void loadAllSounds();
	void getMusicFilenames();
	void setupAlSource( const ALuint id, const float volume );

	COggStream* ogg_stream_;
	ALuint ogg_stream_id_;
	static const size_t max_sounds_ = 8;
	ALuint sources_[max_sounds_];
	size_t no_busy_sources_;
	ALuint tmp_id;
	QList<QString> music_filenames;
	float master_volume_;

	typedef std::map<QString,size_t>
		BufferIdMapType;
	BufferIdMapType fn_to_bufferID_map;

	ALCdevice* device_;

};

#endif // AUDIOMANAGER_H
