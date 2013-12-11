/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: SpringOptionsTab
//

#include "springoptionstab.h"

#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/choicdlg.h>
#include <wx/stdpaths.h>
#include <wx/scrolbar.h>        // added for scroll bar powers
#include <wx/log.h>
#include <wx/listbox.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

#include "nonportable.h"
#include "ui.h"
#include "utils/controls.h"
#include "utils/platform.h"
#include "utils/uievents.h"
#include "utils/misc.h"
#include "uiutils.h"
#include "settings.h"
#include "mainwindow.h"
#include "utils/customdialogs.h"
#include <lslunitsync/c_api.h>


BEGIN_EVENT_TABLE( SpringOptionsTab, wxPanel )

	EVT_BUTTON(     SPRING_EXECBROWSE,  SpringOptionsTab::OnBrowseExec  )
	EVT_BUTTON(     SPRING_SYNCBROWSE,  SpringOptionsTab::OnBrowseSync  )
	EVT_BUTTON(     SPRING_AUTOCONF,    SpringOptionsTab::OnAutoConf    )
	EVT_BUTTON(     SPRING_EXECFIND,    SpringOptionsTab::OnFindExec    )
	EVT_BUTTON(     SPRING_SYNCFIND,    SpringOptionsTab::OnFindSync    )
	EVT_BUTTON(     SPRING_DATADIR,     SpringOptionsTab::OnDataDir     )
	EVT_BUTTON(     SPRING_ADD,         SpringOptionsTab::OnAddBundle   )
	EVT_BUTTON(     SPRING_REMOVE,      SpringOptionsTab::OnRemoveBundle)
	EVT_CHECKBOX(   SPRING_DONTSEARCH,  SpringOptionsTab::OnDontSearch  )
	EVT_LISTBOX(    SPRING_LIST,        SpringOptionsTab::OnGroupListSelectionChange )

