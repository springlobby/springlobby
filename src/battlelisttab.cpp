/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <stdexcept>

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
#include "mainwindow.h"
#include "mainjoinbattletab.h"
#include "battlelistfilter.h"
#include "iconimagelist.h"


BEGIN_EVENT_TABLE(BattleListTab, wxPanel)

  EVT_BUTTON              ( BATTLE_JOIN              , BattleListTab::OnJoin        )
  EVT_BUTTON              ( BATTLE_HOST              , BattleListTab::OnHost        )
  EVT_TOGGLEBUTTON        ( BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter      )
  EVT_LIST_ITEM_ACTIVATED ( BATTLE_JOIN              , BattleListTab::OnListJoin    )
  EVT_LIST_ITEM_SELECTED  ( BLIST_LIST               , BattleListTab::OnSelect      )
  EVT_CHECKBOX            ( BATTLE_LIST_FILTER_ACTIV , BattleListTab::OnFilterActiv )

END_EVENT_TABLE()


BattleListTab::BattleListTab( wxWindow* parent, Ui& ui ) :
  wxPanel( parent, -1 ),
  m_ui(ui),
  m_sel_battle(0)
{
  wxBoxSizer* m_main_sizer;
  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* m_filter_sizer;
  m_filter_sizer = new wxBoxSizer( wxVERTICAL );

  m_filter = new BattleListFilter( this , wxID_ANY, this ,wxDefaultPosition, wxSize( -1,-1 ), wxEXPAND );
  m_filter_sizer->Add( m_filter, 0, wxEXPAND, 5);

  m_main_sizer->Add( m_filter_sizer, 0, wxEXPAND, 5);

  wxBoxSizer* m_battlelist_sizer;
  m_battlelist_sizer = new wxBoxSizer( wxVERTICAL );

  m_battle_list = new BattleListCtrl( this, m_ui );
  m_battlelist_sizer->Add( m_battle_list, 1, wxALL|wxEXPAND, 5 );

  m_main_sizer->Add( m_battlelist_sizer, 1, wxEXPAND, 5 );;

  wxBoxSizer* m_info_sizer;
  m_info_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_minimap = new MapCtrl( this, 100, 0, m_ui, true, true, false, false );
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

  m_players = new NickListCtrl( this, m_ui, false );
  m_info_sizer->Add( m_players, 1, wxALL|wxEXPAND, 5 );

  m_main_sizer->Add( m_info_sizer, 0, wxEXPAND, 5 );

  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_buttons_sizer;
  m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_show = new wxToggleButton( this, BATTLE_LIST_FILTER_BUTTON , wxT(" Filter "), wxDefaultPosition , wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_filter_show, 0, 0, 5 );

	m_filter_activ = new wxCheckBox( this, BATTLE_LIST_FILTER_ACTIV , wxT("Activated"), wxDefaultPosition, wxDefaultSize, 0 );
  m_buttons_sizer->Add( m_filter_activ, 1, wxALL|wxEXPAND, 5 );

  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_host_btn = new wxButton( this, BATTLE_HOST, _("Host new..."), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_host_btn, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5 );

  m_join_btn = new wxButton( this, BATTLE_JOIN, _("Join"), wxDefaultPosition, wxSize( -1,28 ), 0 );
  m_buttons_sizer->Add( m_join_btn, 0, wxBOTTOM|wxRIGHT, 5 );

  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

  m_filter->Hide();

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
    m_map_text->SetLabel( RefineMapname( WX_STRING(m_sel_battle->GetMapName()) ) );
    m_mod_text->SetLabel( WX_STRING(m_sel_battle->GetModName()) );
    m_players_text->SetLabel( wxString::Format( _T("%d / %d"), m_sel_battle->GetNumUsers() - m_sel_battle->GetSpectators(), m_sel_battle->GetMaxPlayers() ) );
    m_spec_text->SetLabel( wxString::Format( _T("%d"), m_sel_battle->GetSpectators() ) );
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

void BattleListTab::AddBattle( Battle& battle ) {

  if ( m_filter->GetActiv() and !m_filter->FilterBattle( battle ) ) {
    return;
  }
  int index = m_battle_list->InsertItem( 0, IconImageList::GetBattleStatusIcon( battle ) );
  ASSERT_LOGIC( index != -1, "index = -1" );
  m_battle_list->SetItemData(index, (long)battle.GetBattleId() );
  battle.SetGUIListActiv( true );

  ASSERT_LOGIC( index != -1, "index = -1" );
  //wxListItem item;
  //item.SetId( index );

 // ASSERT_LOGIC( m_battle_list->GetItem( item ), "!GetItem" );

  m_battle_list->SetItemImage( index, IconImageList::GetBattleStatusIcon( battle ) );
  m_battle_list->SetItemColumnImage( index, 2, IconImageList::GetRankIcon( battle.GetRankNeeded(), false ) );
  m_battle_list->SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( battle.GetFounder().GetCountry() ) );
  m_battle_list->SetItem( index, 3, WX_STRING(battle.GetDescription()) );
  m_battle_list->SetItem( index, 4, RefineMapname( battle.GetMapName() ), battle.MapExists()?ICON_EXISTS:ICON_NEXISTS );
  m_battle_list->SetItem( index, 5, RefineModname( battle.GetModName() ), battle.ModExists()?ICON_EXISTS:ICON_NEXISTS );
  m_battle_list->SetItem( index, 6, WX_STRING(battle.GetFounder().GetNick()) );
  m_battle_list->SetItem( index, 7, wxString::Format(_T("%d"), battle.GetSpectators()) );
  m_battle_list->SetItem( index, 8, wxString::Format(_T("%d"), battle.GetNumUsers() - battle.GetSpectators() ) );
  m_battle_list->SetItem( index, 9, wxString::Format(_T("%d"), battle.GetMaxPlayers()) );

  m_battle_list->Sort();
}


