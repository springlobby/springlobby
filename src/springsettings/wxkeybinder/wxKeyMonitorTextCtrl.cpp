/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "wxKeyMonitorTextCtrl.h"
#include "keybinder.h"

// event table for wxKeyMonitorTextCtrl
IMPLEMENT_CLASS(wxKeyMonitorTextCtrl, wxTextCtrl)
BEGIN_EVENT_TABLE(wxKeyMonitorTextCtrl, wxTextCtrl)

    // keydown events
    EVT_KEY_DOWN(wxKeyMonitorTextCtrl::OnKey)
    EVT_KEY_UP(wxKeyMonitorTextCtrl::OnKey)

END_EVENT_TABLE()


// ----------------------------------------------------------------------------
// wxKeyMonitorTextCtrl
// ----------------------------------------------------------------------------

void wxKeyMonitorTextCtrl::OnKey(wxKeyEvent &event)
{
    // backspace cannot be used as shortcut key...
#ifndef wxKEYBINDER_ALLOW_BACKSPACE
	if (event.GetKeyCode() == WXK_BACK) {

        // this text ctrl contains something and the user pressed backspace...
        // we must delete the keypress...
        Clear();
        return;
    }
#endif

    if (event.GetEventType() == wxEVT_KEY_DOWN ||
        (event.GetEventType() == wxEVT_KEY_UP && !IsValidKeyComb())) {

        // the user pressed some key combination which must be displayed
        // in this text control.... or he has just stopped pressing a
        // modifier key like shift, ctrl or alt without adding any
        // other alphanumeric char, thus generating an invalid keystroke
        // which must be cleared out...
        SetValue(wxKeyBind::GetKeyStrokeString(event));
        SetInsertionPointEnd();
    }
}

int wxKeyMonitorTextCtrl::overflow(int i)
{
  return wxTextCtrl::overflow(i);
}

