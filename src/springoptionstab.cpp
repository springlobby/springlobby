
/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: SpringOptionsTab
//

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
#include "springoptionstab.h"
#include "ui.h"
#include "iunitsync.h"
#include "utils.h"
#include "settings.h"
#include "mainwindow.h"

#include "settings++/custom_dialogs.h"

BEGIN_EVENT_TABLE(SpringOptionsTab, wxPanel)

    EVT_BUTTON ( SPRING_EXECBROWSE, SpringOptionsTab::OnBrowseExec )
    EVT_BUTTON ( SPRING_SYNCBROWSE, SpringOptionsTab::OnBrowseSync )
    EVT_BUTTON ( SPRING_AUTOCONF, SpringOptionsTab::OnAutoConf )
    EVT_BUTTON ( SPRING_EXECFIND, SpringOptionsTab::OnFindExec )
    EVT_BUTTON ( SPRING_SYNCFIND, SpringOptionsTab::OnFindSync )
    EVT_BUTTON ( SPRING_DATADIR, SpringOptionsTab::OnDataDir )
    EVT_CHECKBOX( SPRING_DONTSEARCH, SpringOptionsTab::OnDontSearch )
END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab( wxWindow* parent, Ui& ui ) : wxScrolledWindow( parent, -1 ),m_ui(ui)
{
	m_dontsearch_chkbox = new wxCheckBox( this, SPRING_DONTSEARCH, _("Search only in current installed path"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
	m_dontsearch_chkbox->SetValue( sett().GetSearchSpringOnlyInSLPath() );
	#ifndef __WXMSW__
	m_dontsearch_chkbox->Disable();
	#endif
  /* ================================
   * Spring executable
   */
  m_exec_box = new wxStaticBox( this, -1, _("Spring executable") );

  m_exec_loc_text = new wxStaticText( this, -1, _("Location") );
  m_exec_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedSpringBinary() );
  m_exec_browse_btn = new wxButton( this, SPRING_EXECBROWSE, _("Browse") );
  m_exec_find_btn = new wxButton( this, SPRING_EXECFIND, _("Find") );

  /* ================================
   * UnitSync
   */
  m_sync_box = new wxStaticBox( this, -1, _("UnitSync library") );

  m_sync_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedUnitSync() );
  m_sync_loc_text = new wxStaticText( this, -1, _("Location") );
  m_sync_browse_btn = new wxButton( this, SPRING_SYNCBROWSE, _("Browse") );
  m_sync_find_btn = new wxButton( this, SPRING_SYNCFIND, _("Find") );

  m_auto_btn = new wxButton( this, SPRING_AUTOCONF, _("Auto Configure") );
  m_datadir_btn = new wxButton( this, SPRING_DATADIR, _("Change Datadir path") );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_aconf_sizer = new wxBoxSizer( wxVERTICAL );
  m_exec_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_sync_loc_sizer = new wxBoxSizer( wxHORIZONTAL );

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
  m_aconf_sizer->Add( m_datadir_btn );

	m_main_sizer->Add( m_dontsearch_chkbox, 0, wxEXPAND | wxALL, 5 );
  m_main_sizer->Add( m_exec_box_sizer, 0, wxEXPAND | wxALL, 5 );
  m_main_sizer->Add( m_sync_box_sizer, 0, wxEXPAND | wxALL, 5 );

  m_main_sizer->Add( m_aconf_sizer, 0, wxEXPAND | wxALL, 5 );
  m_main_sizer->AddStretchSpacer();

  SetSizer( m_main_sizer );

  SetScrollRate( 3, 3 );

  Layout();

  DoRestore();

  if ( sett().IsPortableMode() || sett().GetSearchSpringOnlyInSLPath() )
  {
    m_exec_box->Disable();
    m_sync_box->Disable();
    m_auto_btn->Disable();
    m_datadir_btn->Disable();
  }

	if ( sett().IsFirstRun() )
	{
		sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), m_exec_edit->GetValue() );
		sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), m_sync_edit->GetValue() );
		sett().SetSearchSpringOnlyInSLPath( m_dontsearch_chkbox->IsChecked() );
	}

}


SpringOptionsTab::~SpringOptionsTab()
{

}


void SpringOptionsTab::DoRestore()
{
	#ifdef __WXMSW__
	m_dontsearch_chkbox->SetValue( sett().GetSearchSpringOnlyInSLPath() );
	#endif
  m_sync_edit->SetValue( sett().GetCurrentUsedUnitSync() );
  m_exec_edit->SetValue( sett().GetCurrentUsedSpringBinary() );
}


void SpringOptionsTab::OnAutoConf( wxCommandEvent& event )
{
  OnFindExec( event );
  OnFindSync( event );
}


void SpringOptionsTab::OnFindExec( wxCommandEvent& event )
{
  wxString found = sett().AutoFindSpringBin();
  if ( !found.IsEmpty() ) m_exec_edit->SetValue( found );
}


void SpringOptionsTab::OnFindSync( wxCommandEvent& event )
{
  wxString found = sett().AutoFindUnitSync();
  if( !found.IsEmpty() ) m_sync_edit->SetValue( found );
}

void SpringOptionsTab::OnBrowseExec( wxCommandEvent& event )
{
  wxFileDialog pick( this, _("Choose a Spring executable"),
		    wxPathOnly( sett().GetCurrentUsedSpringBinary() ),
		    wxString(SPRING_BIN), CHOOSE_EXE );
  if ( pick.ShowModal() == wxID_OK ) m_exec_edit->SetValue( pick.GetPath() );
}