void BattleListTab::RemoveBattle( Battle& battle ) {
  if ( &battle == m_sel_battle ) SelectBattle( 0 );
  for (int i = 0; i < m_battle_list->GetItemCount() ; i++ ) {
    if ( battle.GetBattleId() == (int)m_battle_list->GetItemData( i ) ) {
      m_battle_list->DeleteItem( i );
      break;
    }
  }
  battle.SetGUIListActiv( false );
  m_battle_list->Sort();
}


void BattleListTab::UserUpdate( User& user )
{
  if (m_sel_battle and user.GetBattle() == m_sel_battle) { m_players->UserUpdated( user ); }
}


void BattleListTab::UpdateBattle( Battle& battle )
{
  if ( !battle.GetGUIListActiv() ) {
    AddBattle( battle );
    return;
  }

  if ( m_filter->GetActiv() and !m_filter->FilterBattle( battle ) ) {
    RemoveBattle( battle );
    return;
  }

  int index = -1;
  for (int i = 0; i < m_battle_list->GetItemCount() ; i++ ) {
    if ( battle.GetBattleId() == (int)m_battle_list->GetItemData( i ) ) {
      index = i;
      break;
    }
  }

  ASSERT_LOGIC( index != -1, "index = -1" );

  //wxListItem item;
  //item.SetId( index );

  //ASSERT_LOGIC( m_battle_list->GetItem( item ), "!GetItem" );

  //Battle& battle = m_ui.GetServer().battles_iter.GetBattle( m_battle_list->GetItemData( index ) );

  m_battle_list->SetItemImage( index, IconImageList::GetBattleStatusIcon( battle ) );
  m_battle_list->SetItemColumnImage( index, 2, IconImageList::GetRankIcon( battle.GetRankNeeded(), false ) );
  m_battle_list->SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( battle.GetFounder().GetCountry() ) );
  m_battle_list->SetItem( index, 3, WX_STRING(battle.GetDescription()) );
  m_battle_list->SetItem( index, 4, RefineMapname( battle.GetMapName() ), battle.MapExists()?ICON_EXISTS:ICON_NEXISTS );
  m_battle_list->SetItem( index, 5, RefineModname( battle.GetModName() ), battle.ModExists()?ICON_EXISTS:ICON_NEXISTS );
  m_battle_list->SetItem( index, 6, WX_STRING(battle.GetFounder().GetNick()) );
  m_battle_list->SetItem( index, 7, wxString::Format(_T("%d"), battle.GetSpectators()) );
  m_battle_list->SetItem( index, 8, wxString::Format(_T("%d"), battle.GetNumUsers() - battle.GetSpectators() ) );
  m_battle_list->SetItem( index, 9, wxString::Format(_T("%d"), battle.GetMaxPlayers()) );


  if ( &battle == m_sel_battle ) SelectBattle( m_sel_battle );
  m_battle_list->Sort();

}


void BattleListTab::RemoveAllBattles() {
  SelectBattle( 0 );
  m_ui.GetServer().battles_iter->IteratorBegin();
  while (! m_ui.GetServer().battles_iter->EOL() ) {
    m_ui.GetServer().battles_iter->GetBattle().SetGUIListActiv( false );
  }
  m_battle_list->DeleteAllItems();
}


void BattleListTab::UpdateList() {
  m_ui.GetServer().battles_iter->IteratorBegin();
  while (! m_ui.GetServer().battles_iter->EOL() ) {
    Battle& b = m_ui.GetServer().battles_iter->GetBattle();
    UpdateBattle(b);
  }
}


