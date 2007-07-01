//
// Class: MainOptionsTab
//

#include <wx/icon.h>
#include "mainoptionstab.h"
#include "ui.h"

#include "images/spring.xpm"

BEGIN_EVENT_TABLE(MainOptionsTab, wxPanel)

    EVT_BUTTON ( wxID_APPLY, MainOptionsTab::OnApply )
    EVT_BUTTON ( wxID_REVERT, MainOptionsTab::OnRestore )

END_EVENT_TABLE()


MainOptionsTab::MainOptionsTab( wxWindow* parent, Ui& ui ) : wxPanel( parent, -1 ),m_ui(ui)
{
  m_tabs = new wxNotebook( this, OPTIONS_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );
  
  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxIcon(spring_xpm) );
  
  m_tabs->AssignImageList( m_imagelist );
  
  m_spring_opts = new SpringOptionsTab( m_tabs, m_ui );
  m_tabs->AddPage( m_spring_opts, _T("Spring"), true, 0 );

  m_restore_btn = new wxButton( this, wxID_REVERT, _T("Restore") );
  m_apply_btn = new wxButton( this, wxID_APPLY, _T("Apply") );

  m_button_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_button_sizer->Add( m_restore_btn, 0, wxALL, 2 );
  m_button_sizer->AddStretchSpacer();
  m_button_sizer->Add( m_apply_btn, 0, wxALL, 2 );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_main_sizer->Add( m_tabs, 1, wxEXPAND );
  m_main_sizer->Add( m_button_sizer, 0, wxEXPAND );

  SetSizer( m_main_sizer );
  Layout();
}


MainOptionsTab::~MainOptionsTab()
{
  
}


void MainOptionsTab::OnApply( wxCommandEvent& event )
{
  m_spring_opts->OnApply( event );
}


void MainOptionsTab::OnRestore( wxCommandEvent& event )
{
  m_spring_opts->OnRestore( event );
}



