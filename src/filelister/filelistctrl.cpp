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
#include <wx/string.h>

#include "filelistctrl.h"
#include "filelistdialog.h"
#include "../utils.h"
#include "../iconimagelist.h"
#include "../uiutils.h"


BEGIN_EVENT_TABLE( FileListCtrl, CustomListCtrl )

	EVT_LIST_ITEM_RIGHT_CLICK( FILELIST_COL_CLICK, FileListCtrl::OnListRightClick )
	EVT_LIST_COL_CLICK( FILELIST_COL_CLICK, FileListCtrl::OnColClick )
	#if wxUSE_TIPWINDOW
	#if !defined(__WXMSW__) /* && !defined(__WXMAC__) */ //disables tooltips on msw /* and mac */
	EVT_MOTION( FileListCtrl::OnMouseMotion )
	#endif
	#endif
END_EVENT_TABLE()

FileListDialog* FileListCtrl::s_parent_dialog = 0;

FileListCtrl::FileListCtrl( wxWindow* parent, FileListDialog* fld  ):
		CustomListCtrl( parent, FILELIST_COL_CLICK, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, _T("FileListCtrl"), 3 ),
        m_parent_dialog( fld )
{
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

	//Sort( );

//  m_popup = new wxMenu( _T("") );
//  // &m enables shortcout "alt + m" and underlines m
//  m_popup->Append( BLIST_DLMAP, _("Download &map") );
//  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}

void FileListCtrl::SetColumnWidths()
{
#if defined(__WXMAC__)
// on mac, autosize does not work at all
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
	//PopupMenu( m_popup );
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
	col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
	SetColumn( m_sortorder[0].col, col );

	Sort();
}

void FileListCtrl::GetSelectedHashes(HashVector& hashes)
{
	long item = -1;
	for ( long i = 0; i < GetSelectedItemCount(); ++i )
	{
		item = GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
		if ( item == -1 ) // means nothing was found
            return;
		hashes.push_back( TowxString<unsigned int>( GetItemData(item) ) );
	}
}


void FileListCtrl::Sort()
{
	bool changed = false;
    s_parent_dialog = m_parent_dialog;
//  if (!m_ui_for_sort || !m_ui_for_sort->GetServerStatus()  ) return;
//SortItems(( true )?&CompareNameUP:&CompareNameDOWN , 0 );
	for ( int i = 3; i >= 0; i-- )
	{
		switch ( m_sortorder[ i ].col )
		{
			case 0 :
				changed = SortItems(( m_sortorder[ i ].direction )?&CompareNameUP:&CompareNameDOWN , 0 );
				break;
			case 1 :
				changed = SortItems(( m_sortorder[ i ].direction )?&CompareTypeUP:&CompareTypeDOWN , 0 );
				break;
			case 2 :
				changed = SortItems(( m_sortorder[ i ].direction )?&CompareHashUP:&CompareHashDOWN , 0 );
				break;
		}
	}
}


int wxCALLBACK FileListCtrl::CompareNameUP( long item1, long item2, long sortData )
{
  TorrentTable::PRow row1=s_parent_dialog->RowByHash(TowxString<long>(item1));
  TorrentTable::PRow row2=s_parent_dialog->RowByHash(TowxString<long>(item2));
  wxString name1 = row1.ok() ? row1->name.Upper() : _T("");
  wxString name2 = row2.ok() ? row2->name.Upper() : _T("");
  return name1.CompareTo(name2);
}


int wxCALLBACK FileListCtrl::CompareNameDOWN( long item1, long item2, long sortData )
{
    TorrentTable::PRow row1=s_parent_dialog->RowByHash(TowxString<long>(item1));
    TorrentTable::PRow row2=s_parent_dialog->RowByHash(TowxString<long>(item2));
    wxString name1 = row1.ok() ? row1->name.Upper() : _T("");
    wxString name2 = row2.ok() ? row2->name.Upper() : _T("");
    return name2.CompareTo(name1);
}


int wxCALLBACK FileListCtrl::CompareTypeUP( long item1, long item2, long sortData )
{
    TorrentTable::PRow row1=s_parent_dialog->RowByHash(TowxString<long>(item1));
    TorrentTable::PRow row2=s_parent_dialog->RowByHash(TowxString<long>(item2));

    wxString name1 = row1.ok() ? (row1->type == IUnitSync::map ? _("Map") : _("Mod")) : _T("");
    wxString name2 = row2.ok() ? (row2->type == IUnitSync::map ? _("Map") : _("Mod")) : _T("");

    return name1.CompareTo(name2);
}


int wxCALLBACK FileListCtrl::CompareTypeDOWN( long item1, long item2, long sortData )
{
    TorrentTable::PRow row1=s_parent_dialog->RowByHash(TowxString<long>(item1));
    TorrentTable::PRow row2=s_parent_dialog->RowByHash(TowxString<long>(item2));

    wxString name1 = row1.ok() ? (row1->type == IUnitSync::map ? _("Map") : _("Mod")) : _T("");
    wxString name2 = row2.ok() ? (row2->type == IUnitSync::map ? _("Map") : _("Mod")) : _T("");

    return name2.CompareTo(name1);
}


int wxCALLBACK FileListCtrl::CompareHashUP( long item1, long item2, long sortData )
{
  if ( item1 < item2 )
		return -1;
	if ( item1 > item2 )
		return 1;

	return 0;
}



int wxCALLBACK FileListCtrl::CompareHashDOWN( long item1, long item2, long sortData )
{
  if ( item1 > item2 )
    return -1;
	if ( item1 < item2 )
    return 1;

	return 0;
}

void FileListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
//    long item = GetItemData( item_hit );
//			Ui* ui = m_ui_for_sort;
//			Battle& battle = ui->GetServer().battles_iter->GetBattle(item);
    int column = getColumnFromPosition( position );
    switch ( column )
    {
//			case 0: // status
//			m_tiptext = icons().GetBattleStatus(battle);
//				break;
//			case 1: // country
//				m_tiptext = GetFlagNameFromCountryCode(battle.GetFounder().GetCountry());
//				break;
//			case 2: // rank_min
//				m_tiptext = m_colinfovec[column].first;
//				break;


        default:
            m_tiptext = _T( "" );
            break;
    }
}

void FileListCtrl::HighlightItem( long item )
{

}

#endif

