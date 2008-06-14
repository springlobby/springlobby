/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/string.h>

#include "filelistctrl.h"
#include "../utils.h"
#include "../iconimagelist.h"
#include "../uiutils.h"
//#include "countrycodes.h"

#define TOOLTIP_DELAY 1000

BEGIN_EVENT_TABLE( FileListCtrl, customListCtrl )

	EVT_LIST_ITEM_RIGHT_CLICK( BLIST_LIST, FileListCtrl::OnListRightClick )
	EVT_LIST_COL_CLICK( BLIST_LIST, FileListCtrl::OnColClick )
	#if wxUSE_TIPWINDOW
	#if !defined(__WXMSW__) /* && !defined(__WXMAC__) */ //disables tooltips on msw /* and mac */
	EVT_MOTION( FileListCtrl::OnMouseMotion )
	#endif
	#endif
END_EVENT_TABLE()

//Ui* FileListCtrl::m_ui_for_sort = 0;

FileListCtrl::FileListCtrl( wxWindow* parent ):
		customListCtrl( parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT )
{

	SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
	SetImageList( &icons(), wxIMAGE_LIST_SMALL );
	SetImageList( &icons(), wxIMAGE_LIST_STATE );


	wxListItem col;

	col.SetText( _( "Name" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 0, col, _( "Name" ) );

	col.SetText( _( "Type" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 1, col, _( "Type" ) );

	col.SetText( _( "Hash" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 2, col, _( "Hash" ) );

	m_sortorder[0].col = 0;
	m_sortorder[0].direction = true;
	m_sortorder[1].col = 1;
	m_sortorder[1].direction = true;
	m_sortorder[2].col = 2;
	m_sortorder[2].direction = true;

	Sort( );

//  m_popup = new wxMenu( _T("") );
//  // &m enables shortcout "alt + m" and underlines m
//  m_popup->Append( BLIST_DLMAP, _("Download &map") );
//  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}

void FileListCtrl::SetColumnWidths()
{
#if defined(__WXMAC__)
/// on mac, autosize does not work at all
	SetColumnWidth( 0, 250 );
	SetColumnWidth( 1, 60 );
	SetColumnWidth( 2, 150 );

#else
	SetColumnWidth( 0, wxLIST_AUTOSIZE );
	SetColumnWidth( 1, wxLIST_AUTOSIZE );
	SetColumnWidth( 2, wxLIST_AUTOSIZE );
#endif
}

FileListCtrl::~FileListCtrl()
{
	//delete m_popup;
}


void FileListCtrl::OnListRightClick( wxListEvent& event )
{
	PopupMenu( m_popup );
}

void FileListCtrl::OnColClick( wxListEvent& event )
{
	if ( event.GetColumn() == -1 ) return;
	wxListItem col;
	GetColumn( m_sortorder[0].col, col );
	col.SetImage( icons().ICON_NONE );
	SetColumn( m_sortorder[0].col, col );

	int i;
	for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 3; ++i ) {}
	if ( i > 2 ) { i = 2; }
	for ( ; i > 0; i-- ) { m_sortorder[i] = m_sortorder[i-1]; }
	m_sortorder[0].col = event.GetColumn();
	m_sortorder[0].direction = !m_sortorder[0].direction;


	GetColumn( m_sortorder[0].col, col );
	//col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
	SetColumn( m_sortorder[0].col, col );

	Sort();
}

void FileListCtrl::GetSelectedHashes(HashVector& hashes)
{
	long index = -1;
	for ( long i = 0; i < GetSelectedItemCount(); ++i )
	{
		index = GetNextItem( index, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
		hashes.push_back( GetItemText( index ) );
	}
}


void FileListCtrl::Sort()
{
	bool changed = false;
//  FileListCtrl::m_ui_for_sort = &m_ui;
//  if (!m_ui_for_sort || !m_ui_for_sort->GetServerStatus()  ) return;
	for ( int i = 3; i >= 0; i-- )
	{
		switch ( m_sortorder[ i ].col )
		{
			case 0 :
				changed = SortItems(( m_sortorder[ i ].direction )?&CompareNameUP:&CompareNameDOWN , 0 );
				break;
			case 1 :
				changed = SortItems(( m_sortorder[ i ].direction )?&CompareCountryUP:&CompareCountryDOWN , 0 );
				break;
			case 2 :
				changed = SortItems(( m_sortorder[ i ].direction )?&CompareRankUP:&CompareRankDOWN , 0 );
				break;
		}
	}
}


int wxCALLBACK FileListCtrl::CompareNameUP( long item1, long item2, long sortData )
{
//  Ui* ui = m_ui_for_sort;
//  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
//  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);
//
//  int b1 = 0, b2 = 0;
//
//  if ( battle1.GetInGame() )
//    b1 += 1000;
//  if ( battle2.GetInGame() )
//    b2 += 1000;
//  if ( battle1.IsLocked() )
//    b1 += 100;
//  if ( battle2.IsLocked() )
//    b2 += 100;
//  if ( battle1.IsPassworded() )
//    b1 += 50;
//  if ( battle2.IsPassworded() )
//    b2 += 50;
//  if ( battle1.IsFull() )
//    b1 += 25;
//  if ( battle2.IsFull() )
//    b2 += 25;
//
//  if ( b1 > 1000 ) b1 = 1000;
//  if ( b2 > 1000 ) b2 = 1000;
//
//  // inverse the order
//  if ( b1 < b2 )
//      return -1;
//  if ( b1 > b2 )
//      return 1;

	return 0;
}


int wxCALLBACK FileListCtrl::CompareNameDOWN( long item1, long item2, long sortData )
{
//  Ui* ui = m_ui_for_sort;
//  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
//  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);
//
//  int b1 = 0, b2 = 0;
//
//  if ( battle1.GetInGame() )
//    b1 += 1000;
//  if ( battle2.GetInGame() )
//    b2 += 1000;
//  if ( battle1.IsLocked() )
//    b1 += 100;
//  if ( battle2.IsLocked() )
//    b2 += 100;
//  if ( battle1.IsPassworded() )
//    b1 += 50;
//  if ( battle2.IsPassworded() )
//    b2 += 50;
//  if ( battle1.IsFull() )
//    b1 += 25;
//  if ( battle2.IsFull() )
//    b2 += 25;
//
//  if ( b1 > 1000 ) b1 = 1000;
//  if ( b2 > 1000 ) b2 = 1000;
//
//  // inverse the order
//  if ( b1 < b2 )
//      return 1;
//  if ( b1 > b2 )
//      return -1;

	return 0;
}


int wxCALLBACK FileListCtrl::CompareRankUP( long item1, long item2, long sortData )
{
//  Ui* ui = m_ui_for_sort;
//  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
//  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);
//
//  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
//      return -1;
//  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
//      return 1;
//
	return 0;
}


int wxCALLBACK FileListCtrl::CompareRankDOWN( long item1, long item2, long sortData )
{
//  Ui* ui = m_ui_for_sort;
//  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
//  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);
//
//  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
//      return 1;
//  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
//      return -1;
//
	return 0;
}


int wxCALLBACK FileListCtrl::CompareCountryUP( long item1, long item2, long sortData )
{
//  Ui* ui = m_ui_for_sort;
//  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
//  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);
//
//  if ( battle1.GetFounder().GetCountry().MakeUpper() < battle2.GetFounder().GetCountry().MakeUpper() )
//      return -1;
//  if ( battle1.GetFounder().GetCountry().MakeUpper() > battle2.GetFounder().GetCountry().MakeUpper() )
//      return 1;
//
	return 0;
}



int wxCALLBACK FileListCtrl::CompareCountryDOWN( long item1, long item2, long sortData )
{
//  Ui* ui = m_ui_for_sort;
//  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
//  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);
//
//  if ( battle1.GetFounder().GetCountry().MakeUpper() < battle2.GetFounder().GetCountry().MakeUpper() )
//      return 1;
//  if ( battle1.GetFounder().GetCountry().MakeUpper() > battle2.GetFounder().GetCountry().MakeUpper() )
//      return -1;
//
	return 0;
}

void FileListCtrl::OnMouseMotion( wxMouseEvent& event )
{
#if wxUSE_TIPWINDOW
	wxPoint position = event.GetPosition();

	try
	{
		tipTimer.Start( TOOLTIP_DELAY, wxTIMER_ONE_SHOT );
		int flag = wxLIST_HITTEST_ONITEM;
		long subItem;
#ifdef HAVE_WX28
		long item_hit = HitTest( position, flag, &subItem );
#else
		long item_hit = HitTest( position, flag );
#endif

		if ( item_hit != wxNOT_FOUND && item_hit>=0 && item_hit<GetItemCount() )
		{
			long item = GetItemData( item_hit );
//			Ui* ui = m_ui_for_sort;
//			Battle& battle = ui->GetServer().battles_iter->GetBattle(item);
			int coloumn = getColoumnFromPosition( position );
			switch ( coloumn )
			{
//			case 0: // status
//			m_tiptext = icons().GetBattleStatus(battle);
//				break;
//			case 1: // country
//				m_tiptext = GetFlagNameFromCountryCode(battle.GetFounder().GetCountry());
//				break;
//			case 2: // rank_min
//				m_tiptext = m_colinfovec[coloumn].first;
//				break;


				default:
					m_tiptext = _T( "" );
					break;
			}
		}
	}
	catch ( ... ) {}
#endif
}