void SpringOptionsTab::OnBrowseSync( wxCommandEvent& event )
{
	wxString filefilter = wxString(_("Library")) << _T("(*") << GetLibExtension() << _T(")|*") + GetLibExtension();
	#ifdef __WXMAC__
	filefilter << _T("|") << _("Library") << _T("(*.dylib)|*.dylib");
	#endif
	filefilter << _T("|")  << wxString(_("Any File")) << _T(" (*.*)|*.*");
  wxFileDialog pick( this, _("Choose UnitSync library"),
		    wxPathOnly( sett().GetCurrentUsedSpringBinary() ),
		    _T("unitsync") + GetLibExtension(),
		    wxString(_("Library")) + _T("(*") + GetLibExtension() + _T(")|*") + GetLibExtension() + _T("|") + wxString(_("Any File")) + _T(" (*.*)|*.*")  );
  if ( pick.ShowModal() == wxID_OK ) m_sync_edit->SetValue( pick.GetPath() );
}


void SpringOptionsTab::OnApply( wxCommandEvent& event )
{
  sett().SetSpringBinary( sett().GetCurrentUsedSpringIndex(), m_exec_edit->GetValue() );
  sett().SetUnitSync( sett().GetCurrentUsedSpringIndex(), m_sync_edit->GetValue() );
  #ifdef __WXMSW__
  sett().SetSearchSpringOnlyInSLPath( m_dontsearch_chkbox->IsChecked() );
  #endif

  if ( sett().IsFirstRun() ) return;

	sett().RefreshSpringVersionList();
  if ( !usync().LoadUnitSyncLib( sett().GetCurrentUsedUnitSync() ) )
  {
    wxLogWarning( _T("Cannot load UnitSync") );
    customMessageBox( SL_MAIN_ICON,
    _("SpringLobby is unable to load your UnitSync library.\n\nYou might want to take another look at your unitsync setting."),
    _("Spring error"), wxOK );
  }

  m_ui.mw().OnUnitSyncReloaded();
}


void SpringOptionsTab::OnRestore( wxCommandEvent& event )
{
  DoRestore();
}

void SpringOptionsTab::OnDataDir( wxCommandEvent& event )
{
  SetupUserFolders();
}

void SpringOptionsTab::OnDontSearch( wxCommandEvent& event )
{
  if ( m_dontsearch_chkbox->IsChecked() )
  {
    m_exec_box->Disable();
    m_sync_box->Disable();
    m_auto_btn->Disable();
    m_datadir_btn->Disable();
  }
  else
  {
    m_exec_box->Enable();
    m_sync_box->Enable();
    m_auto_btn->Enable();
    m_datadir_btn->Enable();
  }
}

/** Try to create the named directory, if it doesn't exist.
 *
 * @param name Path to directory that should exist or be created.
 *
 * @param perm Value of @p perm parameter for wxFileName::Mkdir.
 *
 * @param flags Value of @p flags parameter for wxFileName::Mkdir.
 *
 * @return @c true if the directory already exists, or the return
 * value of wxFileName::Mkdir if it does not.
 */
inline bool
tryCreateDirectory(const wxString& name, int perm = 0775, int flags = 0)
{
    if ( wxFileName::DirExists(name) )
	return true;
    else
	return wxFileName::Mkdir(name, perm, flags);
}

void SpringOptionsTab::SetupUserFolders()
{
      wxString sep = wxFileName::GetPathSeparator();
      wxString defaultdir = wxFileName::GetHomeDir() + sep +_T(".spring");

      int result = wxMessageBox( _("Do you want to change spring's datadir location? (select yes only if you know what you're doing)"), _("Data dir wizard"), wxICON_QUESTION | wxYES_NO, &ui().mw() );

      if ( result != wxYES ) return;

      wxString dir = wxDirSelector( _("Choose a folder"), defaultdir );

			if ( dir.IsEmpty() ||
	       ( !tryCreateDirectory( dir, 0775 ) ||
				 ( !tryCreateDirectory( dir + sep + _T("mods"), 0775 ) ||
		       !tryCreateDirectory( dir + sep + _T("maps"), 0775 ) ||
		       !tryCreateDirectory( dir + sep + _T("base"), 0775 ) ||
		       !tryCreateDirectory( dir + sep + _T("demos"), 0775 ) ||
					 !tryCreateDirectory( dir + sep + _T("screenshots"), 0775  ) )
				 )
	       )
			{
				if ( dir.IsEmpty() ) dir = defaultdir;
				wxMessageBox( _("Something went wrong when creating the directories\nPlease create manually the following folders:") + wxString(_T("\n")) + dir +  _T("\n") + dir + sep + _T("mods\n") + dir + sep + _T("maps\n") + dir + sep + _T("base\n") );
			return;
			}
      if ( usync().IsLoaded() )
      {
				usync().SetSpringDataPath(dir);
      }

			// copy uikeys.txt
			wxPathList pl;
			pl.AddEnvList( _T("%ProgramFiles%") );
			pl.AddEnvList( _T("XDG_DATA_DIRS") );
			pl = sett().GetAdditionalSearchPaths( pl );
			wxString uikeyslocation = pl.FindValidPath( _T("uikeys.txt") );
			if ( !uikeyslocation.IsEmpty() )
			{
				wxCopyFile( uikeyslocation, sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("uikeys.txt"), false );
			}
}
