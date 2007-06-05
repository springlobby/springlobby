//
// Class: BattleroomListCtrl
//

#include "battleroomlistctrl.h"
#include "iconimagelist.h"

BattleroomListCtrl::BattleroomListCtrl( wxWindow* parent ) : wxListCtrl(parent, -1, 
  wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL )
{
  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );
  
  wxListItem col;
  
  col.SetText( _("r") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 1, col );
  
  col.SetText( _("c") );
  col.SetImage( -1 );
  InsertColumn( 2, col );

  col.SetText( _("f") );
  col.SetImage( -1 );
  InsertColumn( 3, col );
  
  col.SetText( _("r") );
  col.SetImage( -1 );
  InsertColumn( 4, col );
  
  col.SetText( _T("Nickname") );
  col.SetImage( -1 );
  InsertColumn( 5, col );
  
  col.SetText( _T("t") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _T("a") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _T("cpu") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  col.SetText( _T("Handicap") );
  col.SetImage( -1 );
  InsertColumn( 9, col );

  SetColumnWidth( 0, 20 );
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


void BattleroomListCtrl::AddUser( User& user )
{
  int index = InsertItem( 0, ICON_NREADY );
  assert( index != -1 );
  SetItemData(index, (long)&user );

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
    
  User& user = *((User*)item.GetData());
  
  SetItemImage( index, IconImageList::GetReadyIcon( user.GetBattleStatus().ready ) );
  SetItemColumnImage( index, 1, IconImageList::GetSideIcon( user.GetBattleStatus().side ) );
  SetItemColumnImage( index, 2, IconImageList::GetColorIcon( user.GetBattleStatus().color_index ) );
  SetItemColumnImage( index, 3, IconImageList::GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 4, IconImageList::GetRankIcon( user.GetStatus().rank ) );
  SetItem( index, 5, WX_STRING( user.GetNick() ) );
  SetItem( index, 6, wxString::Format( _("%d"), user.GetBattleStatus().team ) );
  SetItem( index, 7, wxString::Format( _("%d"), user.GetBattleStatus().ally ) );
  SetItem( index, 8, wxString::Format( _("%.1f GHz"), user.GetCpu() / 1000.0 ) );
  SetItem( index, 9, wxString::Format( _("%d%%"), user.GetBattleStatus().handicap ) );
}


int BattleroomListCtrl::GetUserIndex( User& user )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    if ( (long)&user == GetItemData( i ) ) return i;
  }
  std::cout << "!! BattleroomListCtrl::GetUserIndex( ): didn't find the battle." << std::endl;
}

