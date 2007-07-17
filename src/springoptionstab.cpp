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

#include "springoptionstab.h"
#include "ui.h"
#include "unitsync.h"
#include "utils.h"
#include "settings.h"

BEGIN_EVENT_TABLE(SpringOptionsTab, wxPanel)

  EVT_BUTTON ( SPRING_DIRBROWSE, SpringOptionsTab::OnBrowseDir )
  EVT_BUTTON ( SPRING_EXECBROWSE, SpringOptionsTab::OnBrowseExec )
  EVT_BUTTON ( SPRING_SYNCBROWSE, SpringOptionsTab::OnBrowseSync )

END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab( wxWindow* parent, Ui& ui ) : wxPanel( parent, -1 ),m_ui(ui)
{

  m_dir_text = new wxStaticText( this, -1, _("Spring directory") );
  m_exec_loc_text = new wxStaticText( this, -1, _("Location") );
  m_sync_loc_text = new wxStaticText( this, -1, _("Location") );

  m_dir_browse_btn = new wxButton( this, SPRING_DIRBROWSE, _("Browse") );
  m_dir_find_btn = new wxButton( this, -1, _("Find") );
  m_exec_browse_btn = new wxButton( this, SPRING_EXECBROWSE, _("Browse") );
  m_exec_find_btn = new wxButton( this, -1, _("Find") );
  m_sync_browse_btn = new wxButton( this, SPRING_SYNCBROWSE, _("Browse") );
  m_sync_find_btn = new wxButton( this, -1, _("Find") );
  m_auto_btn = new wxButton( this, -1, _("Auto Configure") );

  m_exec_def_radio = new wxRadioButton( this, -1, _("Default location."), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  m_exec_spec_radio = new wxRadioButton( this, -1, _("Specify:") );
  m_sync_def_radio = new wxRadioButton( this, -1, _("Default location."), wxDefaultPosition, wxDefaultSize, wxRB_GROUP  );
  m_sync_spec_radio = new wxRadioButton( this, -1, _("Specify:") );

  if ( sett().GetSpringUseDefLoc() ) m_exec_def_radio->SetValue( true );
  else m_exec_spec_radio->SetValue( true );
  if ( sett().GetUnitsyncUseDefLoc() ) m_sync_def_radio->SetValue( true );
  else m_sync_spec_radio->SetValue( true );

  m_dir_edit = new wxTextCtrl( this, -1, WX_STRING(sett().GetSpringDir()) );
  m_exec_edit = new wxTextCtrl( this, -1, WX_STRING(sett().GetSpringLoc()) );
  m_sync_edit = new wxTextCtrl( this, -1, WX_STRING(sett().GetUnitsyncLoc()) );

  m_exec_box = new wxStaticBox( this, -1, _("Spring executable") );
  m_sync_box = new wxStaticBox( this, -1, _("Unitsync library") );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_dir_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_exec_sizer = new wxBoxSizer( wxVERTICAL );
  m_sync_sizer = new wxBoxSizer( wxVERTICAL );
  m_aconf_sizer = new wxBoxSizer( wxVERTICAL );
  m_exec_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_sync_loc_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_dir_sizer->Add( m_dir_text, 0, wxALL, 2 );
  m_dir_sizer->Add( m_dir_edit, 1 );
  m_dir_sizer->Add( m_dir_browse_btn );
  m_dir_sizer->Add( m_dir_find_btn );

  m_exec_loc_sizer->Add( m_exec_loc_text, 0, wxALL, 2 );
  m_exec_loc_sizer->Add( m_exec_edit, 1, wxEXPAND );
  m_exec_loc_sizer->Add( m_exec_browse_btn );
  m_exec_loc_sizer->Add( m_exec_find_btn );

  m_sync_loc_sizer->Add( m_sync_loc_text, 0, wxALL, 2 );
  m_sync_loc_sizer->Add( m_sync_edit, 1, wxEXPAND );
  m_sync_loc_sizer->Add( m_sync_browse_btn );
  m_sync_loc_sizer->Add( m_sync_find_btn );

  m_exec_box_sizer = new wxStaticBoxSizer( m_exec_box, wxVERTICAL );
  m_sync_box_sizer = new wxStaticBoxSizer( m_sync_box, wxVERTICAL );

  m_exec_box_sizer->Add( m_exec_def_radio, 0, wxALL, 2 );
  m_exec_box_sizer->Add( m_exec_spec_radio, 0, wxALL, 2 );
  m_exec_box_sizer->Add( m_exec_loc_sizer, 0, wxEXPAND | wxALL, 2 );

  m_sync_box_sizer->Add( m_sync_def_radio, 0, wxALL, 2 );
  m_sync_box_sizer->Add( m_sync_spec_radio, 0, wxALL, 2 );
  m_sync_box_sizer->Add( m_sync_loc_sizer, 0, wxEXPAND | wxALL, 2 );

  m_aconf_sizer->AddStretchSpacer();
  m_aconf_sizer->Add( m_auto_btn );

  m_main_sizer->Add( m_dir_sizer, 0, wxEXPAND | wxALL, 2 );
  m_main_sizer->Add( m_exec_box_sizer, 0, wxEXPAND | wxALL, 2 );
  m_main_sizer->Add( m_sync_box_sizer, 0, wxEXPAND | wxALL, 2 );
  m_main_sizer->Add( m_aconf_sizer, 0, wxEXPAND | wxALL, 2 );
  m_main_sizer->AddStretchSpacer();

  SetSizer( m_main_sizer );
  Layout();

  DoRestore();
}


SpringOptionsTab::~SpringOptionsTab()
{

}


void SpringOptionsTab::OnBrowseDir( wxCommandEvent& event )
{
  wxDirDialog dirpic( this, _("Choose a directory"), WX_STRING(sett().GetSpringDir()), wxDD_DEFAULT_STYLE );
  if ( dirpic.ShowModal() == wxID_OK ) m_dir_edit->SetValue( dirpic.GetPath() );
}


void SpringOptionsTab::OnBrowseExec( wxCommandEvent& event )
{
  wxFileDialog pic( this, _("Choose a Spring executable"), WX_STRING(sett().GetSpringDir()), wxString(_T("spring"))+wxString(EXEEXT), CHOOSE_EXE );
  if ( pic.ShowModal() == wxID_OK ) m_exec_edit->SetValue( pic.GetPath() );
}



void SpringOptionsTab::OnBrowseSync( wxCommandEvent& event )
{
  wxFileDialog pic( this, _("Choose a unitsync library"), WX_STRING(sett().GetSpringDir()), wxString(_T("unitsync"))+wxString(DLLEXT), CHOOSE_DLL );
  if ( pic.ShowModal() == wxID_OK ) m_sync_edit->SetValue( pic.GetPath() );
}


void SpringOptionsTab::OnApply( wxCommandEvent& event )
{
  sett().SetSpringDir( STL_STRING(m_dir_edit->GetValue()) );
  sett().SetUnitsyncLoc( STL_STRING( m_sync_edit->GetValue() ) );
  sett().SetSpringLoc( STL_STRING( m_exec_edit->GetValue() ) );
  sett().SetSpringUseDefLoc( m_exec_def_radio->GetValue() );
  sett().SetUnitsyncUseDefLoc( m_sync_def_radio->GetValue() );

  usync().FreeUnitsyncLib();
  usync().LoadUnitsyncLib();
}


void SpringOptionsTab::DoRestore()
{
  m_dir_edit->SetValue( WX_STRING(sett().GetSpringDir()) );
  m_sync_edit->SetValue( WX_STRING(sett().GetUnitsyncLoc()) );
  m_exec_edit->SetValue( WX_STRING(sett().GetSpringLoc()) );
  m_exec_def_radio->SetValue( sett().GetSpringUseDefLoc() );
  m_sync_def_radio->SetValue( sett().GetUnitsyncUseDefLoc() );
  m_exec_spec_radio->SetValue( !sett().GetSpringUseDefLoc() );
  m_sync_spec_radio->SetValue( !sett().GetUnitsyncUseDefLoc() );
}


void SpringOptionsTab::OnRestore( wxCommandEvent& event )
{
  DoRestore();
}

