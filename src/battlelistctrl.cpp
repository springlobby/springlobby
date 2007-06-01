//
// Class: BattleListCtrl
//

#include "battlelistctrl.h"
#include "utils.h"

BattleListCtrl::BattleListCtrl( wxWindow* parent ) : wxListCtrl(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL)
{
  wxListItem col;
  
  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _T("Description") );
  col.SetImage( -1 );
  InsertColumn( 1, col );
  
  col.SetText( _T("Map") );
  col.SetImage( -1 );
  InsertColumn( 2, col );

  col.SetText( _T("Mod") );
  col.SetImage( -1 );
  InsertColumn( 3, col );

  col.SetText( _T("Host") );
  col.SetImage( -1 );
  InsertColumn( 4, col );

  col.SetText( _T("s") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _T("p") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _T("m") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 170 );
  SetColumnWidth( 2, 100 );
  SetColumnWidth( 3, 100 );
  SetColumnWidth( 4, 100 );
  SetColumnWidth( 5, 20 );
  SetColumnWidth( 6, 20 );
  SetColumnWidth( 7, 20 );
}


BattleListCtrl::~BattleListCtrl()
{
  
}


void BattleListCtrl::AddBattle( Battle& battle )
{
  int index = InsertItem( 0, GetStatusIcon( battle ) );
  assert( index != -1 );
  SetItem( index, 1, WX_STRING(battle.opts().description) );
  SetItem( index, 2, WX_STRING(battle.opts().mapname) );
  SetItem( index, 3, WX_STRING(battle.opts().modname) );
  SetItem( index, 4, WX_STRING(battle.opts().founder) );
  SetItem( index, 5, _("0") );
  SetItem( index, 6, _("1") );
  SetItem( index, 7, _("10") );

  SetItemData(index, (long)&battle );
  //SortItems( NickListSortCallback, 0 );
}


void BattleListCtrl::RemoveBattle( Battle& battle )
{

}


bool BattleListCtrl::BattleExists( Battle& battle )
{
  return false;
}

  
int BattleListCtrl::GetStatusIcon( Battle& battle )
{
  return -1;
}

