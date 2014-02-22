/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_STATUSBAR_H
#define SPRINGLOBBY_HEADERGUARD_STATUSBAR_H

#include <wx/statusbr.h>
#include "../utils/uievents.h"

class Statusbar : public wxStatusBar
{
public:
	Statusbar( wxWindow* parent );
	void OnAddMessage	( UiEvents::StatusData data );
	void OnRemoveMessage( UiEvents::StatusData data );

protected:
	static const wxEventType UpdateMsgEvt;
	void OnUpdateMsg(wxCommandEvent& evt);
	EventReceiverFunc< Statusbar, UiEvents::StatusData, &Statusbar::OnAddMessage >
		m_addMessageSink;
	EventReceiverFunc< Statusbar, UiEvents::StatusData, &Statusbar::OnRemoveMessage >
		m_removeMessageSink;

	DECLARE_EVENT_TABLE()
};

#endif // STATUSBAR_H
