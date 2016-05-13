/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/eventfilter.h>

#include "sltipwin.h"

#if wxUSE_TIPWINDOW

SLTipWindow::SLTipWindow(wxWindow* parent, const wxString& text)
    : wxTipWindow(parent, text)
    , wxEventFilter()
{
	parentWindow = parent;
	creationTime = wxDateTime::GetTimeNow();

	//Register event hook
	wxEvtHandler::AddFilter(this);
	isHookInstalled = true;
}

SLTipWindow::~SLTipWindow()
{
	if (isHookInstalled) {
		wxEvtHandler::RemoveFilter(this);
		isHookInstalled = false;
	}
}

//Hook for events
int SLTipWindow::FilterEvent(wxEvent& hookedEvent)
{
	//Only mouse events are in interest
	if (isMouseEvent(hookedEvent)) {
		//Remove hook before proceed, prevents deadlock
		wxEvtHandler::RemoveFilter(this);
		isHookInstalled = false;

		//Send event to parent control
		wxEvent* clonedEvent = hookedEvent.Clone();
		parentWindow->GetEventHandler()->QueueEvent(clonedEvent);

		//Close tip window
		Close();
		return Event_Processed;
	}

	//Leave events unhandled
	return Event_Skip;
}

bool SLTipWindow::isMouseEvent(wxEvent& hookedEvent)
{
	wxEventType et = hookedEvent.GetEventType();

	//Check for almost all possible mouse events
	if (et == wxEVT_LEFT_DOWN ||
	    et == wxEVT_RIGHT_DOWN ||
	    et == wxEVT_MIDDLE_DOWN ||
	    et == wxEVT_LEFT_DCLICK ||
	    et == wxEVT_RIGHT_DCLICK ||
	    et == wxEVT_MIDDLE_DCLICK ||
	    et == wxEVT_MOUSEWHEEL) {
		return true;
	}

	//Detect mouse motion only after some delay
	if (wxDateTime::GetTimeNow() - creationTime >= MOTION_DETECTION_DELAY) {
		if (et == wxEVT_MOTION) {
			return true;
		}
	}
	return false;
}
#endif
