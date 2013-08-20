#include "statusbar.h"
#include "../updater/updatehelper.h"
#include "../utils/platform.h"
#include "../utils/conversion.h"
#include <wx/event.h>

Statusbar::Statusbar( wxWindow* parent )
	:wxStatusBar( parent, wxNewId() ),
	m_addMessageSink( this, &GetStatusEventSender(UiEvents::addStatusMessage) ),
	m_removeMessageSink( this, &GetStatusEventSender(UiEvents::removeStatusMessage) )

{
	int w[3] = {-1,-1,120};
	SetFieldsCount( 3, w );
	PushStatusText( wxFormat( _T("%s %s") )
									  % GetAppName()
									  % GetSpringLobbyVersion(),
					1 );
}
DEFINE_EVENT_TYPE(PUSH_STATUS_MSG)
DEFINE_EVENT_TYPE(POP_STATUS_MSG)

BEGIN_EVENT_TABLE( Statusbar, wxStatusBar )
	EVT_COMMAND(wxID_ANY, PUSH_STATUS_MSG, Statusbar::OnUpdateMsg)
	EVT_COMMAND(wxID_ANY, POP_STATUS_MSG, Statusbar::OnUpdateMsg)
END_EVENT_TABLE()

void Statusbar::OnUpdateMsg(wxCommandEvent& evt) {
	const int pos = evt.GetInt();
	const int id = evt.GetId();
	SetStatusText(evt.GetString(), pos);
//FIXME: Push/Pop seems to have never worked as it should, what should the code do?
/*
	if (id == PUSH_STATUS_MSG) {
		const wxString& msg = evt.GetString();
		PushStatusText(msg, pos);
	} else if (id == POP_STATUS_MSG) {
		PopStatusText(pos);
	}*/
}

void Statusbar::OnAddMessage( UiEvents::StatusData data )
{
	// is called from a thread, wxPostEvent used for thread-safety!
	wxCommandEvent evt(PUSH_STATUS_MSG, GetId());
	evt.SetEventObject(this);

	evt.SetString(data.first);
	evt.SetInt(data.second);
	wxPostEvent(this, evt);
}

void Statusbar::OnRemoveMessage( UiEvents::StatusData data )
{
	// is called from a thread, wxPostEvent used for thread-safety!
	wxCommandEvent evt(POP_STATUS_MSG, GetId());
	evt.SetEventObject(this);

	evt.SetString(data.first);
	evt.SetInt(data.second);
	wxPostEvent(this, evt);
}

