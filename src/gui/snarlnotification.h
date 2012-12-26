#ifndef SNARLNOTIFICATION_H
#define SNARLNOTIFICATION_H

#ifdef __WXMSW__

#include "inotification.h"
#include "snarl/SnarlInterface.h"

class ToasterNotification;

class SnarlNotification : public INotification
{
	public:
		SnarlNotification(wxWindow* parent);
		virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data );

	protected:
		ToasterNotification* m_fallback_wrapper;
		Snarl::V39::SnarlInterface m_snarl_interface;

};

#endif // __WXMSW__
#endif // SNARLNOTIFICATION_H

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
