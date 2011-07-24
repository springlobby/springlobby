#ifndef OFFLINEBATTLE_H_INCLUDED
#define OFFLINEBATTLE_H_INCLUDED

#include "ibattle.h"

class OfflineBattle : public IBattle
{
	public:
			OfflineBattle ( const int id );
			OfflineBattle ( );
			OfflineBattle ( const OfflineBattle&  );
			OfflineBattle& operator = ( const OfflineBattle&  );
			~OfflineBattle (){}
			User& GetMe() { return m_me; }
            const User& GetMe() const { return m_me; }
			bool IsFounderMe() const { return true; }
			void StartSpring();

	protected:
			int m_id;
			User m_me;
};

#endif // OFFLINEBATTLE_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

