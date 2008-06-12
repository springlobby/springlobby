/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/icon.h>
#include <stdexcept>

#ifndef HAVE_WX26
#include <wx/aui/auibook.h>
#else
#include <wx/listbook.h>
#endif

#include "singleplayertab.h"
#include "battleoptionstab.h"
#include "mainsingleplayertab.h"
#include "battleroommmoptionstab.h"
#include "ui.h"
#include "utils.h"

#include "images/battle.xpm"
#include "images/battle_settings.xpm"


MainSinglePlayerTab::MainSinglePlayerTab( wxWindow* parent, Ui& ui ):
wxScrolledWindow( parent, -1 ),
m_ui(ui)
{
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  #ifdef HAVE_WX26
  m_tabs = new wxNotebook( this, -1, wxDefaultPosition, wxDefaultSize, wxLB_TOP );
  #else
  m_tabs = new wxAuiNotebook( this, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE );
  #endif

  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxIcon(battle_xpm) );
  m_imagelist->Add( wxIcon(battle_settings_xpm) );

  #ifdef HAVE_WX26
  m_tabs->AssignImageList( m_imagelist );
  #endif

  m_sp_tab = new SinglePlayerTab( m_tabs, m_ui, *this );
  m_tabs->AddPage( m_sp_tab, _("Game"), true, 0 );
  m_opts_tab = new BattleOptionsTab( m_tabs, m_ui, m_sp_tab->GetBattle(), true );
  #ifdef HAVE_WX26
  m_tabs->InsertPage( 1, m_opts_tab, _("Options"), false, 1 );
  #else
  m_tabs->InsertPage( 1, m_opts_tab, _("Options"), false, wxIcon(battle_settings_xpm) );
  #endif
  m_mm_opts_tab = new BattleroomMMOptionsTab( m_sp_tab->GetBattle(), m_tabs);
  #ifdef HAVE_WX26
  m_tabs->InsertPage( 2, m_mm_opts_tab, _("Map/Mod Options"), false, 1 );
  #else
  m_tabs->InsertPage( 2, m_mm_opts_tab, _("Map/Mod Options"), false, wxIcon(battle_settings_xpm) );
  #endif

  m_main_sizer->Add( m_tabs, 1, wxEXPAND );

  SetScrollRate( 3, 3 );
  SetSizer( m_main_sizer );
  Layout();
}


MainSinglePlayerTab::~MainSinglePlayerTab()
{

}


void MainSinglePlayerTab::UpdateMinimap()
{
  m_sp_tab->UpdateMinimap();
}


void MainSinglePlayerTab::OnUnitSyncReloaded()
{
  wxLogDebugFunc( _T("") );
  try
  {
    ASSERT_LOGIC( m_sp_tab != 0, _T("m_sp_tab = 0") );
  } catch (...) { return; }
  wxLogMessage( _T("Reloading map list") );
  m_sp_tab->ReloadMaplist();
  wxLogMessage( _T("Reloading mod list") );
  m_sp_tab->ReloadModlist();
  wxLogMessage( _T("Reloading minimap") );
  m_sp_tab->UpdateMinimap();
}


void MainSinglePlayerTab::ReloadRestrictions()
{
  m_opts_tab->ReloadRestrictions();
}


void MainSinglePlayerTab::ReloadMapOptContrls()
{
  if (m_mm_opts_tab != 0)
		m_mm_opts_tab->OnRefreshControls( MapOption );
}


void MainSinglePlayerTab::ReloadModOptContrls()
{
	if (m_mm_opts_tab != 0)
		m_mm_opts_tab->OnRefreshControls( ModOption );

}
