#include "torrentoptionspanel.h"

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>

#include "settings.h"
#include "torrentwrapper.h"
#include "utils.h"


BEGIN_EVENT_TABLE( TorrentOptionsPanel, wxPanel )
  EVT_BUTTON( ID_APPLY, TorrentOptionsPanel::OnApply )
  EVT_CHECKBOX( ID_ENABLEP2P, TorrentOptionsPanel::OnEnableP2P)
  EVT_BUTTON( ID_RESTORE, TorrentOptionsPanel::OnRestore )
END_EVENT_TABLE()


TorrentOptionsPanel::TorrentOptionsPanel( wxWindow* parent)
    : wxPanel( parent, -1)
{
    wxBoxSizer* mainboxsizer = new wxBoxSizer( wxVERTICAL );

    m_enableP2P = new wxCheckBox( this, ID_ENABLEP2P, _("Enable peer to peer download system"));
    m_enableP2P->SetValue( sett().GetTorrentSystemEnabled() );
    mainboxsizer->Add( 0, wxALL, 5 );

    m_maxUp = new wxTextCtrl( this, ID_MAXUP, i2s( sett().GetTorrentUploadRate() ) );
    mainboxsizer->Add( m_maxUp, 0, wxALL, 5 );

    m_maxDown = new wxTextCtrl( this, ID_MAXDOWN, i2s( sett().GetTorrentDownloadRate() ) );
    mainboxsizer->Add( m_maxDown, 0, wxALL, 5 );

    m_p2pport = new wxTextCtrl( this, ID_P2PPORT, i2s( sett().GetTorrentPort() ) );
    mainboxsizer->Add( m_p2pport, 0, wxALL, 5 );

    m_maxConnections = new wxTextCtrl( this, ID_MAXUP, i2s( sett().GetTorrentMaxConnections() ) );
    mainboxsizer->Add( m_maxConnections, 0, wxALL, 5 );

    EnableSettings( sett().GetTorrentSystemEnabled() );
}

TorrentOptionsPanel::~TorrentOptionsPanel()
{

}

void TorrentOptionsPanel::EnableSettings( bool enable)
{
    m_maxUp->Enable( enable );
    m_maxDown->Enable( enable );
    m_p2pport->Enable( enable );
    m_maxConnections->Enable( enable );
}

void TorrentOptionsPanel::OnEnableP2P( wxCommandEvent& event )
{
    EnableSettings( m_enableP2P->IsChecked() );

}


void TorrentOptionsPanel::OnMaxUp( wxCommandEvent& event ){}
void TorrentOptionsPanel::OnMaxDown( wxCommandEvent& event ){}
void TorrentOptionsPanel::OnP2PPort( wxCommandEvent& event ){}
void TorrentOptionsPanel::OnMaxConnections( wxCommandEvent& event ){}

void TorrentOptionsPanel::OnApply( wxCommandEvent& event )
{
    sett().SetTorrentSystemEnabled( m_enableP2P->IsChecked() );
    sett().SetTorrentUploadRate( s2l( m_maxUp->GetValue() ) );
    sett().SetTorrentDownloadRate( s2l( m_maxDown->GetValue() ) );
    sett().SetTorrentPort( s2l( m_p2pport->GetValue() ) );
    sett().SetTorrentMaxConnections( s2l( m_maxConnections->GetValue() ) );
    if (!torrent()->IsConnectedToP2PSystem() && m_enableP2P->IsChecked() ) //TODO add server connection test
        torrent()->ConnectToP2PSystem();
    torrent()->UpdateSettings();
}

void TorrentOptionsPanel::OnRestore( wxCommandEvent& event )
{
    m_maxConnections->SetValue( sett().GetTorrentMaxConnections() );
    m_p2pport->SetValue( sett().GetTorrentPort() );
    m_maxDown->SetValue( sett().GetTorrentDownloadRate() ):
    m_maxUp->SetValue( sett().GetTorrentUploadRate() );
    m_enableP2P->SetChecked( sett().GetTorrentSystemEnabled() );
}
