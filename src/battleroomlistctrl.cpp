//
// Class: BattleroomListCtrl
//

#include "battleroomlistctrl.h"
#include "iconimagelist.h"

BattleroomListCtrl::BattleroomListCtrl( wxWindow* parent ) : wxListCtrl(parent, -1, wxDefaultPosition, wxDefaultSize,
             wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL)
{
  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );
  
  wxListItem col;
  
  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _("c") );
  col.SetImage( -1 );
  InsertColumn( 1, col );

  col.SetText( _("r") );
  col.SetImage( -1 );
  InsertColumn( 2, col );
  
  col.SetText( _T("Description") );
  col.SetImage( -1 );
  InsertColumn( 3, col );
  
  col.SetText( _T("Map") );
  col.SetImage( -1 );
  InsertColumn( 4, col );

  col.SetText( _T("Mod") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _T("Host") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _T("s") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _T("p") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  col.SetText( _T("m") );
  col.SetImage( -1 );
  InsertColumn( 9, col );

  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 170 );
  SetColumnWidth( 4, 140 );
  SetColumnWidth( 5, 140 );
  SetColumnWidth( 6, 120 );
  SetColumnWidth( 7, 26 );
  SetColumnWidth( 8, 26 );
  SetColumnWidth( 9, 26 );
}


BattleroomListCtrl::~BattleroomListCtrl()
{
  
}


