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


#ifndef SASI_APP_H
#define SASI_APP_H

#include <QApplication>

class BattlelistModel;
class TASServer;

class SasiApp : public QApplication
{
	Q_OBJECT
public:
	explicit SasiApp(int argc, char *argv[]);
	bool CmdInit();
	~SasiApp();

	int exec();

signals:
	void appLoaded();

public slots:
	void Update();

private:
	BattlelistModel* bl_model;
        TASServer* m_server;
};

#endif // SASI_APP_H
