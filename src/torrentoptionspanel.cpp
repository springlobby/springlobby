#ifndef NO_TORRENT_SYSTEM

#include "torrentoptionspanel.h"

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/intl.h>
#include <wx/radiobut.h>
#include <wx/button.h>

#include "settings.h"
#include "ui.h"
#include "torrentwrapper.h"
#include "utils.h"


BEGIN_EVENT_TABLE( TorrentOptionsPanel, wxPanel )
  EVT_BUTTON( ID_APPLY, TorrentOptionsPanel::OnApply )
<<<<<<< HEAD:src/torrentoptionspanel.cpp
//  EVT_CHECKBOX( ID_ENABLEP2P, TorrentOptionsPanel::OnEnableP2P)
=======
>>>>>>> bd/torrent:src/torrentoptionspanel.cpp
  EVT_BUTTON( ID_RESTORE, TorrentOptionsPanel::OnRestore )
  EVT_BUTTON( ID_MAN_START, TorrentOptionsPanel::OnManStart )
  EVT_BUTTON( ID_MAN_STOP, TorrentOptionsPanel::OnManStop )
END_EVENT_TABLE()


TorrentOptionsPanel::TorrentOptionsPanel( wxWindow* parent, Ui& ui)
    : wxPanel( parent, -1), m_ui(ui)
{
    wxBoxSizer* mainboxsizer = new wxBoxSizer( wxVERTICAL );

<<<<<<< HEAD:src/torrentoptionspanel.cpp
=======
    wxBoxSizer* enable_siter = new wxBoxSizer( wxHORIZONTAL );
    m_enableP2P = new wxCheckBox( this, ID_ENABLEP2P, _("Enable peer to peer download system"));
    m_enableP2P->SetValue( true );
    mainboxsizer->Add( m_enableP2P, 0, wxALL, 5 );
>>>>>>> bd/torrent:src/torrentoptionspanel.cpp

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


    wxBoxSizer* m_sys_ctrl_box = new wxBoxSizer( wxHORIZONTAL );
    m_sys_start = new wxButton( this, ID_MAN_START, _("Start system") );
    m_sys_stop = new wxButton( this, ID_MAN_STOP, _("Stop system") );
    m_sys_ctrl_box->Add(m_sys_stop );
    m_sys_ctrl_box->Add(m_sys_start );

    m_autostart_box = new wxStaticBox(this, -1, _("Torrent system autostart") );
    wxStaticBoxSizer* m_autostart_box_sizer  = new wxStaticBoxSizer( m_autostart_box, wxVERTICAL );
    m_autostart_logon = new wxRadioButton (this, ID_AUTOSTART_RADIO, _("at lobby connection (default)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP  );
    m_autostart_start = new wxRadioButton (this, ID_AUTOSTART_RADIO, _("at lobby start"));
    m_autostart_manual = new wxRadioButton (this, ID_AUTOSTART_RADIO, _("manual") );
    m_autostart_box_sizer->Add(m_autostart_logon);
    m_autostart_box_sizer->Add(m_autostart_start);
    m_autostart_box_sizer->Add(m_autostart_manual);
//    m_autostart_box_sizer->Add(NULL);
    m_autostart_box_sizer->Add(m_sys_ctrl_box);
    mainboxsizer->Add( m_autostart_box_sizer, 0, wxALL, 5 );

    m_gamestart_box = new wxStaticBox(this, -1, _("At game start suspend mode") );
    wxStaticBoxSizer* m_gamestart_box_sizer = new wxStaticBoxSizer(m_gamestart_box , wxVERTICAL );
    m_gamestart_pause = new wxRadioButton(this, ID_GAMESTART_RADIO, _("Pause all torrents"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP  );
    m_gamestart_throttle = new wxRadioButton(this, ID_GAMESTART_RADIO, _("Throttle speeds:") );
    wxBoxSizer* m_gamestart_input_box1  = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* m_gamestart_input_box2  = new wxBoxSizer( wxHORIZONTAL );
    m_gamestart_throttle_up_lbl = new wxStaticText( this, wxID_ANY, _("upload (KB/s)") );
    m_gamestart_throttle_up = new wxTextCtrl(this, ID_INGAME_UP, i2s(2));
    m_gamestart_throttle_down_lbl = new wxStaticText( this, wxID_ANY, _("download (KB/s)") );
    m_gamestart_throttle_down = new wxTextCtrl(this, ID_INGAME_DOWN, i2s(2));

    m_gamestart_box_sizer->Add(m_gamestart_pause );
    m_gamestart_box_sizer->Add(m_gamestart_throttle );
    m_gamestart_input_box1->Add(m_gamestart_throttle_up_lbl );
    m_gamestart_input_box1->Add(m_gamestart_throttle_up);
    m_gamestart_input_box2->Add(m_gamestart_throttle_down_lbl);
    m_gamestart_input_box2->Add(m_gamestart_throttle_down);
    m_gamestart_box_sizer->Add(m_gamestart_input_box1);
    m_gamestart_box_sizer->Add(m_gamestart_input_box2);
    mainboxsizer->Add( m_gamestart_box_sizer, 0, wxALL, 5 );

    EnableSettings( sett().GetTorrentSystemEnabled() );
    EnableStartStopButtons( torrent()->IsConnectedToP2PSystem() );
    wxCommandEvent dummy;
    OnRestore(dummy);

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

//void TorrentOptionsPanel::OnEnableP2P( wxCommandEvent& event )
//{
//    EnableSettings( m_enableP2P->IsChecked() );
//
//}

//TODO wtf did i add these for
void TorrentOptionsPanel::OnMaxUp( wxCommandEvent& event ){}
void TorrentOptionsPanel::OnMaxDown( wxCommandEvent& event ){}
void TorrentOptionsPanel::OnP2PPort( wxCommandEvent& event ){}
void TorrentOptionsPanel::OnMaxConnections( wxCommandEvent& event ){}


void TorrentOptionsPanel::OnApply( wxCommandEvent& event )
{

    sett().SetTorrentSystemAutoStartMode( 0 );
    sett().SetTorrentUploadRate( s2l( m_maxUp->GetValue() ) );
    sett().SetTorrentDownloadRate( s2l( m_maxDown->GetValue() ) );
    sett().SetTorrentPort( s2l( m_p2pport->GetValue() ) );
    sett().SetTorrentMaxConnections( s2l( m_maxConnections->GetValue() ) );
//    if (!torrent()->IsConnectedToP2PSystem() && m_enableP2P->IsChecked() && m_ui.IsConnected() )
//        torrent()->ConnectToP2PSystem();
//    else if ( torrent()->IsConnectedToP2PSystem() && !m_enableP2P->IsChecked() )
//        torrent()->DisconnectToP2PSystem();
    torrent()->UpdateSettings();
}

void TorrentOptionsPanel::OnRestore( wxCommandEvent& event )
{
    m_maxConnections->SetValue( i2s( sett().GetTorrentMaxConnections() ) );
    m_p2pport->SetValue( i2s( sett().GetTorrentPort() ) );
    m_maxDown->SetValue( i2s( sett().GetTorrentDownloadRate() ) );
    m_maxUp->SetValue( i2s( sett().GetTorrentUploadRate() ) );

}

void TorrentOptionsPanel::OnManStart( wxCommandEvent& event )
{
    torrent()->ConnectToP2PSystem();
    EnableStartStopButtons(true);
}

void TorrentOptionsPanel::EnableStartStopButtons( bool system_is_running)
{
    m_sys_start->Enable(!system_is_running);
    m_sys_stop->Enable(system_is_running);
}

void TorrentOptionsPanel::OnManStop( wxCommandEvent& event )
{
    torrent()->DisconnectToP2PSystem();
    EnableStartStopButtons(false);
}


#endif
