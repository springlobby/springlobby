#include "simplempframe.h"
#include "simplebattlelistctrl.h"
#include "../tasserver.h"
#include "../settings.h"
#include "../ui.h"

#include <wx/sizer.h>

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

	// Connect
	m_serv->Connect( servername, host, port );

	wxBoxSizer* h = new wxBoxSizer( wxHORIZONTAL );
	m_battlelist = new SimpleBattlelistCtrl( this );
	m_battlelist->SetSize(100,GetSize().GetHeight());
	h->Add( m_battlelist, 0, wxALL, 25 );
	SetSizer( h );
	Layout();
}

SimpleMPFrame::~SimpleMPFrame()
{

}
