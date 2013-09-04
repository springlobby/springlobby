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


BEGIN_EVENT_TABLE( SpringOptionsTab, wxPanel )

	EVT_BUTTON (    SPRING_EXECBROWSE,  SpringOptionsTab::OnBrowseExec  )
	EVT_BUTTON (    SPRING_SYNCBROWSE,  SpringOptionsTab::OnBrowseSync  )
	EVT_BUTTON (    SPRING_BUNDLEBROWSE,SpringOptionsTab::OnBrowseBundle)
	EVT_BUTTON (    SPRING_AUTOCONF,    SpringOptionsTab::OnAutoConf    )
	EVT_BUTTON (    SPRING_EXECFIND,    SpringOptionsTab::OnFindExec    )
	EVT_BUTTON (    SPRING_SYNCFIND,    SpringOptionsTab::OnFindSync    )
	EVT_BUTTON (    SPRING_BUNDLEFIND,  SpringOptionsTab::OnFindBundle  )
	EVT_BUTTON (    SPRING_DATADIR,     SpringOptionsTab::OnDataDir     )
	EVT_CHECKBOX(   SPRING_DONTSEARCH,  SpringOptionsTab::OnDontSearch  )
	EVT_CHECKBOX(   SPRING_FORCEBUNDLE, SpringOptionsTab::OnForceBundle )

