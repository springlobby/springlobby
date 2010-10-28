#include "statusbar.h"
#include "../updater/updatehelper.h"
#include "../utils/platform.h"
#include "../utils/conversion.h"

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

void Statusbar::OnAddMessage( UiEvents::StatusData data )
{
	PushStatusText( data.first, data.second );
}

void Statusbar::OnRemoveMessage( UiEvents::StatusData data )
{
	PopStatusText( data.second );
}
