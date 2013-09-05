#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/tooltip.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>

#include "lobbyoptionstab.h"
#include "nonportable.h"
#include "settings.h"
#include "utils/customdialogs.h"
#include "utils/controls.h"
#include "utils/platform.h"
#include "aui/auimanager.h"
#include "ui.h"
#include "mainwindow.h"


BEGIN_EVENT_TABLE(LobbyOptionsTab, wxPanel)

    EVT_BUTTON (        SPRING_WEBBROWSE,   LobbyOptionsTab::OnBrowseWeb    )
    EVT_BUTTON (        ID_BUT_EDITOR,      LobbyOptionsTab::OnBrowseEditor )
    EVT_RADIOBUTTON(    SPRING_DEFWEB,      LobbyOptionsTab::OnDefaultWeb   )

END_EVENT_TABLE()

LobbyOptionsTab::LobbyOptionsTab(wxWindow* parent)
    : wxScrolledWindow( parent, -1 ),
    m_show_tooltips_label( 0 )
{
    GetAui().manager->AddPane( this, wxLEFT, _T("lobbyoptionstab") );

    m_main_sizer = new wxBoxSizer ( wxVERTICAL );

/* ================================
   * Web browser
   */

    wxStaticBox* m_web_box = new wxStaticBox( this , -1, _("Web Browser") );
    m_web_loc_text = new wxStaticText( this, -1, _("Web Browser") );

    m_web_def_radio = new wxRadioButton( this, SPRING_DEFWEB, _("Default Browser."),
                 wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_web_def_radio->SetToolTip(TE(_("Use your system-wide browser preference")));

    m_web_spec_radio = new wxRadioButton( this, SPRING_DEFWEB, _("Specify:") );
    m_web_spec_radio->SetToolTip(TE(_("Specify the web browser you want to use")));

    m_web_edit = new wxTextCtrl( this, -1, sett().GetWebBrowserPath() );

    m_web_browse_btn = new wxButton( this, SPRING_WEBBROWSE, _("Browse") );
    m_web_browse_btn->SetToolTip(TE(_("Use a file dialog to find the web browser")));

    if ( sett().GetWebBrowserUseDefault() ) m_web_def_radio->SetValue( true );
    else m_web_spec_radio->SetValue( true );

    m_web_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_web_loc_sizer->Add( m_web_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
    m_web_loc_sizer->Add( m_web_edit, 1, wxEXPAND );
    m_web_loc_sizer->Add( m_web_browse_btn );

    m_web_box_sizer = new wxStaticBoxSizer( m_web_box, wxVERTICAL );

    m_web_box_sizer->Add( m_web_def_radio, 0, wxALL, 2 );
    m_web_box_sizer->Add( m_web_spec_radio, 0, wxALL, 2 );
    m_web_box_sizer->Add( m_web_loc_sizer, 0, wxEXPAND | wxALL, 2 );
/////
    wxStaticBox* m_editor_box = new wxStaticBox( this , -1, _("External text editor") );
    m_editor_loc_text = new wxStaticText( this, -1, _("Path") );

    m_editor_edit = new wxTextCtrl( this, -1, sett().GetEditorPath() );

    m_editor_browse_btn = new wxButton( this, ID_BUT_EDITOR, _("Browse") );
    m_editor_browse_btn->SetToolTip(TE(_("Use a file dialog to find the editor binary")));

    m_editor_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_editor_loc_sizer->Add( m_editor_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
    m_editor_loc_sizer->Add( m_editor_edit, 1, wxEXPAND );
    m_editor_loc_sizer->Add( m_editor_browse_btn );

    m_editor_box_sizer = new wxStaticBoxSizer( m_editor_box, wxVERTICAL );

    m_editor_box_sizer->Add( m_editor_loc_sizer, 0, wxEXPAND | wxALL, 2 );
////////
    wxStaticBoxSizer* m_autojoin_sizer= new wxStaticBoxSizer ( wxVERTICAL, this, _("Autoconnect") );
	m_autoconnect_label = new wxStaticText ( this,
											 -1,
											 IdentityString( _("If checked, %s will automatically log on to the last used server") )
											 );
    m_autojoin = new wxCheckBox( this, -1, _("Autoconnect on lobby start"), wxDefaultPosition, wxDefaultSize, 0 );
    m_autojoin->SetValue( sett().GetAutoConnect() );
    m_autojoin_sizer->Add( m_autoconnect_label, 1, wxEXPAND | wxALL, 5 );
    m_autojoin_sizer->Add( m_autojoin, 0, wxEXPAND | wxALL, 5 );

    wxStaticBoxSizer* m_reportstats_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Report statistics") );
	m_reportstats_label = new wxStaticText ( this,
											 -1,
											 IdentityString( _("By default %s will send some statistics (OS,lobby version) to server,\nto both make helping you in case of problems easier and inform of critical updates.\nUncheck to disable.") )
											 );
    m_reportstats = new wxCheckBox( this, -1, _("report statistics"), wxDefaultPosition, wxDefaultSize, 0 );
    m_reportstats->SetValue( sett().GetReportStats() );
    m_reportstats_sizer->Add( m_reportstats_label, 1, wxEXPAND|wxALL, 5);

    m_reportstats_sizer->Add( m_reportstats, 0, wxEXPAND|wxALL, 5);
    wxStaticBoxSizer* m_disable_version_check_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Disable version check") );
	m_disable_version_check_label = new wxStaticText ( this,
											 -1,
											 IdentityString( _("By default %s will check spring version.\nCheck to disable.") )
											 );
    m_disable_version_check = new wxCheckBox( this, -1, _("disable version check"), wxDefaultPosition, wxDefaultSize, 0 );
    m_disable_version_check->SetValue( sett().GetDisableSpringVersionCheck() );
    m_disable_version_check_sizer->Add( m_disable_version_check_label, 1, wxEXPAND|wxALL, 5);
    m_disable_version_check_sizer->Add( m_disable_version_check, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_web_box_sizer, 0, wxEXPAND | wxALL, 5 );
    m_main_sizer->Add( m_editor_box_sizer, 0, wxEXPAND | wxALL, 5 );
    m_main_sizer->Add( m_autojoin_sizer, 0, wxALL, 5 );
    m_main_sizer->Add( m_reportstats_sizer, 0, wxALL, 5 );
    m_main_sizer->Add( m_disable_version_check_sizer, 0, wxALL, 5 );
#ifdef __WXMSW__
    wxStaticBoxSizer* m_updater_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Automatic updates") );
	m_updater_label = new wxStaticText ( this,
										 -1,
										 IdentityString( _("%s can check at startup if a newer version is available and automatically download it for you.") )
										 );
    m_updater = new wxCheckBox( this, -1, _("automatically check for updates"), wxDefaultPosition, wxDefaultSize, 0 );
    m_updater->SetValue( sett().GetAutoUpdate() );
    m_updater_sizer->Add( m_updater_label, 1, wxEXPAND|wxALL, 5);
    m_updater_sizer->Add( m_updater, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_updater_sizer, 0, wxALL, 5 );
#endif

    wxStaticBoxSizer* m_show_tooltips_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Tooltips") );
    m_show_tooltips = new wxCheckBox( this, -1, _("Show Tooltips?"), wxDefaultPosition, wxDefaultSize, 0 );
    m_show_tooltips->SetValue( sett().GetShowTooltips() );
#ifndef __WXMSW__ // on windows this change is immediate
	m_show_tooltips_label = new wxStaticText ( this, -1, IdentityString( _("Requires %s restart to take effect.") ) );
    m_show_tooltips_sizer->Add( m_show_tooltips_label, 1, wxEXPAND|wxALL, 5);
#endif
    m_show_tooltips_sizer->Add( m_show_tooltips, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_show_tooltips_sizer, 0, wxALL, 5 );

    wxStaticBoxSizer* m_complete_method_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Tab completion method") );
    m_complete_method_label = new wxStaticText ( this, -1, _("\"Match exact\" will complete a word if there is one and only one match.\n"
        "\"Match nearest\" will select the (first) match that has closest Levenshtein distance") );
    m_complete_method_old = new wxRadioButton( this, -1, _("Match exact"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_complete_method_new = new wxRadioButton( this, -1, _("Match nearest"), wxDefaultPosition, wxDefaultSize );
    m_complete_method_old->SetValue( sett().GetCompletionMethod() == Settings::MatchExact );
    m_complete_method_new->SetValue( sett().GetCompletionMethod() == Settings::MatchNearest );
    m_complete_method_sizer->Add( m_complete_method_label, 1, wxEXPAND|wxALL, 5);
    m_complete_method_sizer->Add( m_complete_method_old, 0, wxEXPAND|wxALL, 5);
    m_complete_method_sizer->Add( m_complete_method_new, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_complete_method_sizer, 0, wxALL, 5 );

    wxStaticBoxSizer* m_misc_gui_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Misc GUI") );
    m_use_tabicons = new wxCheckBox( this, -1, _("Show big icons in mainwindow tabs?"), wxDefaultPosition, wxDefaultSize, 0 );
    m_use_tabicons->SetValue( sett().GetUseTabIcons() );
    m_misc_gui_sizer->Add( m_use_tabicons , 0, wxEXPAND | wxALL, 5 );
	m_use_notif_popups = new wxCheckBox( this, -1, _("Use notification popups?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_misc_gui_sizer->Add( m_use_notif_popups, 0, wxEXPAND | wxALL, 5 );
	m_notif_popup_pos_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_notif_popup_pos_label = new wxStaticText( this, -1, _("Screen position for notification popups") );
	m_notif_popup_pos = new wxChoice( this, -1,  wxDefaultPosition, wxDefaultSize, ScreenPosition() );
	m_notif_popup_pos_sizer->Add( m_notif_popup_pos_label, 0, wxALL| wxALIGN_CENTER_VERTICAL, 5 );
	m_notif_popup_pos_sizer->Add( m_notif_popup_pos, 0, wxALL| wxALIGN_CENTER_VERTICAL, 5 );
	m_misc_gui_sizer->Add( m_notif_popup_pos_sizer, 0, wxEXPAND | wxALL, 0 );
	m_notif_popup_time_sizer  = new wxBoxSizer( wxHORIZONTAL );
	m_notif_popup_time = new wxSpinCtrl( this, wxID_ANY );
	m_notif_popup_time_label = new wxStaticText( this, -1, _("Display time for popup notifications in seconds") );
	m_notif_popup_time_sizer->Add( m_notif_popup_time_label, 0, wxALL| wxALIGN_CENTER_VERTICAL, 5 );
	m_notif_popup_time_sizer->Add( m_notif_popup_time, 0, wxALL| wxALIGN_CENTER_VERTICAL, 5 );
	m_misc_gui_sizer->Add( m_notif_popup_time_sizer, 0, wxEXPAND | wxALL, 0 );


    m_x_on_all_tabs = new wxCheckBox( this, -1, _("Show close button on all tabs? (needs restart to take effect)"), wxDefaultPosition, wxDefaultSize, 0 );
    m_x_on_all_tabs->SetValue( sett().GetShowXallTabs() );
    m_misc_gui_sizer->Add( m_x_on_all_tabs , 1, wxEXPAND | wxALL, 5 );
    m_main_sizer->Add( m_misc_gui_sizer , 0, wxALL, 5 );

    wxStaticBoxSizer* m_start_tab_sizer = new wxStaticBoxSizer ( wxHORIZONTAL, this, _("Start tab") );
    m_start_tab = new wxChoice( this, -1,  wxDefaultPosition, wxDefaultSize, MainWindow::GetTabNames() );
    m_start_tab->SetSelection( sett().GetStartTab() );
    wxStaticText* m_start_tab_label = new wxStaticText ( this, -1, _("Select which tab to show at startup") );
    m_start_tab_sizer->Add( m_start_tab_label  , 0, wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL, 5 );
    m_start_tab_sizer->Add( m_start_tab , 0,  wxALIGN_CENTER_VERTICAL , 5 );
    m_main_sizer->Add( m_start_tab_sizer, 0, wxALL, 5 );

	//dummy event that updates controls to correct state
	wxCommandEvent evt;
	OnRestore( evt );

    SetScrollRate( SCROLL_RATE, SCROLL_RATE );
    SetSizer( m_main_sizer );
    Layout();
}

LobbyOptionsTab::~LobbyOptionsTab()
{
    //dtor
}

void LobbyOptionsTab::OnApply(wxCommandEvent& /*unused*/)
{
    if ( !m_web_def_radio->GetValue() ) sett().SetWebBrowserPath( m_web_edit->GetValue() );
    sett().SetWebBrowserUseDefault( m_web_def_radio->GetValue() );

    sett().SetAutoConnect( m_autojoin->IsChecked() );
    sett().SetReportStats( m_reportstats->GetValue() );
    sett().SetDisableSpringVersionCheck(m_disable_version_check->GetValue() );
#ifdef __WXMSW__
    sett().SetAutoUpdate( m_updater->IsChecked() );
#endif
    bool show = m_show_tooltips->IsChecked();
    wxToolTip::Enable(show);
    sett().SetShowTooltips(show);

    sett().SetCompletionMethod( m_complete_method_new->GetValue() ? Settings::MatchNearest: Settings::MatchExact );

    sett().SetUseTabIcons( m_use_tabicons->IsChecked() );
    ui().mw().SetTabIcons();
    sett().SetStartTab( m_start_tab->GetSelection() );

    sett().SetEditorPath( m_editor_edit->GetValue() );

    sett().SetShowXallTabs( m_x_on_all_tabs->IsChecked() );
	sett().SetUseNotificationPopups( m_use_notif_popups->IsChecked() );
	sett().SetNotificationPopupPosition( m_notif_popup_pos->GetSelection() );
	sett().SetNotificationPopupDisplayTime( m_notif_popup_time->GetValue() );

}


void LobbyOptionsTab::OnRestore(wxCommandEvent& /*unused*/)
{
    m_autojoin->SetValue( sett().GetAutoConnect() );
    m_reportstats->SetValue( sett().GetReportStats() );
    m_disable_version_check->SetValue( sett().GetDisableSpringVersionCheck() );
#ifdef __WXMSW__
    m_updater->SetValue( sett().GetAutoUpdate() );
#endif
    bool show = sett().GetShowTooltips();
    m_show_tooltips->SetValue(show);
    wxToolTip::Enable(show);

    m_complete_method_old->SetValue( sett().GetCompletionMethod() == Settings::MatchExact );
    m_complete_method_new->SetValue( sett().GetCompletionMethod() == Settings::MatchNearest );

    HandleWebloc( sett().GetWebBrowserUseDefault() );
    m_use_tabicons->SetValue( sett().GetUseTabIcons()  );

    m_start_tab->SetSelection( sett().GetStartTab() );

    m_editor_edit->SetValue( sett().GetEditorPath() );

    m_x_on_all_tabs->SetValue( sett().GetShowXallTabs() );
	m_use_notif_popups->SetValue( sett().GetUseNotificationPopups() );
	m_notif_popup_pos->SetSelection( sett().GetNotificationPopupPosition() );
	m_notif_popup_time->SetValue( sett().GetNotificationPopupDisplayTime() );

}

void LobbyOptionsTab::HandleWebloc( bool defloc )
{
  m_web_def_radio->SetValue( defloc );
  m_web_spec_radio->SetValue( !defloc );
  if ( defloc ) {
    m_web_edit->Enable( false );
    m_web_browse_btn->Enable( false );
    //m_sync_find_btn->Enable( false );
    m_web_edit->SetValue( _T("") );
  } else {
    m_web_edit->Enable( true );
    m_web_browse_btn->Enable( true );
    //m_sync_find_btn->Enable( true );
    m_web_edit->SetValue( sett().GetWebBrowserPath() );
  }
}

void LobbyOptionsTab::OnBrowseWeb( wxCommandEvent& /*unused*/ )
{
  wxFileDialog pick( this, _("Choose a web browser executable"), _T(""), _T("*"), CHOOSE_EXE );
  if ( pick.ShowModal() == wxID_OK ) m_web_edit->SetValue( pick.GetPath() );
}

void LobbyOptionsTab::OnBrowseEditor( wxCommandEvent& /*unused*/ )
{
  wxFileDialog pick( this, _("Choose a editor browser executable"), _T(""), _T("*"), CHOOSE_EXE );
  if ( pick.ShowModal() == wxID_OK ) m_editor_edit->SetValue( pick.GetPath() );
}

void LobbyOptionsTab::OnDefaultWeb( wxCommandEvent& /*unused*/ )
{
  HandleWebloc( m_web_def_radio->GetValue() );
}


