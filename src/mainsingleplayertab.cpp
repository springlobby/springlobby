/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/notebook.h>

#include "singleplayertab.h"
#include "mainsingleplayertab.h"
#include "ui.h"
#include "images/battle.xpm"
#include "images/battle_settings.xpm"


MainSinglePlayerTab::MainSinglePlayerTab( wxWindow* parent, Ui& ui ):
wxPanel( parent, -1 ),
m_ui(ui)
{
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_tabs = new wxNotebook( this, -1, wxDefaultPosition, wxDefaultSize, wxNB_TOP );

  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxIcon(battle_xpm) );
  m_imagelist->Add( wxIcon(battle_settings_xpm) );

  m_tabs->AssignImageList( m_imagelist );

  m_sp_tab = new SinglePlayerTab( m_tabs, m_ui );
  m_tabs->AddPage( m_sp_tab, _("Game"), true, 0 );

  m_main_sizer->Add( m_tabs, 1, wxEXPAND );

  SetSizer( m_main_sizer );
  Layout();
}


 MainSinglePlayerTab::~MainSinglePlayerTab()
{

}

