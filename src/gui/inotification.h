#ifndef INOTIFICATION_H
#define INOTIFICATION_H

#include <cstddef>
#include "../utils/uievents.h"

class wxWindow;
class ScreenPosition;
class wxBitmap;

class INotification
{
	public:
		INotification(){}
		virtual ~INotification(){}
		virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data ) = 0;
};

#endif // INOTIFICATION_H

/**
	This file is part of SpringLobby,
	Copyright (C) 2007-2010

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

