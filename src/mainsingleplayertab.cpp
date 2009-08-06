/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/icon.h>
#include <wx/log.h>

#include <stdexcept>

#include "aui/auimanager.h"
#include "aui/artprovider.h"
#include "aui/slbook.h"
#include "singleplayertab.h"
#include "battleoptionstab.h"
#include "mainsingleplayertab.h"
#include "battleroommmoptionstab.h"
#include "utils/debug.h"
#include "utils/conversion.h"

#include "images/battle.xpm"
#include "images/battle_settings.xpm"


MainSinglePlayerTab::MainSinglePlayerTab( wxWindow* parent )
    : wxScrolledWindow( parent, -1 )
{
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  GetAui().manager->AddPane( this, wxLEFT, _T("mainsingleplayertab") );
  m_tabs = new SLNotebook( this, -1, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE );
  m_tabs->SetArtProvider(new SLArtProvider);

  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxIcon(battle_xpm) );
  m_imagelist->Add( wxIcon(battle_settings_xpm) );

  m_sp_tab = new SinglePlayerTab( m_tabs, *this );
  m_tabs->AddPage( m_sp_tab, _("Game"), true, 0 );
  m_mm_opts_tab = new BattleroomMMOptionsTab<SinglePlayerBattle>( m_sp_tab->GetBattle(), m_tabs);
  m_tabs->InsertPage( 1, m_mm_opts_tab, _("Options"), false, wxIcon(battle_settings_xpm) );
  m_opts_tab = new BattleOptionsTab( m_tabs, m_sp_tab->GetBattle() );
  m_tabs->InsertPage( 2, m_opts_tab, _("Unit Restrictions"), false, wxIcon(battle_settings_xpm) );

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
      GetSinglePlayerTab().ResetUsername();
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
		GetMMOptionsTab().OnReloadControls( OptionsWrapper::MapOption );
  } catch(...) {}
}


void MainSinglePlayerTab::ReloadModOptContrls()
{
	try
	{
		GetMMOptionsTab().OnReloadControls( OptionsWrapper::ModOption );
  } catch(...) {}

}

SinglePlayerTab& MainSinglePlayerTab::GetSinglePlayerTab()
{
  ASSERT_EXCEPTION( m_sp_tab, _T("m_sp_tab == 0") );
	return *m_sp_tab;
}

BattleOptionsTab& MainSinglePlayerTab::GetOptionsTab()
{
  ASSERT_EXCEPTION( m_opts_tab, _T("m_opts_tab == 0") );
	return *m_opts_tab;
}


BattleroomMMOptionsTab<SinglePlayerBattle>& MainSinglePlayerTab::GetMMOptionsTab()
{
  ASSERT_EXCEPTION( m_mm_opts_tab, _T("m_mm_opts_tab == 0") );
	return *m_mm_opts_tab;
}


void MainSinglePlayerTab::ReloadPresetList()
{
  try
  {
    GetSinglePlayerTab().UpdatePresetList();
  } catch (...) {}
  try
  {
    GetMMOptionsTab().UpdatePresetList();
  } catch (...) {}
}


void MainSinglePlayerTab::Update( const wxString& Tag )
{

  try
  {
    GetSinglePlayerTab().Update( Tag );
  } catch (...) {}

  try
  {
    GetOptionsTab().UpdateBattle( Tag );
  } catch (...) {}

}
