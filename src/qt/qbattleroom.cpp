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


#include "qbattleroom.h"
#include <server.h>
#include <utils/conversion.h>
#include <battle.h>

QBattleroom::QBattleroom(QDeclarativeItem *parent) :
	QDeclarativeItem(parent),
	m_battle(0),
	m_battle_id(-1)
{
}

void QBattleroom::setBattleId( int id )
{
	if ( id == m_battle_id )
		return;
	m_battle_id = id;
	//find and set battle and stuff
	try {
		m_battle = &serverSelector().GetServer().GetBattle( id );
		Battle* my_battle = serverSelector().GetServer().GetMe().GetBattle();
		if ( my_battle && m_battle != my_battle )
			my_battle->Leave();
	}
	catch ( ... ) {}

	m_battle->Join();
	emit battleIdChanged();
	emit descriptionChanged();
}
int QBattleroom::battleId() const
{
	return m_battle_id;
}

QString QBattleroom::description()const
{
	return m_battle ? FromwxString<QString>( m_battle->GetDescription() ) : "none" ;
}

void QBattleroom::say(const QString& text )
{
	if (!m_battle)
		return;
	m_battle->Say( TowxString( text ) );
}
