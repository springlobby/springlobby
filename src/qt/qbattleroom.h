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


#ifndef QBATTLEROOM_H
#define QBATTLEROOM_H

#include <QDeclarativeItem>
class Battle;
class QBattleroom : public QDeclarativeItem
{
    Q_OBJECT
	Q_PROPERTY(int battleId READ battleId WRITE setBattleId NOTIFY battleIdChanged)
	Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
public:
	explicit QBattleroom(QDeclarativeItem *parent = 0);

	int battleId() const;
	void setBattleId( int id );
	QString description()const;

signals:
	void battleIdChanged();
	void descriptionChanged();

public slots:
	void say(const QString& text );

private:
	Battle* m_battle;
	int m_battle_id;
};

#endif // QBATTLEROOM_H
