/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <stdexcept>

#include "battlelistctrl.h"
#include "utils.h"
#include "user.h"
#include "iconimagelist.h"
#include "battle.h"
#include "uiutils.h"
#include "ui.h"
#include "server.h"


BEGIN_EVENT_TABLE(BattleListCtrl, wxListCtrl)

  EVT_LIST_ITEM_SELECTED   ( BLIST_LIST, BattleListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( BLIST_LIST, BattleListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( BLIST_LIST, BattleListCtrl::OnDeselected )
  EVT_LIST_ITEM_RIGHT_CLICK( BLIST_LIST, BattleListCtrl::OnListRightClick )
  EVT_LIST_COL_CLICK       ( BLIST_LIST, BattleListCtrl::OnColClick )
  EVT_MENU                 ( BLIST_DLMAP, BattleListCtrl::OnDLMap )
  EVT_MENU                 ( BLIST_DLMOD, BattleListCtrl::OnDLMod )

END_EVENT_TABLE()


Ui* BattleListCtrl::m_ui_for_sort = NULL;

BattleListCtrl::BattleListCtrl( wxWindow* parent, Ui& ui ):
  wxListCtrl(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL ),
  m_selected(NULL),
  m_ui(ui)
{

  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );

  wxListItem col;

  col.SetText( _T("s") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _T("c") );
  col.SetImage( -1 );
  InsertColumn( 1, col );

  col.SetText( _T("r") );
  col.SetImage( -1 );
  InsertColumn( 2, col );

  col.SetText( _("Description") );
  col.SetImage( ICON_DOWN );
  InsertColumn( 3, col );

  col.SetText( _("Map") );
  col.SetImage( -1 );
  InsertColumn( 4, col );

  col.SetText( _("Mod") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _("Host") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _("p") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  col.SetText( _("m") );
  col.SetImage( -1 );
  InsertColumn( 9, col );




  m_sortorder[0].col = 3;
  m_sortorder[0].direction = false;
  m_sortorder[1].col = 8;
  m_sortorder[1].direction = false;
  m_sortorder[2].col = 5;
  m_sortorder[2].direction = false;
  m_sortorder[3].col = 4;
  m_sortorder[3].direction = false;
  Sort( );

#ifdef __WXMSW__
  SetColumnWidth( 0, 45 );
#else
  SetColumnWidth( 0, 20 );
#endif
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 170 );
  SetColumnWidth( 4, 140 );
  SetColumnWidth( 5, 130 );
  SetColumnWidth( 6, 110 );
  SetColumnWidth( 7, 26 );
  SetColumnWidth( 8, 26 );
  SetColumnWidth( 9, 26 );

  m_popup = new wxMenu( _T("") );
  m_popup->Append( BLIST_DLMAP, _("Download &map") );
  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}


BattleListCtrl::~BattleListCtrl()
{
  delete m_popup;
}


void BattleListCtrl::OnSelected( wxListEvent& event )
{
  m_selected = GetItemData( event.GetIndex() );
  event.Skip();
}


void BattleListCtrl::OnDeselected( wxListEvent& event )
{
  if ( m_selected == GetItemData( event.GetIndex() )  )
  m_selected = -1;
}


int BattleListCtrl::GetSelectedIndex()
{
  return m_selected;
}


void BattleListCtrl::OnListRightClick( wxListEvent& event )
{
  PopupMenu( m_popup );
}


void BattleListCtrl::OnDLMap( wxCommandEvent& event )
{
  if ( m_selected != -1 ) {
    if ( m_ui.GetServer().battles_iter->BattleExists(m_selected) ) {
      m_ui.DownloadMap( m_ui.GetServer().battles_iter->GetBattle(m_selected).GetMapName() );
    }
  }
}


void BattleListCtrl::OnDLMod( wxCommandEvent& event )
{
  if ( m_selected != -1 ) {
    if ( m_ui.GetServer().battles_iter->BattleExists(m_selected) ) {
      m_ui.DownloadMod( m_ui.GetServer().battles_iter->GetBattle(m_selected).GetModName() );
    }
  }
}


void BattleListCtrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( -1 );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() and i < 4; ++i ) {}
  if ( i > 3 ) { i = 3; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}


void BattleListCtrl::Sort()
{
  BattleListCtrl::m_ui_for_sort = &m_ui;
  if (!m_ui_for_sort || !m_ui_for_sort->GetServerStatus()  ) return;
  for (int i = 3; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&CompareStatusUP:&CompareStatusDOWN , 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareCountryUP:&CompareCountryDOWN , 0 ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&CompareRankUP:&CompareRankDOWN , 0 ); break;
      case 3 : SortItems( ( m_sortorder[ i ].direction )?&CompareDescriptionUP:&CompareDescriptionDOWN , 0 ); break;
      case 4 : SortItems( ( m_sortorder[ i ].direction )?&CompareMapUP:&CompareMapDOWN , 0 ); break;
      case 5 : SortItems( ( m_sortorder[ i ].direction )?&CompareModUP:&CompareModDOWN , 0 ); break;
      case 6 : SortItems( ( m_sortorder[ i ].direction )?&CompareHostUP:&CompareHostDOWN , 0 ); break;
      case 7 : SortItems( ( m_sortorder[ i ].direction )?&CompareSpectatorsUP:&CompareSpectatorsDOWN , 0 ); break;
      case 8 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayerUP:&ComparePlayerDOWN , 0 ); break;
      case 9 : SortItems( ( m_sortorder[ i ].direction )?&CompareMaxPlayerUP:&CompareMaxPlayerDOWN , 0 ); break;
    }
  }
}


