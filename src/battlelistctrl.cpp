//
// Class: BattleListCtrl
//

#include "battlelistctrl.h"
#include "utils.h"

#include "images/open_game.xpm"
#include "images/open_pw_game.xpm"
#include "images/closed_game.xpm"
#include "images/closed_pw_game.xpm"
#include "images/started_game.xpm"

static wxImageList* _imagelist = NULL;
static int _imagelist_users = 0;

BattleListCtrl::BattleListCtrl( wxWindow* parent ) : wxListCtrl(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL)
{
  
  if ( _imagelist == NULL ) {
    _imagelist = new wxImageList( 16, 16 );
    _imagelist->Add( wxBITMAP(open_game) );
    _imagelist->Add( wxBITMAP(open_pw_game) );
    _imagelist->Add( wxBITMAP(closed_game) );
    _imagelist->Add( wxBITMAP(closed_pw_game) );
    _imagelist->Add( wxBITMAP(started_game) );
  }
  _imagelist_users++;
  
  SetImageList( _imagelist, wxIMAGE_LIST_NORMAL );
  SetImageList( _imagelist, wxIMAGE_LIST_SMALL );
  SetImageList( _imagelist, wxIMAGE_LIST_STATE );
  
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
  SetColumnWidth( 2, 140 );
  SetColumnWidth( 3, 180 );
  SetColumnWidth( 4, 100 );
  SetColumnWidth( 5, 26 );
  SetColumnWidth( 6, 26 );
  SetColumnWidth( 7, 26 );
}


BattleListCtrl::~BattleListCtrl()
{
  _imagelist_users--;
  if ( _imagelist_users == 0 ) {
    delete _imagelist;
    _imagelist = NULL;
  }
}


void BattleListCtrl::AddBattle( Battle& battle )
{
  int index = InsertItem( 0, GetStatusIcon( battle ) );
  assert( index != -1 );
  SetItem( index, 1, WX_STRING(battle.opts().description) );
  SetItem( index, 2, WX_STRING(battle.opts().mapname) );
  SetItem( index, 3, WX_STRING(battle.opts().modname) );
  SetItem( index, 4, WX_STRING(battle.opts().founder) );
  SetItem( index, 5, wxString::Format(_("%d"), battle.opts().spectators) );
  SetItem( index, 6, _("?") );
  SetItem( index, 7, wxString::Format(_("%d"), battle.opts().maxplayers) );

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
  if ( !battle.opts().locked ) {
    if ( !battle.opts().ispassworded ) return ICON_OPEN_GAME;
    else return ICON_OPEN_PW_GAME;
  } else {
    if ( !battle.opts().ispassworded ) return ICON_CLOSED_GAME;
    else return ICON_CLOSED_PW_GAME;
  }
//ICON_STARTED_GAME
  
  return ICON_GAME_UNKNOWN;
}

