#include "statusbar.h"
#include "../updater/updatehelper.h"

Statusbar::Statusbar( wxWindow* parent )
	:wxStatusBar( parent, wxNewId() ),
	m_addMessageSink( this, &GetStatusEventSender(UiEvents::addStatusMessage) ),
	m_removeMessageSink( this, &GetStatusEventSender(UiEvents::removeStatusMessage) )

{
	int w[3] = {-1,-1,120};
	SetFieldsCount( 3, w );
	PushStatusText( _T("SpringLobby ") + GetSpringLobbyVersion(), 1 );
}

void Statusbar::OnAddMessage( UiEvents::StatusData data )
{
	PushStatusText( data.first, data.second );
}

void Statusbar::OnRemoveMessage( UiEvents::StatusData data )
{
	PopStatusText( data.second );
}
