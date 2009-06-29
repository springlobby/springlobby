/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#ifndef NO_TORRENT_SYSTEM

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <wx/menu.h>

#include "torrentlistctrl.h"
#include "torrentwrapper.h"
//#include "utils/.h"
#include "iconimagelist.h"

BEGIN_EVENT_TABLE( TorrentListCtrl, CustomListCtrl )

	EVT_LIST_ITEM_RIGHT_CLICK( TLIST_CLICK, TorrentListCtrl::OnListRightClick )
	EVT_LIST_COL_CLICK( TLIST_CLICK, TorrentListCtrl::OnColClick )
	#if wxUSE_TIPWINDOW
	#ifndef __WXMSW__ //disables tooltips on win
	EVT_MOTION( TorrentListCtrl::OnMouseMotion )
	#endif
	#endif
END_EVENT_TABLE()

map_infos* TorrentListCtrl::m_info_map = 0;

TorrentListCtrl::TorrentListCtrl( wxWindow* parent, Ui& ui ):
		CustomListCtrl( parent, TLIST_CLICK, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("TorrentListCtrl"), 10 )

{
	wxListItem col;

	col.SetText( _( "Name" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 0, col, _T( "Name" ), true );

	col.SetText( _( "numcopies" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 1, col, _T( "complete numcopies" ), true );

	col.SetText( _( "MB downloaded" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 2, col, _T( "MB downloaded" ), true );

	col.SetText( _( "MB uploaded" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 3, col, _T( "MB uploaded" ) );

	col.SetText( _( "status" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 4, col, _T( "status" ) );

	col.SetText( _( "% complete" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 5, col, _T( "% complete" ) );

	col.SetText( _( "KB/s up" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 6, col, _T( "KB/s upload" ) );

	col.SetText( _( "KB/s down" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 7, col, _T( "KB/s download" ), true );

	col.SetText( _( "ETA (s)" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 8, col, _T( "Estimated time of arrival" ), true );

	col.SetText( _( "Filesize (MB)" ) );
	col.SetImage( icons().ICON_NONE );
	InsertColumn( 9, col, _T( "Filesize" ), true );

// sortorder: name --> percent completed --> mb donwloaded

	m_sortorder[0].col = 0;
	m_sortorder[0].direction = true;
	m_sortorder[1].col = 5;
	m_sortorder[1].direction = true;
	m_sortorder[2].col = 2;
	m_sortorder[2].direction = true;

	Sort( );

	//TODO this'll need fixing on win i assume [koshi]
	SetColumnWidth( 0, 250 );
	SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
	SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );
	SetColumnWidth( 7, 80 );
	SetColumnWidth( 8, wxLIST_AUTOSIZE_USEHEADER );
	SetColumnWidth( 9, wxLIST_AUTOSIZE_USEHEADER );


	SetColumnWidth( 3, 100 );
	SetColumnWidth( 4, 70 );
	SetColumnWidth( 5, 100 );
	SetColumnWidth( 6, 70 );

//  m_popup = new wxMenu( _T("") );
//  // &m enables shortcout "alt + m" and underlines m
//  m_popup->Append( BLIST_DLMAP, _("Download &map") );
//  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}
void TorrentListCtrl::SetInfoMap( map_infos* map )
{
	m_info_map = map;
}

TorrentListCtrl::~TorrentListCtrl()
{
//  delete m_popup;
}


void TorrentListCtrl::OnListRightClick( wxListEvent& event )
{
//  PopupMenu( m_popup );
}


void TorrentListCtrl::OnColClick( wxListEvent& event )
{
	if ( event.GetColumn() == -1 ) return;
	wxListItem col;
	GetColumn( m_sortorder[0].col, col );
	col.SetImage( icons().ICON_NONE );
	SetColumn( m_sortorder[0].col, col );

	int i;
	for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}
	if ( i > 3 ) { i = 3; }
	for ( ; i > 0; i-- ) { m_sortorder[i] = m_sortorder[i-1]; }
	m_sortorder[0].col = event.GetColumn();
	m_sortorder[0].direction = !m_sortorder[0].direction;


	GetColumn( m_sortorder[0].col, col );
//  col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
	SetColumn( m_sortorder[0].col, col );

	Sort();
}


void TorrentListCtrl::Sort()
{
	for ( int i = 3; i >= 0; i-- )
	{
		switch ( m_sortorder[ i ].col )
		{
			case 0 :
				SortItems(( m_sortorder[ i ].direction )?&CompareNameUP:&CompareNameDOWN , 0 );
				break;
			case 1 :
				SortItems(( m_sortorder[ i ].direction )?&CompareCopiesUP:&CompareCopiesDOWN , 0 );
				break;
			case 2 :
				SortItems(( m_sortorder[ i ].direction )?&CompareDownSizeUP:&CompareDownSizeDOWN , 0 );
				break;
			case 3 :
				SortItems(( m_sortorder[ i ].direction )?&CompareUpSizeUP:&CompareUpSizeDOWN , 0 );
				break;
			case 4 :
				SortItems(( m_sortorder[ i ].direction )?&CompareLeechUP:&CompareLeechDOWN , 0 );
				break;
			case 5 :
				SortItems(( m_sortorder[ i ].direction )?&CompareCompletedUP:&CompareCompletedDOWN , 0 );
				break;
			case 6 :
				SortItems(( m_sortorder[ i ].direction )?&CompareUpSpeedUP:&CompareUpSpeedDOWN , 0 );
				break;
			case 7 :
				SortItems(( m_sortorder[ i ].direction )?&CompareDownSpeedUP:&CompareDownSpeedDOWN , 0 );
				break;
			case 8 :
				SortItems(( m_sortorder[ i ].direction )?&CompareEtaUP:&CompareEtaDOWN , 0 );
				break;
            case 9 :
				SortItems(( m_sortorder[ i ].direction )?&CompareFileSizeUP:&CompareFileSizeDOWN , 0 );
				break;
		}
	}
}


int wxCALLBACK TorrentListCtrl::CompareNameUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.name < info2.name )
		return -1;
	if ( info1.name > info2.name )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareNameDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.name < info2.name )
		return -1;
	if ( info1.name > info2.name )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSizeUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.downloaded > info2.downloaded )
		return -1;
	if ( info1.downloaded < info2.downloaded )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSizeDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.downloaded < info2.downloaded )
		return 1;
	if ( info1.downloaded > info2.downloaded )
		return -1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareCopiesUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.numcopies < info2.numcopies )
		return -1;
	if ( info1.numcopies > info2.numcopies )
		return 1;

	return 0;
}



int wxCALLBACK TorrentListCtrl::CompareCopiesDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.numcopies < info2.numcopies )
		return 1;
	if ( info1.numcopies > info2.numcopies )
		return -1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSizeUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.uploaded < info2.uploaded )
		return -1;
	if ( info1.uploaded > info2.uploaded )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSizeDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.uploaded <  info2.uploaded )
	{
		return 1;
	}
	if ( info1.uploaded > info2.uploaded )
	{
		return -1;
	}

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareLeechUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.downloadstatus <  info2.downloadstatus )
		return -1;
	if ( info1.downloadstatus >  info2.downloadstatus )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareLeechDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.downloadstatus <  info2.downloadstatus )
		return 1;
	if ( info1.downloadstatus >  info2.downloadstatus )
		return -1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareCompletedUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.progress <  info2.progress )
		return -1;
	if ( info1.progress >  info2.progress )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareCompletedDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.progress <  info2.progress )
		return 1;
	if ( info1.progress >  info2.progress )
		return -1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSpeedUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.outspeed < info2.outspeed )
		return -1;
	if ( info1.outspeed > info2.outspeed )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareUpSpeedDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.outspeed < info2.outspeed )
		return 1;
	if ( info1.outspeed > info2.outspeed )
		return -1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSpeedUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.inspeed < info2.inspeed )
		return -1;
	if ( info1.inspeed > info2.inspeed )
		return 1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareDownSpeedDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.inspeed < info2.inspeed )
		return 1;
	if ( info1.inspeed > info2.inspeed )
		return -1;

	return 0;
}


int wxCALLBACK TorrentListCtrl::CompareEtaUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.eta < info2.eta )
		return -1;
	if ( info1.eta > info2.eta )
		return 1;

	return 0;
}



int wxCALLBACK TorrentListCtrl::CompareEtaDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.eta < info2.eta )
		return 1;
	if ( info1.eta > info2.eta )
		return -1;

	return 0;
}

int wxCALLBACK TorrentListCtrl::CompareFileSizeUP( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.filesize < info2.filesize )
		return -1;
	if ( info1.filesize > info2.filesize )
		return 1;

	return 0;
}

int wxCALLBACK TorrentListCtrl::CompareFileSizeDOWN( long item1, long item2, long sortData )
{
	map_infos info_map = *m_info_map;
	TorrentInfos& info1 = info_map[item1];
	TorrentInfos& info2 = info_map[item2];

	if ( info1.filesize < info2.filesize )
		return 1;
	if ( info1.filesize > info2.filesize )
		return -1;

	return 0;
}

void TorrentListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
    m_tiptext = _T("");
}

void TorrentListCtrl::HighlightItem( long item )
{

}

#endif
