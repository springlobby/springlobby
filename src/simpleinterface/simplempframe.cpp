#include "simplempframe.h"
#include "../tasserver.h"
#include "../settings.h"
#include "../ui.h"

SimpleMPFrame::SimpleMPFrame( wxWindow* parent )
	: CustomizedFrame( parent )
{
	m_serv = new TASServer(1);
	serverSelector().SetCurrentServer( m_serv );
	wxString servername = sett().GetDefaultServer();
	wxString username = sett().GetServerAccountNick( servername );
	wxString password = sett().GetServerAccountPass( servername );

	m_serv->SetUsername( username );
	m_serv->SetPassword( password );

	if ( sett().GetServerAccountSavePass( servername ) )
	{
		if ( m_serv->IsPasswordHash(password) ) sett().SetServerAccountPass( servername, password );
		else sett().SetServerAccountPass( servername, m_serv->GetPasswordHash( password ) );
	}
	else
	{
		sett().SetServerAccountPass( servername, _T("") );
	}
	sett().SaveSettings();
	wxString host;
	int port;
	host = sett().GetServerHost( servername );
	port = sett().GetServerPort( servername );

//    GetServer().uidata.panel = m_main_win->GetChatTab().AddChatPanel( *m_serv, servername );
//    GetServer().uidata.panel->StatusMessage( _T("Connecting to server ") + servername + _T("...") );

	// Connect
	m_serv->Connect( servername, host, port );
}

SimpleMPFrame::~SimpleMPFrame()
{

}
