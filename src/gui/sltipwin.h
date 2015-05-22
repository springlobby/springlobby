/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H
#define SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H

#include <wx/platform.h>
#if wxUSE_TIPWINDOW
#include <wx/tipwin.h>
#include <wx/eventfilter.h>

//! A wxTipWindow that gets canceled on mousewheel events
class SLTipWindow : public wxTipWindow, public wxEventFilter
{
public:
	SLTipWindow(wxWindow* parent, const wxString& text);
	virtual ~SLTipWindow();
        
	virtual int FilterEvent(wxEvent&);
	
private:
	bool isMouseEvent(wxEvent&);
	
private:
	wxWindow* parentWindow;
	bool isHookInstalled;
	time_t creationTime;
	
private:
	const int MOTION_DETECTION_DELAY = 1;
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_UTIL_TIPWIN_H