void BattleListTab::OnHost( wxCommandEvent& event )
{
  if ( !m_ui.IsConnected() ) {
    wxMessageBox( _("You cannot host a game while offline. Please connect to a lobby server."), _("Not Online."), wxOK );
    return;
  }
  if ( !m_ui.IsSpringCompatible() ){
    wxMessageBox(_("Hosting is disabled due to the incompatible version you're using"), _("Spring error"), wxICON_EXCLAMATION);
    return;
  }
  if ( m_ui.IsSpringRunning() ) {
    wxMessageBox(_("You already are running a Spring instance, close it first in order to be able to host a new game"), _("Spring error"), wxICON_EXCLAMATION );
    return;
  }
  Battle* battle = m_ui.mw().GetJoinTab().GetCurrentBattle();
  if ( battle != 0 ) {
    if ( m_ui.Ask( _("Already in a battle"), _("You are already in a battle.\n\nDo you want to leave current battle to start a new?") ) ) {
      battle->Leave();
      m_ui.mw().GetJoinTab().LeaveCurrentBattle();
    } else {
      return;
    }
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
      wxMessageBox( _("Battle not started beacuse the mod you selected could not be found. "), _("Error starting battle."), wxOK );
      return;
    }

    UnitSyncMap map;
    std::string mname = sett().GetLastHostMap();

    try {
      if ( mname != "" ) map = usync()->GetMap( mname );
      else if ( usync()->GetNumMaps() <= 0 ) {
        wxMessageBox( _("Couldn't find any maps in you spring installation. This could happen when you set the Spring settings incorrectly."), _("No maps found"), wxOK );
        return;
      } else {
        map = usync()->GetMap( 0 );
      }
    } catch ( ... ) {
      wxMessageBox( _("Couldn't find any maps in you spring installation. This could happen when you set the Spring settings incorrectly."), _("No maps found"), wxOK );
      return;
    }
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


void BattleListTab::OnFilter( wxCommandEvent& event )
{
  if (m_filter_show->GetValue()) {
    m_filter->Show(  );
    this->Layout();
  }
  else {
    m_filter->Hide(  );
    this->Layout();
  }
}


void BattleListTab::OnFilterActiv( wxCommandEvent& event )
{
  m_filter->SetActiv( m_filter_activ->GetValue() );
}


void BattleListTab::OnJoin( wxCommandEvent& event )
{
  ASSERT_LOGIC( m_battle_list != 0, "m_battle_list = 0" );
  if ( m_battle_list->GetSelectedIndex() < 0 ) return;

  DoJoin( m_ui.GetServer().battles_iter->GetBattle( m_battle_list->GetSelectedIndex() ) );

}


void BattleListTab::OnListJoin( wxListEvent& event )
{
  ASSERT_LOGIC( m_battle_list != 0, "m_battle_list = 0" );
  if ( event.GetIndex() < 0 ) return;

  DoJoin( m_ui.GetServer().battles_iter->GetBattle( m_battle_list->GetItemData( event.GetIndex() ) ) );
}


void BattleListTab::DoJoin( Battle& battle )
{
  if ( !m_ui.IsSpringCompatible() ){
    wxMessageBox(_("Joining battles is disabled due to the incompatible spring version you're using."), _("Spring error"), wxICON_EXCLAMATION);
    return;
  }

  Battle* curbattle = m_ui.mw().GetJoinTab().GetCurrentBattle();
  if ( curbattle != 0 ) {
    if ( m_ui.Ask( _("Already in a battle"), _("You are already in a battle.\n\nDo you want to leave current battle to and join this one?") ) ) {
      curbattle->Leave();
      m_ui.mw().GetJoinTab().LeaveCurrentBattle();
    } else {
      return;
    }
  }

  if ( m_ui.IsSpringRunning() ) {
    wxMessageBox(_("You already are running a Spring instance, close it first in order to be able to join another battle."), _("Spring error"), wxICON_EXCLAMATION );
    return;
  }

  if ( !battle.ModExists() ) {
    if (wxMessageBox( _("You need to download the mod before you can join this game.\n\nDo you want me to take you to the download page?"), _("Mod not awailable"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString mod = battle.GetModName();
      m_ui.DownloadMod ( mod );
    }
    return;
  }

  if ( !battle.MapExists() ) {
    if (wxMessageBox( _("You need to download the map to be able to play in this game.\n\nDo you want me to take you to the download page?"), _("Map not awailable"), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
      wxString map = battle.GetMapName();
      m_ui.DownloadMap ( map );
    }
  }

  if ( battle.IsPassworded() ) {
    wxPasswordEntryDialog pw( this, _("Battle password"), _("Enter password") );
    if ( pw.ShowModal() == wxID_OK ) battle.Join( STD_STRING(pw.GetValue()) );
  } else {
    battle.Join();
  }
}


void BattleListTab::EnableJoinButton()
{
  m_join_btn->Enable();
}


void BattleListTab::DisableJoinButton()
{
  m_join_btn->Disable();
}


void BattleListTab::EnableHostButton()
{
  m_host_btn->Enable();
}


void BattleListTab::DisableHostButton()
{
  m_host_btn->Disable();
}


void BattleListTab::OnSelect( wxListEvent& event )
{
  debug_func("");
  if ( event.GetIndex() == -1 ) {
    SelectBattle( 0 );
  } else {
    SelectBattle( &m_ui.GetServer().battles_iter->GetBattle( m_battle_list->GetItemData( event.GetIndex() ) ) );
  }
}


void BattleListTab::OnUnitSyncReloaded()
{
  if ( ! m_ui.GetServerStatus() ) { return; }

  m_ui.GetServer().battles_iter->IteratorBegin();
  while (! m_ui.GetServer().battles_iter->EOL() ) {
    Battle& b = m_ui.GetServer().battles_iter->GetBattle();
    b.OnUnitSyncReloaded();
  }
  UpdateList();
  m_minimap->UpdateMinimap();
}
