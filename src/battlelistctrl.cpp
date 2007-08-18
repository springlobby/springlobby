/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleListCtrl
//

#include "battlelistctrl.h"
#include "utils.h"
#include "user.h"
#include "iconimagelist.h"
#include "battle.h"
#include "uiutils.h"
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>


BEGIN_EVENT_TABLE(BattleListCtrl, wxListCtrl)

  EVT_LIST_ITEM_SELECTED   ( BLIST_LIST, BattleListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( BLIST_LIST, BattleListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( BLIST_LIST, BattleListCtrl::OnDeselected )
  EVT_LIST_ITEM_RIGHT_CLICK( BLIST_LIST, BattleListCtrl::OnListRightClick )
  EVT_MENU                 ( BLIST_DLMAP, BattleListCtrl::OnDLMap )
  EVT_MENU                 ( BLIST_DLMOD, BattleListCtrl::OnDLMod )

END_EVENT_TABLE()


BattleListCtrl::BattleListCtrl( wxWindow* parent ) : wxListCtrl(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL), m_selected(-1)
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
  col.SetImage( -1 );
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


void BattleListCtrl::UpdateList()
{
  for ( int i = 0; i < GetItemCount(); i++ ) {
    UpdateBattle( i );
  }
}


void BattleListCtrl::AddBattle( Battle& battle )
{
  int index = InsertItem( 0, IconImageList::GetBattleStatusIcon( battle ) );
  assert( index != -1 );
  SetItemData(index, (wxUIntPtr)&battle );

  UpdateBattle( index );
}


void BattleListCtrl::RemoveBattle( Battle& battle )
{
  int index = GetBattleIndex( battle );
  DeleteItem( index );
}


bool BattleListCtrl::BattleExists( Battle& battle )
{
  return false;
}


void BattleListCtrl::UpdateBattle( Battle& battle )
{
  UpdateBattle( GetBattleIndex( battle ) );
}


void BattleListCtrl::UpdateBattle( const int& index )
{
  assert( index != -1 );

  wxListItem item;
  item.SetId( index );

  if (!GetItem( item )) assert(false);

  Battle& battle = *((Battle*)GetItemData( index ));

  SetItemImage( index, IconImageList::GetBattleStatusIcon( battle ) );
  SetItemColumnImage( index, 2, IconImageList::GetRankIcon( battle.opts().rankneeded, false ) );
  SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( battle.GetFounder().GetCountry() ) );
  SetItem( index, 3, WX_STRING(battle.opts().description) );
  SetItem( index, 4, RefineMapname( WX_STRING(battle.opts().mapname) ), battle.IsMapAvailable()?ICON_EXISTS:ICON_NEXISTS );
  SetItem( index, 5, RefineModname( WX_STRING(battle.opts().modname) ), battle.IsModAvailable()?ICON_EXISTS:ICON_NEXISTS );
  SetItem( index, 6, WX_STRING(battle.opts().founder) );
  SetItem( index, 7, wxString::Format(_T("%d"), battle.opts().spectators) );
  SetItem( index, 8, wxString::Format(_T("%d"), battle.GetNumUsers() - battle.opts().spectators ) );
  SetItem( index, 9, wxString::Format(_T("%d"), battle.opts().maxplayers) );

}


int BattleListCtrl::GetBattleIndex( Battle& battle )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    if ( (unsigned long)&battle == GetItemData( i ) ) return i;
  }
  debug_error( "didn't find the battle." );
  return -1;
}


void BattleListCtrl::OnSelected( wxListEvent& event )
{
  m_selected = event.GetIndex();
  event.Skip();
}


void BattleListCtrl::OnDeselected( wxListEvent& event )
{
  if ( m_selected == event.GetIndex() )
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
  if ( m_selected >= 0 ) {
    Battle& battle = *((Battle*)GetItemData( m_selected )); // FIXME m_selected might not exist.
    wxString map = WX_STRING(battle.opts().mapname);
    map = map.SubString(0, map.Find( '.', true ) - 1 );
    map.Replace(_T(" "), _T("%20") );
    wxString url = _T("http://spring.unknown-files.net/page/search/1/13/") + map + _T("/");
    if ( !wxLaunchDefaultBrowser( url ) ) {
      wxMessageBox( _T("Couldn't launch browser. URL is: ") + url, _T("Couldn't launch browser.")  );
    }
  }
}


void BattleListCtrl::OnDLMod( wxCommandEvent& event )
{
  if ( m_selected >= 0 ) {
    Battle& battle = *((Battle*)GetItemData( m_selected )); // FIXME m_selected might not exist.
    wxString mod = WX_STRING(battle.opts().modname);
    mod.Replace(_T(" "), _T("%20") );
    wxString url = _T("http://spring.unknown-files.net/page/search/1/14/") + mod + _T("/");
    wxLaunchDefaultBrowser( url );
  }
}