END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab( wxWindow* parent ):
	wxScrolledWindow( parent, -1 )
{
	wxBoxSizer* windowSizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* mainSizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* groupListSizer = new wxBoxSizer( wxVERTICAL );

	m_spring_list = new wxListBox( this, SPRING_LIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_ALWAYS_SB|wxLB_SINGLE|wxLB_SORT );
	groupListSizer->Add( m_spring_list, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	wxBoxSizer* groupListButtonsSizer = new wxBoxSizer( wxHORIZONTAL );

	m_remove_spring_button = new wxButton( this, SPRING_REMOVE, _("Remove"), wxDefaultPosition, wxSize( -1,-1 ), wxBU_EXACTFIT );
	m_remove_spring_button->SetToolTip( _("Remove an existing spring version") );

	groupListButtonsSizer->Add( m_remove_spring_button, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	m_rename_spring_button = new wxButton( this, SPRING_RENAME, _("Rename.."), wxDefaultPosition, wxSize( -1,-1 ), wxBU_EXACTFIT );
	m_rename_spring_button->Enable( false );
	m_rename_spring_button->SetToolTip( _("Rename an existing spring version") );

	groupListButtonsSizer->Add( m_rename_spring_button, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	groupListButtonsSizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_add_spring_button = new wxButton( this, SPRING_ADD, _("Add New.."), wxDefaultPosition, wxSize( -1,-1 ), wxBU_EXACTFIT );
	m_add_spring_button->SetToolTip( _("Add new spring version") );

	groupListButtonsSizer->Add( m_add_spring_button, 0, wxTOP|wxRIGHT|wxLEFT, 5 );

	groupListSizer->Add( groupListButtonsSizer, 0, wxEXPAND|wxBOTTOM, 5 );

	mainSizer->Add( groupListSizer, 2, wxEXPAND, 5 );




	m_oldlaunch_chkbox = new wxCheckBox( this, SPRING_DONTSEARCH, _("Use alternative launch method (DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU'RE DOING)"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
	m_oldlaunch_chkbox->SetValue( sett().UseOldSpringLaunchMethod() );
	/* ================================
	 * Spring executable
	 */
	m_exec_box = new wxStaticBox( this, -1, _( "Spring executable" ) );

	m_exec_loc_text = new wxStaticText( this, -1, _( "Location" ) );
	m_exec_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedSpringBinary() );
	m_exec_browse_btn = new wxButton( this, SPRING_EXECBROWSE, _( "Browse" ) );
	m_exec_find_btn = new wxButton( this, SPRING_EXECFIND, _( "Find" ) );

	/* ================================
	 * UnitSync
	 */
	m_sync_box = new wxStaticBox( this, -1, _( "UnitSync library" ) );

	m_sync_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedUnitSync() );
	m_sync_loc_text = new wxStaticText( this, -1, _( "Location" ) );
	m_sync_browse_btn = new wxButton( this, SPRING_SYNCBROWSE, _( "Browse" ) );
	m_sync_find_btn = new wxButton( this, SPRING_SYNCFIND, _( "Find" ) );

	m_auto_btn = new wxButton( this, SPRING_AUTOCONF, _( "Auto Configure" ) );
//	m_datadir_btn = new wxButton( this, SPRING_DATADIR, _( "Change Datadir path" ) );

	wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* m_aconf_sizer = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* m_exec_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* m_sync_loc_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_exec_loc_sizer->Add( m_exec_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
	m_exec_loc_sizer->Add( m_exec_edit, 1, wxEXPAND );
	m_exec_loc_sizer->Add( m_exec_browse_btn );
	m_exec_loc_sizer->Add( m_exec_find_btn );

	m_sync_loc_sizer->Add( m_sync_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
	m_sync_loc_sizer->Add( m_sync_edit, 1, wxEXPAND );
	m_sync_loc_sizer->Add( m_sync_browse_btn );
	m_sync_loc_sizer->Add( m_sync_find_btn );

	m_exec_box_sizer = new wxStaticBoxSizer( m_exec_box, wxVERTICAL );
	m_sync_box_sizer = new wxStaticBoxSizer( m_sync_box, wxVERTICAL );

	m_exec_box_sizer->Add( m_exec_loc_sizer, 0, wxEXPAND | wxALL, 2 );

	m_sync_box_sizer->Add( m_sync_loc_sizer, 0, wxEXPAND | wxALL, 2 );

	m_aconf_sizer->AddStretchSpacer();
	m_aconf_sizer->Add( m_auto_btn );
//	m_aconf_sizer->Add( m_datadir_btn );

	m_main_sizer->Add( m_exec_box_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_sync_box_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_oldlaunch_chkbox, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_aconf_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->AddStretchSpacer();

	windowSizer->Add(mainSizer);
	windowSizer->Add(m_main_sizer);
	SetSizer( windowSizer );

	SetScrollRate( SCROLL_RATE, SCROLL_RATE );

	ReloadSpringList();
	Layout();
	DoRestore();
}


SpringOptionsTab::~SpringOptionsTab()
{

}

void SpringOptionsTab::EnableSpringBox(bool enabled)
{
	m_exec_box->Enable(enabled);
	m_exec_loc_text->Enable(enabled);
	m_exec_edit->Enable(enabled);
	m_exec_browse_btn->Enable(enabled);
	m_exec_find_btn->Enable(enabled);
}

void SpringOptionsTab::EnableUnitsyncBox(bool enabled)
{
	m_sync_box->Enable(enabled);
	m_sync_loc_text->Enable(enabled);
	m_sync_edit->Enable(enabled);
	m_sync_browse_btn->Enable(enabled);
	m_sync_find_btn->Enable(enabled);
}

void SpringOptionsTab::DoRestore()
{
	const wxString index = m_spring_list->GetStringSelection();
	m_oldlaunch_chkbox->SetValue( sett().UseOldSpringLaunchMethod() );
	m_sync_edit->SetValue(sett().GetUnitSync(index));
	m_exec_edit->SetValue(sett().GetSpringBinary(index));
}


void SpringOptionsTab::OnAutoConf( wxCommandEvent& event )
{
	OnFindExec( event );
	OnFindSync( event );
}


void SpringOptionsTab::OnFindExec( wxCommandEvent& /*unused*/ )
{
	wxString found = sett().AutoFindSpringBin();
	if ( !found.IsEmpty() ) m_exec_edit->SetValue( found );
}


void SpringOptionsTab::OnFindSync( wxCommandEvent& /*unused*/ )
{
	wxString found = sett().AutoFindUnitSync();
	if ( !found.IsEmpty() ) m_sync_edit->SetValue( found );
}

void SpringOptionsTab::OnBrowseExec( wxCommandEvent& /*unused*/ )
{
	wxFileDialog pick( this, _( "Choose a Spring executable" ),
			   wxPathOnly( sett().GetCurrentUsedSpringBinary() ),
			   wxString( SPRING_BIN ), CHOOSE_EXE );
	if ( pick.ShowModal() == wxID_OK ) m_exec_edit->SetValue( pick.GetPath() );
}

void SpringOptionsTab::OnBrowseSync( wxCommandEvent& /*unused*/ )
{
	wxString filefilter = wxString( _( "Library" ) ) << _T( "(*" ) << GetLibExtension() << _T( ")|*" ) + GetLibExtension();
#ifdef __WXMAC__
	filefilter << _T( "|" ) << _( "Library" ) << _T( "(*.dylib)|*.dylib" );
#endif
	filefilter << _T( "|" )  << wxString( _( "Any File" ) ) << _T( " (*.*)|*.*" );
	wxFileDialog pick( this, _( "Choose UnitSync library" ),
			   wxPathOnly( sett().GetCurrentUsedSpringBinary() ),
			   _T( "unitsync" ) + GetLibExtension(),
			   wxString( _( "Library" ) ) + _T( "(*" ) + GetLibExtension() + _T( ")|*" ) + GetLibExtension() + _T( "|" ) + wxString( _( "Any File" ) ) + _T( " (*.*)|*.*" )  );
	if ( pick.ShowModal() == wxID_OK ) m_sync_edit->SetValue( pick.GetPath() );
}


void SpringOptionsTab::OnApply( wxCommandEvent& /*unused*/ )
{
	const wxString index = m_spring_list->GetStringSelection();
	sett().SetSpringBinary(index, m_exec_edit->GetValue() );
	sett().SetUnitSync(index, m_sync_edit->GetValue() );
	sett().SetOldSpringLaunchMethod( m_oldlaunch_chkbox->IsChecked() );

	UiEvents::ScopedStatusMessage( _("Reloading unitsync"), 0 );
	sett().RefreshSpringVersionList();

	const bool reload_usync = sett().GetUnitSync( index ) != m_sync_edit->GetValue();
	if ( reload_usync && !LSL::usync().LoadUnitSyncLib(STD_STRING(sett().GetUnitSync(index)))) {
		wxLogWarning( _T( "Cannot load UnitSync" ) );
		customMessageBox( SL_MAIN_ICON,
				  IdentityString( _( "%s is unable to load your UnitSync library.\n\nYou might want to take another look at your unitsync setting." ) ),
				  _( "Spring error" ), wxOK );
	}
}


void SpringOptionsTab::OnRestore( wxCommandEvent& /*unused*/ )
{
	DoRestore();
}

void SpringOptionsTab::OnDataDir( wxCommandEvent& /*unused*/ )
{
	SetupUserFolders();
}

void SpringOptionsTab::OnDontSearch( wxCommandEvent& /*unused*/ )
{
	EnableSpringBox(true);
	EnableUnitsyncBox(true);
}

void SpringOptionsTab::SetupUserFolders()
{
	wxString sep = wxFileName::GetPathSeparator();
	wxString defaultdir = wxFileName::GetHomeDir() + sep + _T( ".spring" );

	int result = wxMessageBox( _( "Do you want to change spring's datadir location? (select yes only if you know what you're doing)" ), _( "Data dir wizard" ), wxICON_QUESTION | wxYES_NO, &ui().mw() );

	if ( result != wxYES ) return;

	wxString dir = wxDirSelector( _( "Choose a folder" ), defaultdir );

	if ( dir.IsEmpty() ||
	     ( !tryCreateDirectory( dir, 0775 ) ||
	       (!tryCreateDirectory( dir + sep + _T( "games" ), 0775 ) ||
		!tryCreateDirectory( dir + sep + _T( "maps" ), 0775 ) ||
		!tryCreateDirectory( dir + sep + _T( "demos" ), 0775 ) ||
		!tryCreateDirectory( dir + sep + _T( "screenshots" ), 0775  ) )
	     )
	   ) {
		if ( dir.IsEmpty() ) dir = defaultdir;
		wxMessageBox( _( "Something went wrong when creating the directories\nPlease create manually the following folders:" ) + wxString( _T( "\n" ) ) + dir +  _T( "\n" ) + dir + sep + _T( "maps\n" ) + dir + sep + _T( "base\n" ) );
		return;
	}
	if ( LSL::usync().IsLoaded() ) {
		LSL::usync().SetSpringDataPath(STD_STRING(dir));
	}
	CopyUikeys( sett().GetCurrentUsedDataDir() );
}


void SpringOptionsTab::OnGroupListSelectionChange( wxCommandEvent& event )
{
	const wxString selection = m_spring_list->GetStringSelection();
	if (!selection.empty()) {
		sett().SetUsedSpringIndex(selection);
		DoRestore();
	}
}

void SpringOptionsTab::ReloadSpringList()
{
	std::map<wxString, LSL::SpringBundle> springlist = sett().GetSpringVersionList();
	m_spring_list->Clear();
	for(auto bundle: springlist) {
		m_spring_list->Append(bundle.first);
		m_spring_list->SetStringSelection(sett().GetCurrentUsedSpringIndex());
	}
}

void SpringOptionsTab::OnAddBundle(wxCommandEvent& event)
{
#ifdef __WXMAC__
	wxString filefilter << _T( "|" ) << _( "Library" ) << _T( "(*.dylib)|*.dylib" );
#else
	wxString filefilter = wxString( _( "Library" ) ) + _T( "(*" ) + GetLibExtension() + _T( ")|*" ) + GetLibExtension() + _T( "|" ) + wxString( _( "Any File" ) ) + _T( " (*.*)|*.*" );
#endif
	filefilter << _T( "|" )  << wxString( _( "Any File" ) ) << _T( " (*.*)|*.*" );
	wxFileDialog pick( this, _( "Choose UnitSync library" ),
			   wxPathOnly( sett().GetCurrentUsedUnitSync() ),
			   _T( "unitsync" ) + GetLibExtension(),
			   filefilter  );
	if ( pick.ShowModal() == wxID_OK ) {
		//get unitsync version & add to list
		LSL::SpringBundle bundle;
		bundle.unitsync = STD_STRING(pick.GetPath());
		bundle.AutoComplete();
		const wxString version = TowxString(bundle.version);
		m_spring_list->Append(version);
		m_spring_list->SetStringSelection(version);
		m_sync_edit->SetValue(TowxString(bundle.unitsync));
		m_exec_edit->SetValue(TowxString(bundle.spring));
	}
}

void SpringOptionsTab::OnRemoveBundle(wxCommandEvent& event)
{
	const wxString index = m_spring_list->GetStringSelection();
	sett().DeleteSpringVersionbyIndex(index);
	sett().RefreshSpringVersionList(false);
	ReloadSpringList();
}
