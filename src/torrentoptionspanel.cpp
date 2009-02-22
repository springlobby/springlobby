#ifndef NO_TORRENT_SYSTEM

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/intl.h>
#include <wx/radiobut.h>
#include <wx/button.h>

#include "torrentoptionspanel.h"
#include "aui/auimanager.h"
#include "settings.h"
#include "ui.h"
#include "torrentwrapper.h"
#include "utils.h"


BEGIN_EVENT_TABLE( TorrentOptionsPanel, wxScrolledWindow )
END_EVENT_TABLE()

TorrentOptionsPanel::TorrentOptionsPanel( wxWindow* parent, Ui& ui)
    : wxScrolledWindow( parent, -1), m_ui(ui)
{
    GetAui().manager->AddPane( this, wxLEFT, _T("torrentoptionspanel") );

    wxBoxSizer* mainboxsizer = new wxBoxSizer( wxVERTICAL );

    m_autostart_box = new wxStaticBox(this, -1, _("Torrent system autostart") );
    wxStaticBoxSizer* m_autostart_box_sizer  = new wxStaticBoxSizer( m_autostart_box, wxVERTICAL );
    m_autostart_logon = new wxRadioButton (this, ID_AUTOSTART_RADIO, _("at lobby connection (default)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP  );
    m_autostart_start = new wxRadioButton (this, ID_AUTOSTART_RADIO, _("at lobby start"));
    m_autostart_manual = new wxRadioButton (this, ID_AUTOSTART_RADIO, _("manual") );

    SetAutoStartRadio();

    m_autostart_box_sizer->Add(m_autostart_logon,  0, wxALL, 5);
    m_autostart_box_sizer->Add(m_autostart_start,  0, wxALL, 5);
    m_autostart_box_sizer->Add(m_autostart_manual,  0, wxALL, 5);
//    m_autostart_box_sizer->Add(NULL);
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

    m_gamestart_box_sizer->Add(m_gamestart_pause,  0, wxALL, 5 );
    m_gamestart_box_sizer->Add(m_gamestart_throttle,  0, wxALL, 5 );
    m_gamestart_input_box1->Add(m_gamestart_throttle_up_lbl,  0, wxALL, 5 );
    m_gamestart_input_box1->Add(m_gamestart_throttle_up,  0, wxALL, 5);
    m_gamestart_input_box2->Add(m_gamestart_throttle_down_lbl,  0, wxALL, 5);
    m_gamestart_input_box2->Add(m_gamestart_throttle_down,  0, wxALL, 5);
    m_gamestart_box_sizer->Add(m_gamestart_input_box1,  0, wxALL, 5);
    m_gamestart_box_sizer->Add(m_gamestart_input_box2,  0, wxALL, 5);
    mainboxsizer->Add( m_gamestart_box_sizer, 0, wxALL, 5 );

    wxStaticBox* m_numbers_box = new wxStaticBox(this, -1, _("Numbers") );
    wxStaticBoxSizer* m_numbers_box_sizer = new wxStaticBoxSizer( m_numbers_box , wxVERTICAL );
    wxBoxSizer* up_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxUp = new wxTextCtrl( this, ID_MAXUP, i2s( sett().GetTorrentUploadRate() ) );
    wxStaticText* m_maxUp_lbl = new wxStaticText( this, wxID_ANY, _("maximum upload speed in KB/sec(-1 for infinite)") );
    up_siter->Add( m_maxUp, 0, wxALL, 5 );
    up_siter->Add( m_maxUp_lbl, 0, wxALL, 5 );
    m_numbers_box_sizer->Add( up_siter, 0, wxALL, 5 );

    wxBoxSizer* down_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxDown = new wxTextCtrl( this, ID_MAXDOWN, i2s( sett().GetTorrentDownloadRate() ) );
    wxStaticText* m_maxDown_lbl = new wxStaticText( this, wxID_ANY, _("maximum download speed in KB/sec (-1 for infinite)") );
    down_siter->Add( m_maxDown, 0, wxALL, 5 );
    down_siter->Add( m_maxDown_lbl, 0, wxALL, 5 );
    m_numbers_box_sizer->Add( down_siter, 0, wxALL, 5 );

    wxBoxSizer* port_siter = new wxBoxSizer( wxHORIZONTAL );
    m_p2pport = new wxTextCtrl( this, ID_P2PPORT, i2s( sett().GetTorrentPort() ) );
    wxStaticText* m_p2pport_lbl = new wxStaticText( this, wxID_ANY, _("port used for torrent connections") );
    port_siter->Add( m_p2pport, 0, wxALL, 5 );
    port_siter->Add( m_p2pport_lbl, 0, wxALL, 5 );
    m_numbers_box_sizer->Add( port_siter, 0, wxALL, 5 );

    wxBoxSizer* con_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxConnections = new wxTextCtrl( this, ID_MAXUP, i2s( sett().GetTorrentMaxConnections() ) );
    wxStaticText* m_maxConnections_lbl = new wxStaticText( this, wxID_ANY, _("maximum number of concurrent connections") );
    con_siter->Add( m_maxConnections, 0, wxALL, 5 );
    con_siter->Add( m_maxConnections_lbl, 0, wxALL, 5 );
    m_numbers_box_sizer->Add( con_siter, 0, wxALL, 5 );

    mainboxsizer->Add( m_numbers_box_sizer, 0, wxALL, 5 );


    SetSizer( mainboxsizer );
    SetScrollRate( 3, 3 );
    Layout();
}

TorrentOptionsPanel::~TorrentOptionsPanel()
{

}


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
    sett().SetTorrentThrottledUploadRate( s2l( m_gamestart_throttle_up->GetValue() ) );
    sett().SetTorrentThrottledDownloadRate( s2l( m_gamestart_throttle_down->GetValue() ) );

    //last radio box value, will be changed if other box is pressed
    int autostart_mode = 2;
    if ( m_autostart_logon->GetValue() )
        autostart_mode = 0;
    else if ( m_autostart_start->GetValue() )
        autostart_mode = 1;
    sett().SetTorrentSystemAutoStartMode( autostart_mode );

    // if mode == pause selected --> m_gamestart_throttle->GetValue() == 0
    sett().SetTorrentSystemSuspendMode( m_gamestart_throttle->GetValue() );

    torrent().UpdateSettings();
}

void TorrentOptionsPanel::OnRestore( wxCommandEvent& event )
{
    m_maxConnections->SetValue( i2s( sett().GetTorrentMaxConnections() ) );
    m_p2pport->SetValue( i2s( sett().GetTorrentPort() ) );
    m_maxDown->SetValue( i2s( sett().GetTorrentDownloadRate() ) );
    m_maxUp->SetValue( i2s( sett().GetTorrentUploadRate() ) );
    m_gamestart_throttle_up->SetValue( i2s( sett().GetTorrentThrottledUploadRate() ) );
    m_gamestart_throttle_down->SetValue( i2s( sett().GetTorrentThrottledDownloadRate( ) ) );
    SetAutoStartRadio();
    m_gamestart_throttle->SetValue( sett().GetTorrentSystemSuspendMode() );
}

void TorrentOptionsPanel::SetAutoStartRadio()
{
    switch ( sett().GetTorrentSystemAutoStartMode() )
    {
        case 1:
            m_autostart_logon->SetValue( true );
            break;
        case 2:
            m_autostart_manual->SetValue( true );
            break;
        default:
            m_autostart_logon->SetValue( true );
    }
}

#endif
