#ifndef SIMPLEMPFRAME_H
#define SIMPLEMPFRAME_H

#include "customizedframe.h"
#include "../utils/mixins.hh"

class TASServer;
class SimpleBattlelistCtrl;

class SimpleMPFrame : public CustomizedFrame, public SL::NonCopyable
{
public:
	SimpleMPFrame( wxWindow* parent );
	~SimpleMPFrame();

protected:
	TASServer* m_serv;
	SimpleBattlelistCtrl* m_battlelist;
};

#endif // SIMPLEMPFRAME_H

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
