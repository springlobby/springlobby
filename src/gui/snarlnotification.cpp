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


#include "snarlnotification.h"


SnarlNotification::SnarlNotification(wxWindow* parent):
	m_fallback_wrapper(NULL)
{
	if ( Snarl::V39::SnarlInterface::IsRunning() )
		m_snarl_interface.RegisterApp(_T("CppTest"), _T("C++ test app"), NULL);
}

void SnarlNotification::Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data )
{
	if ( Snarl::V39::SnarlInterface::IsRunning() ) {
		m_snarl_interface.AddClass(_T("Class1"), _T("Class 1"));
//		m_snarl_interface.ShowNotification(_T("SpringLobby"), data.second, _T("Some more text"), 10);
		m_snarl_interface.ShowNotification("SpringLobby", "dummy text", "Some more text", 10);
	}
}
