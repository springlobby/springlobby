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
#include <QList>

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

private:
	void getMusicFilenames();

	COggStream* ogg_stream_;
	ALuint ogg_stream_id_;
	QList<QString> music_filenames;

};

#endif // AUDIOMANAGER_H
