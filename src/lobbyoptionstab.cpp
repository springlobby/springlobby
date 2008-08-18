#include "lobbyoptionstab.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>

#include "settings.h"
//#include


LobbyOptionsTab::LobbyOptionsTab(wxWindow* parent)
    : wxScrolledWindow( parent, -1 )
{
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );
    wxStaticBoxSizer* m_autojoin_sizer= new wxStaticBoxSizer ( wxVERTICAL, this, _("Autoconnect") );
    m_autoconnect_label = new wxStaticText ( this, -1, _("If checked, SpringLobby will automatically log on to the last used server") );
    m_autojoin = new wxCheckBox( this, -1, _("Autoconnect on lobby start"), wxDefaultPosition, wxDefaultSize, 0 );
    m_autojoin->SetValue( sett().GetAutoConnect() );
    m_autojoin_sizer->Add( m_autoconnect_label, 1, wxEXPAND | wxALL, 5 );
    m_autojoin_sizer->Add( m_autojoin, 0, wxEXPAND | wxALL, 5 );

    wxStaticBoxSizer* m_reportstats_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Report statistics") );
    m_reportstats_label = new wxStaticText ( this, -1, _("By default SpringLobby will send some statistics (OS,SpringLobby version) to server,\nto both make helping you in case of problems easier and inform of critical updates.\nUncheck to disable.") );
    m_reportstats = new wxCheckBox( this, -1, _("report statistics"), wxDefaultPosition, wxDefaultSize, 0 );
    m_reportstats->SetValue( sett().GetReportStats() );
    m_reportstats_sizer->Add( m_reportstats_label, 1, wxEXPAND|wxALL, 5);
    m_reportstats_sizer->Add( m_reportstats, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_autojoin_sizer, 0, wxALL, 15 );
    m_main_sizer->Add( m_reportstats_sizer, 0, wxALL, 15 );
    SetSizer( m_main_sizer );
}

LobbyOptionsTab::~LobbyOptionsTab()
{
    //dtor
}

void LobbyOptionsTab::OnApply(wxCommandEvent& event)
{
    sett().SetAutoConnect( m_autojoin->IsChecked() );
    sett().SetReportStats( m_reportstats->GetValue() );
}


void LobbyOptionsTab::OnRestore(wxCommandEvent& event)
{
    m_autojoin->SetValue( sett().GetAutoConnect() );
    m_reportstats->SetValue( sett().GetReportStats() );
}

