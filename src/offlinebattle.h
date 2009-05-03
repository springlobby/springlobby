#ifndef OFFLINEBATTLE_H_INCLUDED
#define OFFLINEBATTLE_H_INCLUDED

#include "ibattle.h"

class OfflineBattle : public IBattle
{
	public:
			OfflineBattle ( const int id ): m_id( id ), m_me( User(_T("")) )
			{
				m_opts.founder = m_me.GetNick();
				AddUser( m_me );
				m_me.BattleStatus().spectator = true;
				m_me.BattleStatus().sync = true;
			}
			OfflineBattle ( ): m_id( 0 ), m_me( User(_T("")) ) {}
			~OfflineBattle (){};
			User& GetMe() { return m_me; }
			bool IsFounderMe() {return true;}

	protected:
			int m_id;
			User m_me;
};

#endif // OFFLINEBATTLE_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