END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab( wxWindow* parent )
    : wxScrolledWindow( parent, -1 )
{
	m_dontsearch_chkbox = new wxCheckBox( this, SPRING_DONTSEARCH, _("Search only in current installed path"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
	m_dontsearch_chkbox->SetValue( sett().GetSearchSpringOnlyInSLPath() );
	m_oldlaunch_chkbox = new wxCheckBox( this, SPRING_DONTSEARCH, _("Use alternative launch method (DO NOT TOUCH THIS UNLESS YOU KNOW WHAT YOU'RE DOING)"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
	m_oldlaunch_chkbox->SetValue( sett().UseOldSpringLaunchMethod() );
	m_forcebundle_chkbox = new wxCheckBox( this, SPRING_FORCEBUNDLE, _("Use the Spring version in the current bundle"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
	m_forcebundle_chkbox->SetValue( sett().GetUseSpringPathFromBundle() );
	if (!sett().IsInsideSpringBundle()) m_forcebundle_chkbox->Disable();
#ifndef __WXMSW__
	m_dontsearch_chkbox->Disable();
	m_oldlaunch_chkbox->Disable();
#endif
	if (!sett().GetBundleMode()) m_forcebundle_chkbox->Disable();
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

	/* ================================
	 * Spring Bundle
	 */
	m_bundle_box = new wxStaticBox( this, -1, _( "Spring App Bundle" ) );

	m_bundle_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedBundle() );
	m_bundle_loc_text = new wxStaticText( this, -1, _( "Location" ) );
	m_bundle_browse_btn = new wxButton( this, SPRING_BUNDLEBROWSE, _( "Browse" ) );
	m_bundle_find_btn = new wxButton( this, SPRING_BUNDLEFIND, _( "Find" ) );

	if (sett().GetBundleMode())
	{
		EnableSpringBox(false);
		EnableUnitsyncBox(false);
	}
	else
	{
		EnableBundleBox(false);
	}

	m_auto_btn = new wxButton( this, SPRING_AUTOCONF, _( "Auto Configure" ) );
	m_datadir_btn = new wxButton( this, SPRING_DATADIR, _( "Change Datadir path" ) );

	m_main_sizer = new wxBoxSizer( wxVERTICAL );
	m_aconf_sizer = new wxBoxSizer( wxVERTICAL );
	m_exec_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_sync_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_bundle_loc_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_exec_loc_sizer->Add( m_exec_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
	m_exec_loc_sizer->Add( m_exec_edit, 1, wxEXPAND );
	m_exec_loc_sizer->Add( m_exec_browse_btn );
	m_exec_loc_sizer->Add( m_exec_find_btn );

	m_sync_loc_sizer->Add( m_sync_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
	m_sync_loc_sizer->Add( m_sync_edit, 1, wxEXPAND );
	m_sync_loc_sizer->Add( m_sync_browse_btn );
	m_sync_loc_sizer->Add( m_sync_find_btn );

	m_bundle_loc_sizer->Add( m_bundle_loc_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 2 );
	m_bundle_loc_sizer->Add( m_bundle_edit, 1, wxEXPAND );
	m_bundle_loc_sizer->Add( m_bundle_browse_btn );
	m_bundle_loc_sizer->Add( m_bundle_find_btn );

	m_exec_box_sizer = new wxStaticBoxSizer( m_exec_box, wxVERTICAL );
	m_sync_box_sizer = new wxStaticBoxSizer( m_sync_box, wxVERTICAL );
	m_bundle_box_sizer = new wxStaticBoxSizer( m_bundle_box, wxVERTICAL );

	m_exec_box_sizer->Add( m_exec_loc_sizer, 0, wxEXPAND | wxALL, 2 );

	m_sync_box_sizer->Add( m_sync_loc_sizer, 0, wxEXPAND | wxALL, 2 );

	m_bundle_box_sizer->Add( m_bundle_loc_sizer, 0, wxEXPAND | wxALL, 2 );

	m_aconf_sizer->AddStretchSpacer();
	m_aconf_sizer->Add( m_auto_btn );
	m_aconf_sizer->Add( m_datadir_btn );

	m_main_sizer->Add( m_dontsearch_chkbox, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_forcebundle_chkbox, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_exec_box_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_sync_box_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_bundle_box_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_oldlaunch_chkbox, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->Add( m_aconf_sizer, 0, wxEXPAND | wxALL, 5 );
	m_main_sizer->AddStretchSpacer();

	SetSizer( m_main_sizer );

	SetScrollRate( SCROLL_RATE, SCROLL_RATE );

	Layout();

	DoRestore();

	if ( sett().IsPortableMode() || sett().GetSearchSpringOnlyInSLPath() )
	{
		EnableSpringBox(false);
		EnableUnitsyncBox(false);
		m_auto_btn->Disable();
		m_bundle_box->Disable();
		m_datadir_btn->Disable();
	}

	if ( sett().GetUseSpringPathFromBundle() )
	{
		m_bundle_box->Disable();
	}

	if ( sett().IsFirstRun() )
	{
		sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), m_exec_edit->GetValue() );
		sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), m_sync_edit->GetValue() );
		sett().SetBundle( sett().GetCurrentUsedSpringIndex(), m_bundle_edit->GetValue() );
		sett().SetSearchSpringOnlyInSLPath( m_dontsearch_chkbox->IsChecked() );
	}

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

void SpringOptionsTab::EnableBundleBox(bool enabled)
{
		m_bundle_box->Enable(enabled);
		m_bundle_edit->Enable(enabled);
		m_bundle_loc_text->Enable(enabled);
		m_bundle_browse_btn->Enable(enabled);
		m_bundle_find_btn->Enable(enabled);
}

void SpringOptionsTab::DoRestore()
{
	m_dontsearch_chkbox->SetValue( sett().GetSearchSpringOnlyInSLPath() );
    m_oldlaunch_chkbox->SetValue( sett().UseOldSpringLaunchMethod() );
	m_sync_edit->SetValue( sett().GetCurrentUsedUnitSync() );
	m_exec_edit->SetValue( sett().GetCurrentUsedSpringBinary() );
	m_bundle_edit->SetValue( sett().GetCurrentUsedBundle() );
}


void SpringOptionsTab::OnAutoConf( wxCommandEvent& event )
{
	OnFindExec( event );
	OnFindSync( event );
	OnFindBundle( event );
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

void SpringOptionsTab::OnFindBundle( wxCommandEvent& /*unused*/ )
{
	wxString found = sett().AutoFindBundle();
	if ( !found.IsEmpty() ) m_bundle_edit->SetValue( found );
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


void SpringOptionsTab::OnBrowseBundle( wxCommandEvent& /*unused*/ )
{
	wxFileDialog pick( this, _( "Choose a Spring executable" ),
	                   wxPathOnly( sett().GetCurrentUsedBundle() ),
	                   _T("Spring.App"), wxString(_("App bundle")) + _T("(*.App)||.App") );
	if ( pick.ShowModal() == wxID_OK ) m_bundle_edit->SetValue( pick.GetPath() );
}

void SpringOptionsTab::OnApply( wxCommandEvent& /*unused*/ )
{
    const bool reload_usync =
            sett().GetUnitSync( sett().GetCurrentUsedSpringIndex() ) != m_sync_edit->GetValue();
	sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), m_exec_edit->GetValue() );
	sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), m_sync_edit->GetValue() );
	sett().SetBundle( sett().GetCurrentUsedSpringIndex(), m_bundle_edit->GetValue() );
	sett().SetSearchSpringOnlyInSLPath( m_dontsearch_chkbox->IsChecked() );
	sett().SetOldSpringLaunchMethod( m_oldlaunch_chkbox->IsChecked() );
	sett().SetUseSpringPathFromBundle( m_forcebundle_chkbox->IsChecked() );

	if ( sett().IsFirstRun() ) return;

	UiEvents::ScopedStatusMessage( _("Reloading unitsync"), 0 );
	sett().RefreshSpringVersionList();
    if ( reload_usync && !LSL::usync().LoadUnitSyncLib(STD_STRING(sett().GetCurrentUsedUnitSync())))
	{
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
	if ( m_dontsearch_chkbox->IsChecked() ) {
		EnableSpringBox(false);
		EnableUnitsyncBox(false);
		m_datadir_btn->Disable();
	}
	else {
		EnableSpringBox(true);
		EnableUnitsyncBox(true);
		m_datadir_btn->Enable();
	}
}

void SpringOptionsTab::OnForceBundle( wxCommandEvent& /*unused*/ )
{
	EnableBundleBox(!m_forcebundle_chkbox->IsChecked());
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
	          ( !tryCreateDirectory( dir + sep + _T( "mods" ), 0775 ) ||
				!tryCreateDirectory( dir + sep + _T( "games" ), 0775 ) ||
	            !tryCreateDirectory( dir + sep + _T( "maps" ), 0775 ) ||
	            !tryCreateDirectory( dir + sep + _T( "base" ), 0775 ) ||
	            !tryCreateDirectory( dir + sep + _T( "demos" ), 0775 ) ||
	            !tryCreateDirectory( dir + sep + _T( "screenshots" ), 0775  ) )
	        )
	   )
	{
		if ( dir.IsEmpty() ) dir = defaultdir;
		wxMessageBox( _( "Something went wrong when creating the directories\nPlease create manually the following folders:" ) + wxString( _T( "\n" ) ) + dir +  _T( "\n" ) + dir + sep + _T( "mods\n" ) + dir + sep + _T( "maps\n" ) + dir + sep + _T( "base\n" ) );
		return;
	}
	if ( LSL::usync().IsLoaded() )
	{
        LSL::usync().SetSpringDataPath(STD_STRING(dir));
	}
    CopyUikeys( sett().GetCurrentUsedDataDir() );
}

