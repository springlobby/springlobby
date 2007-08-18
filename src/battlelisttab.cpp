/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleListTab
//

#include "battlelisttab.h"
#include "battlelistctrl.h"
#include "battle.h"
#include "ui.h"
#include "chatpanel.h"
#include "utils.h"
#include "uiutils.h"
#include "hostbattledialog.h"
#include "server.h"
#include "settings.h"
#include "iunitsync.h"
#include "mapctrl.h"
#include "nicklistctrl.h"

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>

BEGIN_EVENT_TABLE(BattleListTab, wxPanel)

  EVT_BUTTON              ( BATTLE_JOIN, BattleListTab::OnJoin     )
  EVT_BUTTON              ( BATTLE_HOST, BattleListTab::OnHost     )
  EVT_LIST_ITEM_ACTIVATED ( BATTLE_JOIN, BattleListTab::OnListJoin )
  EVT_LIST_ITEM_SELECTED  ( BLIST_LIST,  BattleListTab::OnSelect   )

END_EVENT_TABLE()


BattleListTab::BattleListTab( wxWindow* parent, Ui& ui ) :
  wxPanel( parent, -1 ),
  m_ui(ui),
  m_sel_battle(0)
{
  wxBoxSizer* m_main_sizer;
  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  
  m_battle_list = new BattleListCtrl( this );
  m_main_sizer->Add( m_battle_list, 1, wxALL|wxEXPAND, 5 );
  
  wxBoxSizer* m_info_sizer;
  m_info_sizer = new wxBoxSizer( wxHORIZONTAL );
  
  m_minimap = new MapCtrl( this, 100, 0, m_ui, true );
  m_info_sizer->Add( m_minimap, 0, wxALL, 5 );
  
  wxFlexGridSizer* m_data_sizer;
  m_data_sizer = new wxFlexGridSizer( 4, 2, 0, 0 );
  
  m_map_lbl = new wxStaticText( this, wxID_ANY, _("Map:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_map_lbl, 0, wxALL, 5 );
  
  m_map_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_map_text, 0, wxALL, 5 );
  
  m_mod_lbl = new wxStaticText( this, wxID_ANY, _("Mod:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_mod_lbl, 0, wxALL, 5 );
  
  m_mod_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_mod_text, 0, wxALL, 5 );
  
  m_players_lbl = new wxStaticText( this, wxID_ANY, _("Players:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_players_lbl, 0, wxALL, 5 );
  
  m_players_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_players_text, 0, wxALL, 5 );
  
  m_spec_lbl = new wxStaticText( this, wxID_ANY, _("Spectators:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_spec_lbl, 0, wxALL, 5 );
  
  m_spec_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_data_sizer->Add( m_spec_text, 0, wxALL, 5 );
  
  m_info_sizer->Add( m_data_sizer, 1, wxEXPAND, 5 );
  
  m_players = new NickListCtrl( this, false );
  m_info_sizer->Add( m_players, 1, wxALL|wxEXPAND, 5 );
  
  m_main_sizer->Add( m_info_sizer, 0, wxEXPAND, 5 );
  
  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND, 5 );
  
  wxBoxSizer* m_buttons_sizer;
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
  
  m_filter_lbl = new wxStaticText( this, wxID_ANY, _("Filter:"), wxDefaultPosition, wxDefaultSize, 0 );
  m_buttons_sizer->Add( m_filter_lbl, 0, wxALL, 5 );
  
  m_filter_combo = new wxComboBox( this, wxID_ANY, _("search"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
  m_filter_combo->Append( _("All") );
  m_filter_combo->Append( _("Open only") );
  m_buttons_sizer->Add( m_filter_combo, 0, 0, 5 );
  
  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );
  
  m_host_btn = new wxButton( this, BATTLE_HOST, _("Host new..."), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_host_btn, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );
  
  m_join_btn = new wxButton( this, BATTLE_JOIN, _("Join"), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_join_btn, 0, wxBOTTOM|wxRIGHT, 5 );
  
  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );
  
  this->SetSizer( m_main_sizer );
  this->Layout();
  
  SelectBattle(0);
}


BattleListTab::~BattleListTab()
{

}


void BattleListTab::SelectBattle( Battle* battle )
{
  m_sel_battle = battle;
  m_minimap->SetBattle( m_sel_battle );
  m_players->ClearUsers();
  if ( m_sel_battle != 0 ) {
    m_map_text->SetLabel( RefineMapname( WX_STRING(m_sel_battle->opts().mapname) ) );
    m_mod_text->SetLabel( WX_STRING(m_sel_battle->opts().modname) );
    m_players_text->SetLabel( wxString::Format( _T("%d / %d"), m_sel_battle->GetNumUsers() - m_sel_battle->opts().spectators, m_sel_battle->opts().maxplayers ) );
    m_spec_text->SetLabel( wxString::Format( _T("%d"), m_sel_battle->opts().spectators ) );
    for ( unsigned int i = 0; i < m_sel_battle->GetNumUsers(); i++ ) {
      m_players->AddUser( m_sel_battle->GetUser( i ) );
    }
  } else {
    m_map_text->SetLabel( wxEmptyString );
    m_mod_text->SetLabel( wxEmptyString );
    m_players_text->SetLabel(  _T("0 / 0") );
    m_spec_text->SetLabel( _T("0") );
  }
}


void BattleListTab::RemoveAllBattles()
{
  while ( m_battle_list->GetItemCount() > 0 ) m_battle_list->DeleteItem( 0 );
}


void BattleListTab::AddBattle( Battle& battle )
{
  m_battle_list->AddBattle( battle );
}


void BattleListTab::RemoveBattle( Battle& battle )
{
  if ( &battle == m_sel_battle ) SelectBattle( 0 );
  m_battle_list->RemoveBattle( battle );
}


void BattleListTab::UpdateBattle( Battle& battle )
{
  m_battle_list->UpdateBattle( battle );
  if ( &battle == m_sel_battle ) SelectBattle( m_sel_battle );
}


void BattleListTab::UpdateList()
{
  m_battle_list->UpdateList();
}


void BattleListTab::OnHost( wxCommandEvent& event )
{
  if ( !m_ui.IsConnected() ) {
    wxMessageBox( _("You cannot a host game when beeing offline."), _("Not Online."), wxOK );
    return;
  }
  HostBattleDialog dlg( this );
  if ( dlg.ShowModal() == wxID_OK ) {
    BattleOptions bo;
    bo.description = sett().GetLastHostDescription();
    bo.port = sett().GetLastHostPort();

    // Get selected mod from unitsync.
    UnitSyncMod mod;
    try {
      mod = usync()->GetMod( sett().GetLastHostMod() );
      bo.modhash = mod.hash;
      bo.modname = mod.name;
    } catch ( ... ) {
      wxMessageBox( _("Battle not started beacuse the mod you selected could not be fond. "), _("Error starting battle."), wxOK );
      return;
    }

    UnitSyncMap map;
    std::string mname = sett().GetLastHostMap();
    if ( mname != "" ) map = usync()->GetMap( mname );
    else map = usync()->GetMap( 0 ); // TODO: This might fail.
    bo.maphash = map.hash;
    bo.mapname = map.name;

    bo.maxplayers = sett().GetLastHostPlayerNum();

/*
    bo.nattype,
    bo.startmetal,
    bo.startenergy,
    bo.maxunits,
    bo.starttype,
    bo.gametype,
    bo.limitdgun,
    bo.dimmms,
    bo.ghostedbuildings,
)*/

    m_ui.GetServer().HostBattle( bo, sett().GetLastHostPassword() );
  }
}


void BattleListTab::OnJoin( wxCommandEvent& event )
{
  assert( m_battle_list != 0 );
  if ( m_battle_list->GetSelectedIndex() < 0 ) return;

  Battle& battle = *((Battle*)m_battle_list->GetItemData( m_battle_list->GetSelectedIndex() ));

  DoJoin( battle );
}


void BattleListTab::OnListJoin( wxListEvent& event )
{
  assert( m_battle_list != 0 );
  if ( event.GetIndex() < 0 ) return;

  Battle& battle = *((Battle*)m_battle_list->GetItemData( event.GetIndex() ));

  DoJoin( battle );
}


void BattleListTab::DoJoin( Battle& battle )
{
  if ( !battle.HasMod() ) {
    if (wxMessageBox( _("You need to download the mod before you can join this game.\n\nDo you want me to take you to the download page?"), _("Mod not awailable"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString mod = WX_STRING(battle.opts().modname);
      mod.Replace(_T(" "), _T("%20") );
      wxString url = _T("http://spring.unknown-files.net/page/search/1/14/") + mod + _T("/");
      wxLaunchDefaultBrowser( url );
    }
    return;
  }  

  if ( !battle.IsMapAvailable() ) {
    if (wxMessageBox( _("You need to download the map to be able to play in this game.\n\nDo you want me to take you to the download page?"), _("Map not awailable"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString map = WX_STRING(battle.opts().mapname);
      map.Replace(_T(" "), _T("%20") );
      wxString url = _T("http://spring.unknown-files.net/page/search/1/13/") + map + _T("/");
      wxLaunchDefaultBrowser( url );
    }
  }
  
  if ( battle.opts().ispassworded ) {
    wxPasswordEntryDialog pw( this, _("Battle password"), _("Enter password") );
    if ( pw.ShowModal() == wxID_OK ) battle.Join( STD_STRING(pw.GetValue()) );
  } else {
    battle.Join();
  }
}


void BattleListTab::OnSelect( wxListEvent& event )
{
  debug_func("");
  if ( event.GetIndex() == -1 ) {
    SelectBattle( 0 );
  } else {
    Battle& battle = *((Battle*)m_battle_list->GetItemData( event.GetIndex() ));
    SelectBattle( &battle );
  }
}

