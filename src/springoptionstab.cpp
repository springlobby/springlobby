//
// Class: SpringOptionsTab
//

#include "springoptionstab.h"
#include "ui.h"

BEGIN_EVENT_TABLE(SpringOptionsTab, wxPanel)

  EVT_BUTTON ( SPRING_DIRBROWSE, SpringOptionsTab::OnBrowseDir )

END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab( wxWindow* parent, Ui& ui ) : wxPanel( parent, -1 ),m_ui(ui)
{

  m_dir_text = new wxStaticText( this, -1, _T("Spring directory") );
  m_exec_loc_text = new wxStaticText( this, -1, _T("Location") );
  m_sync_loc_text = new wxStaticText( this, -1, _T("Location") );

  m_dir_browse_btn = new wxButton( this, -1, _T("Browse") );
  wxButton* m_dir_find_btn = new wxButton( this, -1, _T("Find") );
  wxButton* m_exec_browse_btn = new wxButton( this, -1, _T("Browse") );
  wxButton* m_exec_find_btn = new wxButton( this, -1, _T("Find") );
  wxButton* m_sync_browse_btn = new wxButton( this, -1, _T("Browse") );
  wxButton* m_sync_find_btn = new wxButton( this, -1, _T("Find") );
  wxButton* m_auto_btn = new wxButton( this, -1, _T("Auto Configure") );
    
  m_exec_def_radio = new wxRadioButton( this, -1, _T("Default location"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  m_exec_spec_radio = new wxRadioButton( this, -1, _T("Specify") );
  m_sync_def_radio = new wxRadioButton( this, -1, _T("Default location"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP  );
  m_sync_spec_radio = new wxRadioButton( this, -1, _T("Specify") );

  m_dir_edit = new wxTextCtrl( this, -1, _("") );
  m_exec_edit = new wxTextCtrl( this, -1, _("") );
  m_sync_edit = new wxTextCtrl( this, -1, _("") );

  m_exec_box = new wxStaticBox( this, -1, _T("Spring executable") );
  m_sync_box = new wxStaticBox( this, -1, _T("Unitsync library") );

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
}


SpringOptionsTab::~SpringOptionsTab()
{
  
}


void SpringOptionsTab::OnBrowseDir( wxCommandEvent& event )
{

}
