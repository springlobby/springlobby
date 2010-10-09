#include "statusbar.h"
#include "../updater/updatehelper.h"
#include "../utils/platform.h"

Statusbar::Statusbar( wxWindow* parent )
	:wxStatusBar( parent, wxNewId() ),
	m_addMessageSink( this, &GetStatusEventSender(UiEvents::addStatusMessage) ),
	m_removeMessageSink( this, &GetStatusEventSender(UiEvents::removeStatusMessage) )

{
	int w[3] = {-1,-1,120};
	SetFieldsCount( 3, w );
	PushStatusText( wxString::Format( _T("%s %s"),
									  GetAppName().c_str(),
									  GetSpringLobbyVersion().c_str() ),
					1 );
}

void Statusbar::OnAddMessage( UiEvents::StatusData data )
{
	PushStatusText( data.first, data.second );
}

void Statusbar::OnRemoveMessage( UiEvents::StatusData data )
{
	PopStatusText( data.second );
}
