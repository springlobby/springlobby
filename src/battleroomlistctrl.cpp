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

  col.SetText( _("r") );
  col.SetImage( -1 );
  InsertColumn( 1, col );

  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 2, col );

  col.SetText( _("c") );
  col.SetImage( -1 );
  InsertColumn( 3, col );
  
  col.SetText( _T("Nickname") );
  col.SetImage( -1 );
  InsertColumn( 4, col );
  
  col.SetText( _T("t") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _T("a") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _T("cpu") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _T("Handicap") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 20 );
  SetColumnWidth( 4, 160 );
  SetColumnWidth( 5, 26 );
  SetColumnWidth( 6, 26 );
  SetColumnWidth( 7, 60 );
  SetColumnWidth( 8, 40 );
}


BattleroomListCtrl::~BattleroomListCtrl()
{
  
}