int wxCALLBACK BattleListCtrl::CompareStatusUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  int b1 = 0, b2 = 0;

  if ( battle1.GetInGame() )
    b1 += 1000;
  if ( battle1.GetInGame() )
    b2 += 1000;
  if ( battle1.IsLocked() )
    b1 += 100;
  if ( battle2.IsLocked() )
    b2 += 100;
  if ( battle1.IsPassworded() )
    b1 += 50;
  if ( battle2.IsPassworded() )
    b2 += 50;

  // inverse the order
  if ( b1 < b2 )
      return -1;
  if ( b1 > b2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareStatusDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  int b1 = 0, b2 = 0;

  if ( battle1.GetInGame() )
    b1 += 1000;
  if ( battle1.GetInGame() )
    b2 += 1000;
  if ( battle1.IsLocked() )
    b1 += 100;
  if ( battle2.IsLocked() )
    b2 += 100;
  if ( battle1.IsPassworded() )
    b1 += 50;
  if ( battle2.IsPassworded() )
    b2 += 50;

  // inverse the order
  if ( b1 < b2 )
      return 1;
  if ( b1 > b2 )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareRankUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
      return -1;
  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareRankDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
      return 1;
  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareCountryUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() < WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return -1;
  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() > WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return 1;

  return 0;
}



int wxCALLBACK BattleListCtrl::CompareCountryDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() < WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return 1;
  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() > WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareDescriptionUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetDescription()).MakeUpper() < WX_STRING(battle2.GetDescription()).MakeUpper() )
      return -1;
  if ( WX_STRING(battle1.GetDescription()).MakeUpper() > WX_STRING(battle2.GetDescription()).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareDescriptionDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( (WX_STRING(battle1.GetDescription()).MakeUpper()) < (WX_STRING(battle2.GetDescription()).MakeUpper()) )
   { wxMessageBox(_("bigger"));
      return 1;
   }
  if ( (WX_STRING(battle1.GetDescription()).MakeUpper()) > (WX_STRING(battle2.GetDescription()).MakeUpper()) )
    {
      wxMessageBox(_("smaller"));
      return -1;
    }

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareMapUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() < RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return -1;
  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() > RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareMapDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() < RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return 1;
  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() > RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareModUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( RefineModname( battle1.GetModName() ).MakeUpper() < RefineModname( battle2.GetModName() ).MakeUpper() )
      return -1;
  if ( RefineModname( battle1.GetModName() ).MakeUpper() > RefineModname( battle2.GetModName() ).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareModDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( RefineModname( battle1.GetModName() ).MakeUpper() < RefineModname( battle2.GetModName() ).MakeUpper() )
      return 1;
  if ( RefineModname( battle1.GetModName() ).MakeUpper() > RefineModname( battle2.GetModName() ).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareHostUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() < WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return -1;
  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() > WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareHostDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() < WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return 1;
  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() > WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareSpectatorsUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetSpectators() < battle2.GetSpectators() )
      return -1;
  if ( battle1.GetSpectators() > battle2.GetSpectators() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareSpectatorsDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetSpectators() < battle2.GetSpectators() )
      return 1;
  if ( battle1.GetSpectators() > battle2.GetSpectators() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::ComparePlayerUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetNumUsers() - battle1.GetSpectators() < battle2.GetNumUsers() - battle2.GetSpectators() )
      return -1;
  if ( battle1.GetNumUsers() - battle1.GetSpectators() > battle2.GetNumUsers() - battle2.GetSpectators() )
      return 1;

  return 0;
}



int wxCALLBACK BattleListCtrl::ComparePlayerDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetNumUsers() - battle1.GetSpectators() < battle2.GetNumUsers() - battle2.GetSpectators() )
      return 1;
  if ( battle1.GetNumUsers() - battle1.GetSpectators() > battle2.GetNumUsers() - battle2.GetSpectators() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareMaxPlayerUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetMaxPlayers() < battle2.GetMaxPlayers() )
      return -1;
  if ( battle1.GetMaxPlayers() > battle2.GetMaxPlayers() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareMaxPlayerDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item2);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetMaxPlayers() < battle2.GetMaxPlayers() )
      return 1;
  if ( battle1.GetMaxPlayers() > battle2.GetMaxPlayers() )
      return -1;

  return 0;
}



