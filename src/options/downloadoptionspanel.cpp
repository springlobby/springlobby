#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/intl.h>
#include <wx/radiobut.h>
#include <wx/button.h>

#include "downloadoptionspanel.h"
#include "aui/auimanager.h"
#include "settings.h"
#include "downloader/prdownloader.h"
#include "utils/conversion.h"
#include "utils/controls.h"


BEGIN_EVENT_TABLE( TorrentOptionsPanel, wxScrolledWindow )
END_EVENT_TABLE()

TorrentOptionsPanel::TorrentOptionsPanel( wxWindow* parent )
    : wxScrolledWindow( parent, -1)
{
    GetAui().manager->AddPane( this, wxLEFT, _T("torrentoptionspanel") );

    wxBoxSizer* mainboxsizer = new wxBoxSizer( wxVERTICAL );

    m_gamestart_box = new wxStaticBox(this, -1, _("At game start suspend mode") );
    wxStaticBoxSizer* m_gamestart_box_sizer = new wxStaticBoxSizer(m_gamestart_box , wxVERTICAL );
    m_gamestart_pause = new wxRadioButton(this, ID_GAMESTART_RADIO, _("Pause all torrents"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP  );
    m_gamestart_throttle = new wxRadioButton(this, ID_GAMESTART_RADIO, _("Throttle speeds:") );
    wxBoxSizer* m_gamestart_input_box1  = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* m_gamestart_input_box2  = new wxBoxSizer( wxHORIZONTAL );
    m_gamestart_throttle_up_lbl = new wxStaticText( this, wxID_ANY, _("upload (KB/s)") );
    m_gamestart_throttle_up = new wxTextCtrl(this, ID_INGAME_UP, TowxString(2));
    m_gamestart_throttle_down_lbl = new wxStaticText( this, wxID_ANY, _("download (KB/s)") );
    m_gamestart_throttle_down = new wxTextCtrl(this, ID_INGAME_DOWN, TowxString(2));

    m_gamestart_box_sizer->Add(m_gamestart_pause,  0, wxALL, 5 );
    m_gamestart_box_sizer->Add(m_gamestart_throttle,  0, wxALL, 5 );
    m_gamestart_input_box1->Add(m_gamestart_throttle_up_lbl,  0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_gamestart_input_box1->Add(m_gamestart_throttle_up,  0, wxALL, 5);
    m_gamestart_input_box2->Add(m_gamestart_throttle_down_lbl,  0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    m_gamestart_input_box2->Add(m_gamestart_throttle_down,  0, wxALL, 5);
    m_gamestart_box_sizer->Add(m_gamestart_input_box1,  0, wxALL, 5);
    m_gamestart_box_sizer->Add(m_gamestart_input_box2,  0, wxALL, 5);
    mainboxsizer->Add( m_gamestart_box_sizer, 0, wxALL, 5 );

    wxStaticBox* m_numbers_box = new wxStaticBox(this, -1, _("Numbers") );
    wxStaticBoxSizer* m_numbers_box_sizer = new wxStaticBoxSizer( m_numbers_box , wxVERTICAL );
    wxBoxSizer* up_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxUp = new wxTextCtrl( this, ID_MAXUP, TowxString( sett().GetTorrentUploadRate() ) );
    wxStaticText* m_maxUp_lbl = new wxStaticText( this, wxID_ANY, _("maximum upload speed in KB/sec(-1 for infinite)") );
    up_siter->Add( m_maxUp, 0, wxALL, 5 );
    up_siter->Add( m_maxUp_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_numbers_box_sizer->Add( up_siter, 0, wxALL, 5 );

    wxBoxSizer* down_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxDown = new wxTextCtrl( this, ID_MAXDOWN, TowxString( sett().GetTorrentDownloadRate() ) );
    wxStaticText* m_maxDown_lbl = new wxStaticText( this, wxID_ANY, _("maximum download speed in KB/sec (-1 for infinite)") );
    down_siter->Add( m_maxDown, 0, wxALL, 5 );
    down_siter->Add( m_maxDown_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_numbers_box_sizer->Add( down_siter, 0, wxALL, 5 );

    wxBoxSizer* port_siter = new wxBoxSizer( wxHORIZONTAL );
    m_p2pport = new wxTextCtrl( this, ID_P2PPORT, TowxString( sett().GetTorrentPort() ) );
    wxStaticText* m_p2pport_lbl = new wxStaticText( this, wxID_ANY, _("port used for torrent connections") );
    port_siter->Add( m_p2pport, 0, wxALL, 5 );
    port_siter->Add( m_p2pport_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_numbers_box_sizer->Add( port_siter, 0, wxALL, 5 );

    wxBoxSizer* con_siter = new wxBoxSizer( wxHORIZONTAL );
    m_maxConnections = new wxTextCtrl( this, ID_MAXUP, TowxString( sett().GetTorrentMaxConnections() ) );
    wxStaticText* m_maxConnections_lbl = new wxStaticText( this, wxID_ANY, _("maximum number of concurrent connections") );
    con_siter->Add( m_maxConnections, 0, wxALL, 5 );
    con_siter->Add( m_maxConnections_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_numbers_box_sizer->Add( con_siter, 0, wxALL, 5 );

    mainboxsizer->Add( m_numbers_box_sizer, 0, wxALL, 5 );

    //the lazy man's solution to options not being set correctly at panel creation
    wxCommandEvent dummy;
    OnRestore( dummy );

    SetSizer( mainboxsizer );
    SetScrollRate( SCROLL_RATE, SCROLL_RATE );
    Layout();
}

TorrentOptionsPanel::~TorrentOptionsPanel()
{

}

void TorrentOptionsPanel::OnApply( wxCommandEvent& /*unused*/ )
{

    sett().SetTorrentUploadRate( FromwxString<long>( m_maxUp->GetValue() ) );
    sett().SetTorrentDownloadRate( FromwxString<long>( m_maxDown->GetValue() ) );
    sett().SetTorrentPort( FromwxString<long>( m_p2pport->GetValue() ) );
    sett().SetTorrentMaxConnections( FromwxString<long>( m_maxConnections->GetValue() ) );
    sett().SetTorrentThrottledUploadRate( FromwxString<long>( m_gamestart_throttle_up->GetValue() ) );
    sett().SetTorrentThrottledDownloadRate( FromwxString<long>( m_gamestart_throttle_down->GetValue() ) );

    // if mode == pause selected --> m_gamestart_throttle->GetValue() == 0
    sett().SetTorrentSystemSuspendMode( m_gamestart_throttle->GetValue() );

	prDownloader().UpdateSettings();
}

void TorrentOptionsPanel::OnRestore( wxCommandEvent& /*unused*/ )
{
    m_maxConnections->SetValue( TowxString( sett().GetTorrentMaxConnections() ) );
    m_p2pport->SetValue( TowxString( sett().GetTorrentPort() ) );
    m_maxDown->SetValue( TowxString( sett().GetTorrentDownloadRate() ) );
    m_maxUp->SetValue( TowxString( sett().GetTorrentUploadRate() ) );
    m_gamestart_throttle_up->SetValue( TowxString( sett().GetTorrentThrottledUploadRate() ) );
    m_gamestart_throttle_down->SetValue( TowxString( sett().GetTorrentThrottledDownloadRate( ) ) );
    m_gamestart_throttle->SetValue( sett().GetTorrentSystemSuspendMode() );
}
