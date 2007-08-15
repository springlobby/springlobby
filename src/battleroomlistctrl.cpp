/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleroomListCtrl
//

#include <wx/intl.h>

#include "battleroomlistctrl.h"
#include "iconimagelist.h"
#include "iunitsync.h"
#include "battle.h"
#include "user.h"
#include "utils.h"

BattleroomListCtrl::BattleroomListCtrl( wxWindow* parent, Battle& battle ) : wxListCtrl(parent, -1,
  wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL ), m_battle(battle)
{
  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );

  wxListItem col;

  col.SetText( _T("r") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _T("s") );
  col.SetImage( -1 );
  InsertColumn( 1, col );

  col.SetText( _T("c") );
  col.SetImage( -1 );
  InsertColumn( 2, col );

  col.SetText( _T("f") );
  col.SetImage( -1 );
  InsertColumn( 3, col );

  col.SetText( _T("r") );
  col.SetImage( -1 );
  InsertColumn( 4, col );

  col.SetText( _("Nickname") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _("t") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _("a") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _("cpu") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  col.SetText( _("Handicap") );
  col.SetImage( -1 );
  InsertColumn( 9, col );

#ifdef __WXMSW__
  SetColumnWidth( 0, 45 );
#else
  SetColumnWidth( 0, 20 );
#endif
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 20 );
  SetColumnWidth( 4, 20 );
  SetColumnWidth( 5, 170 );
  SetColumnWidth( 6, 26 );
  SetColumnWidth( 7, 26 );
  SetColumnWidth( 8, 80 );
  SetColumnWidth( 9, 60 );
}


BattleroomListCtrl::~BattleroomListCtrl()
{

}


void BattleroomListCtrl::UpdateList()
{

}


void BattleroomListCtrl::AddUser( User& user )
{
  int index = InsertItem( 0, ICON_NREADY );
  assert( index != -1 );
  SetItemData(index, (wxUIntPtr)&user );

  UpdateUser( index );
}


void BattleroomListCtrl::RemoveUser( User& user )
{
  DeleteItem( GetUserIndex( user ) );
}


void BattleroomListCtrl::UpdateUser( User& user )
{
  UpdateUser( GetUserIndex( user ) );
}


void BattleroomListCtrl::UpdateUser( const int& index )
{
  assert( index != -1 );

  wxListItem item;
  item.SetId( index );

  if (!GetItem( item )) assert(false);

  User& user = *((User*)GetItemData( index ));

  icons().SetColourIcon( user.BattleStatus().team, wxColour( user.BattleStatus().color_r, user.BattleStatus().color_g, user.BattleStatus().color_b ) );

  SetItemImage( index, (user.BattleStatus().spectator)?ICON_SPECTATOR:IconImageList::GetReadyIcon( user.BattleStatus().ready, user.BattleStatus().sync ) );

  SetItemColumnImage( index, 1, -1 );

  if ( !user.BattleStatus().spectator ) {

    try {
      int sideimg = icons().GetSideIcon( usync()->GetSideName( m_battle.opts().modname, user.BattleStatus().side ) );
      if ( sideimg >= 0 ) SetItemColumnImage( index, 1, sideimg );
      else SetItem( index, 1, WX_STRING(usync()->GetSideName( m_battle.opts().modname, user.BattleStatus().side )) );
    } catch ( ... ) {
      SetItem( index, 1, wxString::Format( _T("s%d"), user.BattleStatus().side + 1 ) );
    }

    SetItemColumnImage( index, 2, IconImageList::GetColourIcon( user.BattleStatus().team ) );

  } else {
    SetItemColumnImage( index, 2, -1 );
  }

  SetItemColumnImage( index, 3, IconImageList::GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 4, IconImageList::GetRankIcon( user.GetStatus().rank ) );
  SetItem( index, 5, WX_STRING( user.GetNick() ) );
  if ( !user.BattleStatus().spectator ) {
    SetItem( index, 6, wxString::Format( _T("%d"), user.BattleStatus().team + 1 ) );
    SetItem( index, 7, wxString::Format( _T("%d"), user.BattleStatus().ally + 1 ) );
    SetItem( index, 9, wxString::Format( _T("%d%%"), user.BattleStatus().handicap ) );
  } else {
    SetItem( index, 6, _T("") );
    SetItem( index, 7, _T("") );
    SetItem( index, 9, _T("") );
  }
  SetItem( index, 8, wxString::Format( _T("%.1f GHz"), user.GetCpu() / 1000.0 ) );
}


int BattleroomListCtrl::GetUserIndex( User& user )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    wxListItem item;
    item.SetId( i );
    GetItem( item );
    if ( item.GetImage() == ICON_BOT ) continue;
    if ( (unsigned long)&user == GetItemData( i ) ) return i;
  }
  debug_error( "didn't find the battle." );
  return -1;
}


void BattleroomListCtrl::AddBot( BattleBot& bot )
{
  int index = InsertItem( 0, ICON_BOT );
  assert( index != -1 );
  SetItemData(index, (wxUIntPtr)&bot );

  UpdateBot( index );
}


void BattleroomListCtrl::RemoveBot( BattleBot& bot )
{
  DeleteItem( GetBotIndex( bot ) );
}


void BattleroomListCtrl::UpdateBot( BattleBot& bot )
{
  UpdateBot( GetBotIndex( bot ) );
}


void BattleroomListCtrl::UpdateBot( const int& index )
{
  assert( index != -1 );

  wxListItem item;
  item.SetId( index );

  if (!GetItem( item )) assert(false);

  BattleBot& bot = *((BattleBot*)GetItemData( index ));

  icons().SetColourIcon( bot.bs.team, wxColour( bot.bs.color_r, bot.bs.color_g, bot.bs.color_b ) );

  SetItemImage( index, ICON_BOT );

  SetItemColumnImage( index, 1, -1 );

  try {
    int sideimg = icons().GetSideIcon( usync()->GetSideName( m_battle.opts().modname, bot.bs.side ) );
    if ( sideimg >= 0 ) SetItemColumnImage( index, 1, sideimg );
    else SetItem( index, 1, WX_STRING(usync()->GetSideName( m_battle.opts().modname, bot.bs.side )) );
  } catch ( ... ) {
    SetItem( index, 1, wxString::Format( _T("s%d"), bot.bs.side + 1 ) );
  }

  SetItemColumnImage( index, 2, IconImageList::GetColourIcon( bot.bs.team ) );

  SetItemColumnImage( index, 3, ICON_NONE );
  SetItemColumnImage( index, 4, ICON_NONE );
  SetItem( index, 5, WX_STRING( bot.name ) );

  SetItem( index, 6, wxString::Format( _T("%d"), bot.bs.team + 1 ) );
  SetItem( index, 7, wxString::Format( _T("%d"), bot.bs.ally + 1 ) );
  SetItem( index, 9, wxString::Format( _T("%d%%"), bot.bs.handicap ) );

  SetItem( index, 8, WX_STRING( bot.aidll ) );
}


int BattleroomListCtrl::GetBotIndex( BattleBot& bot )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    wxListItem item;
    item.SetId( i );
    GetItem( item );
    if ( item.GetImage() != ICON_BOT ) continue;
    if ( (unsigned long)&bot == GetItemData( i ) ) return i;
  }
  debug_error( "didn't find the bot." );
  return -1;
}




