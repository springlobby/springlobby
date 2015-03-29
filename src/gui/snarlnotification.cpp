/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "snarlnotification.h"


SnarlNotification::SnarlNotification(wxWindow* parent)
    : m_fallback_wrapper(NULL)
{
	if (Snarl::V39::SnarlInterface::IsRunning())
		m_snarl_interface.RegisterApp(_T("CppTest"), _T("C++ test app"), NULL);
}

void SnarlNotification::Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data)
{
	if (Snarl::V39::SnarlInterface::IsRunning()) {
		m_snarl_interface.AddClass(_T("Class1"), _T("Class 1"));
		//		m_snarl_interface.ShowNotification(_T("SpringLobby"), data.second, _T("Some more text"), 10);
		m_snarl_interface.ShowNotification("SpringLobby", "dummy text", "Some more text", 10);
	}
}
