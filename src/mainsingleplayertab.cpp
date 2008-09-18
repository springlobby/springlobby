/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/notebook.h>
#include <wx/icon.h>
#include <stdexcept>

#include "singleplayertab.h"
#include "battleoptionstab.h"
#include "mainsingleplayertab.h"
#include "battleroommmoptionstab.h"
#include "ui.h"
#include "utils.h"

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

  m_sp_tab = new SinglePlayerTab( m_tabs, m_ui, *this );
  m_tabs->AddPage( m_sp_tab, _("Game"), true, 0 );
  m_opts_tab = new BattleOptionsTab( m_tabs, m_ui, m_sp_tab->GetBattle(), true );
  m_tabs->InsertPage( 1, m_opts_tab, _("Options"), false, 1 );
  m_mm_opts_tab = new BattleroomMMOptionsTab( m_sp_tab->GetBattle(), m_tabs);
  m_tabs->InsertPage( 2, m_mm_opts_tab, _("Map/Mod Options"), false, 1 );

  m_main_sizer->Add( m_tabs, 1, wxEXPAND );

  SetSizer( m_main_sizer );
  Layout();
}


MainSinglePlayerTab::~MainSinglePlayerTab()
{

}


void MainSinglePlayerTab::UpdateMinimap()
{
  try
  {
    GetSinglePlayerTab().UpdateMinimap();
  } catch(...){}
}


void MainSinglePlayerTab::OnUnitSyncReloaded()
{
  wxLogDebugFunc( _T("") );
  try
  {
    wxLogMessage( _T("Reloading map list") );
    GetSinglePlayerTab().ReloadMaplist();
    wxLogMessage( _T("Reloading mod list") );
    GetSinglePlayerTab().ReloadModlist();
    wxLogMessage( _T("Reloading minimap") );
    GetSinglePlayerTab().UpdateMinimap();
  } catch (...) {}

}


void MainSinglePlayerTab::ReloadRestrictions()
{
  try
  {
    GetOptionsTab().ReloadRestrictions();
  } catch(...) {}
}


void MainSinglePlayerTab::ReloadMapOptContrls()
{
  try
  {
		GetMMOptionsTab().OnReloadControls( MapOption );
  } catch(...) {}
}


void MainSinglePlayerTab::ReloadModOptContrls()
{
	try
	{
		GetMMOptionsTab().OnReloadControls( ModOption );
  } catch(...) {}

}

SinglePlayerTab& MainSinglePlayerTab::GetSinglePlayerTab()
{
  ASSERT_RUNTIME( m_sp_tab, _T("m_sp_tab == 0") );
	return *m_sp_tab;
}

BattleOptionsTab& MainSinglePlayerTab::GetOptionsTab()
{
  ASSERT_RUNTIME( m_opts_tab, _T("m_opts_tab == 0") );
	return *m_opts_tab;
}


BattleroomMMOptionsTab& MainSinglePlayerTab::GetMMOptionsTab()
{
  ASSERT_RUNTIME( m_mm_opts_tab, _T("m_mm_opts_tab == 0") );
	return *m_mm_opts_tab;
}


void MainSinglePlayerTab::ReloadPresetList()
{
  try
  {
    GetOptionsTab().UpdatePresetList();
  } catch (...) {}
}
