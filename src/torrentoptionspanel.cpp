#ifndef NO_TORRENT_SYSTEM

#include "torrentoptionspanel.h"

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/intl.h>

#include "settings.h"
#include "ui.h"
#include "torrentwrapper.h"
#include "utils.h"


BEGIN_EVENT_TABLE( TorrentOptionsPanel, wxPanel )
  EVT_BUTTON( ID_APPLY, TorrentOptionsPanel::OnApply )
  EVT_CHECKBOX( ID_ENABLEP2P, TorrentOptionsPanel::OnEnableP2P)
  EVT_BUTTON( ID_RESTORE, TorrentOptionsPanel::OnRestore )
END_EVENT_TABLE()


TorrentOptionsPanel::TorrentOptionsPanel( wxWindow* parent, Ui& ui)
    : wxPanel( parent, -1), m_ui(ui)
{
    wxBoxSizer* mainboxsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* enable_siter = new wxBoxSizer( wxHORIZONTAL );
    m_enableP2P = new wxCheckBox( this, ID_ENABLEP2P, _("Enable peer to peer download system"));
    m_enableP2P->SetValue( sett().GetTorrentSystemEnabled() );
    mainboxsizer->Add( m_enableP2P, 0, wxALL, 5 );

    wxBoxSizer* up_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxUp = new wxTextCtrl( this, ID_MAXUP, i2s( sett().GetTorrentUploadRate() ) );
    wxStaticText* m_maxUp_lbl = new wxStaticText( this, wxID_ANY, _("maximum upload speed in KB/sec(-1 for infinite)") );
    up_siter->Add( m_maxUp, 0, wxALL, 5 );
    up_siter->Add( m_maxUp_lbl, 0, wxALL, 5 );
    mainboxsizer->Add( up_siter, 0, wxALL, 5 );

    wxBoxSizer* down_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxDown = new wxTextCtrl( this, ID_MAXDOWN, i2s( sett().GetTorrentDownloadRate() ) );
    wxStaticText* m_maxDown_lbl = new wxStaticText( this, wxID_ANY, _("maximum download speed in KB/sec (-1 for infinite)") );
    down_siter->Add( m_maxDown, 0, wxALL, 5 );
    down_siter->Add( m_maxDown_lbl, 0, wxALL, 5 );
    mainboxsizer->Add( down_siter, 0, wxALL, 5 );

    wxBoxSizer* port_siter = new wxBoxSizer( wxHORIZONTAL );
    m_p2pport = new wxTextCtrl( this, ID_P2PPORT, i2s( sett().GetTorrentPort() ) );
    wxStaticText* m_p2pport_lbl = new wxStaticText( this, wxID_ANY, _("port used for torrent connections") );
    port_siter->Add( m_p2pport, 0, wxALL, 5 );
    port_siter->Add( m_p2pport_lbl, 0, wxALL, 5 );
    mainboxsizer->Add( port_siter, 0, wxALL, 5 );

    wxBoxSizer* con_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxConnections = new wxTextCtrl( this, ID_MAXUP, i2s( sett().GetTorrentMaxConnections() ) );
    wxStaticText* m_maxConnections_lbl = new wxStaticText( this, wxID_ANY, _("maximum number of concurrent connections") );
    con_siter->Add( m_maxConnections, 0, wxALL, 5 );
    con_siter->Add( m_maxConnections_lbl, 0, wxALL, 5 );
    mainboxsizer->Add( con_siter, 0, wxALL, 5 );

    EnableSettings( sett().GetTorrentSystemEnabled() );

    SetSizer( mainboxsizer );
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

//TODO wtf did i add these
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
    if (!torrent()->IsConnectedToP2PSystem() && m_enableP2P->IsChecked() && m_ui.IsConnected() )
        torrent()->ConnectToP2PSystem();
    else if ( torrent()->IsConnectedToP2PSystem() && !m_enableP2P->IsChecked() )
        torrent()->DisconnectToP2PSystem();
    torrent()->UpdateSettings();
}

void TorrentOptionsPanel::OnRestore( wxCommandEvent& event )
{
    m_maxConnections->SetValue( i2s( sett().GetTorrentMaxConnections() ) );
    m_p2pport->SetValue( i2s( sett().GetTorrentPort() ) );
    m_maxDown->SetValue( i2s( sett().GetTorrentDownloadRate() ) );
    m_maxUp->SetValue( i2s( sett().GetTorrentUploadRate() ) );
    m_enableP2P->SetValue( sett().GetTorrentSystemEnabled() );
}

#endif
