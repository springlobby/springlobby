/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H
#define SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H

#include <wx/platform.h>
#if wxUSE_TIPWINDOW
#include <wx/tipwin.h>

//! A wxTipWindow that gets canceled on mousewheel events
class SLTipWindow : public wxTipWindow
{
public:
	SLTipWindow(wxWindow* parent, const wxString& text);

	void Cancel(wxMouseEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H
