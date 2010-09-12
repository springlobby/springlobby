/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#ifndef NO_TORRENT_SYSTEM


#include "filelistctrl.h"

#include <wx/intl.h>
#include <wx/menu.h>

#include "filelistdialog.h"
#include "../utils/conversion.h"
#include "../iconimagelist.h"
#include "../uiutils.h"
#include "../utils/downloader.h"

template<> SortOrder FileListCtrl::BaseType::m_sortorder = SortOrder( ) ;

BEGIN_EVENT_TABLE( FileListCtrl, FileListCtrl::BaseType )

	EVT_LIST_ITEM_RIGHT_CLICK( FILELIST_COL_CLICK, FileListCtrl::OnListRightClick )

END_EVENT_TABLE()

const int fileListDialogID = wxNewId();

FileListCtrl::FileListCtrl( FileListDialog* parent  )
	: FileListCtrl::BaseType( parent, fileListDialogID, wxDefaultPosition, wxDefaultSize,
            wxLC_VIRTUAL | wxSUNKEN_BORDER | wxLC_REPORT , _T("FileListCtrl"), 2,
			&FileListCtrl::CompareOneCrit, false/*no highlights*/, UserActions::ActHighlight, false /*periodic sort*/ ),
    m_parent_dialog( parent )
{
#if defined(__WXMAC__)
	const int widths [4] = { 250, 150, 50, 50 };
#else
	const int widths [4] = { wxLIST_AUTOSIZE, wxLIST_AUTOSIZE, wxLIST_AUTOSIZE, wxLIST_AUTOSIZE };
#endif

	AddColumn( 0, widths[0], _( "Name" ), _( "Name" ) );
	AddColumn( 1, widths[1], _( "Size" ), _( "Size" ) );
	AddColumn( 2, widths[2], _( "Type" ), _( "Type" ) );
	AddColumn( 3, widths[3], _( "Depends" ), _( "has dependencies?" ) );

	if ( m_sortorder.size() == 0 ) {
		m_sortorder[0].col = 0;
		m_sortorder[0].direction = -1;
		m_sortorder[1].col = 1;
		m_sortorder[1].direction = 1;
	}

//  m_popup = new wxMenu( _T("") );
//  // &m enables shortcout "alt + m" and underlines m
//  m_popup->Append( BLIST_DLMAP, _("Download &map") );
//  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}

FileListCtrl::~FileListCtrl()
{
	//delete m_popup;
}

void FileListCtrl::OnListRightClick( wxListEvent& /*unused*/ )
{
//	PopupMenu( m_popup );
}

void FileListCtrl::Sort()
{
    if ( m_data.size() > 0 )
	{
		SaveSelection();
		SlQuickSort::Sort ( m_data, m_comparator );
		RestoreSelection();
	}
}

bool FileListCtrl::AddFile( DataType info )
{
    return AddItem( info );
}

/** @brief GetIndexFromData
  *
  * we use a static seekpos here cause a lot of sequential index gettings expected
  */
int FileListCtrl::GetIndexFromData(const DataType& data) const
{
	static long seekpos;
	seekpos = clamp( seekpos, 0l , ( long )m_data.size() );
	int index = seekpos;

	for ( DataCIter f_idx = m_data.begin() + seekpos; f_idx != m_data.end() ; ++f_idx )
	{
		if ( data == *f_idx )
		{
			seekpos = index;
			return seekpos;
		}
		index++;
	}
	//it's ok to init with seekpos, if it had changed this would not be reached
	int r_index = seekpos - 1;
	for ( DataRevCIter r_idx = m_data.rbegin() + ( m_data.size() - seekpos ); r_idx != m_data.rend() ; ++r_idx )
	{
		if ( data == *r_idx )
		{
			seekpos = r_index;
			return seekpos;
		}
		r_index--;
	}

	return -1;
}

/** @brief CompareOneCrit
  *
  * @todo: document this function
  */
int FileListCtrl::CompareOneCrit(DataType u1, DataType u2, int col, int dir) const
{
    if ( ! ( u1 && u2 ) )
        return 0;
	switch ( col ) {
		case 0:
			return dir * u2->m_name.CmpNoCase( u2->m_name ) ;
		case 1:
			return dir * compareSimple( u2->m_size_KB, u1->m_size_KB );
		case 2:
			return dir * compareSimple( u2->m_type, u1->m_type );
		case 3:
			return dir * compareSimple( u2->m_dependencies.Count(), u1->m_dependencies.Count() );
		default:
			return 0;
	}
}

/** @brief GetItemAttr
  *
  * @todo: document this function
  */
wxListItemAttr * FileListCtrl::GetItemAttr(long ) const
{
    //currently no highlighting
    return 0;
}

/** @brief GetItemColumnImage
  *
  * @todo: document this function
  */
int FileListCtrl::GetItemColumnImage(long , long ) const
{
    //no images for any item
    return -1;
}

/** @brief GetItemText
  *
  * @todo: document this function
  */
wxString FileListCtrl::GetItemText(long item, long column) const
{
    const DataType info = m_data[item];
    if ( !info )
        return wxEmptyString;

    switch ( column ) {
		case 0: return info->m_name;
		case 1: return TowxString( info->m_size_KB );
		case 2: switch ( info->m_type ) {
                    case PlasmaResourceInfo::map: return _("map");
                    case PlasmaResourceInfo::mod: return _("mod");
                    default: return _("unknown");
                }
		case 3: return TowxString( info->m_dependencies.Count() );
        default: return wxEmptyString;
    }
	return wxEmptyString;
}

/** @brief GetSelectedHashes
  *
  * @todo: document this function
  */
void FileListCtrl::GetSelectedHashes(InternalNameVector& names)
{
	long item = -1;
	for ( long i = 0; i < GetSelectedItemCount(); ++i )
	{
		item = GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
		if ( item == -1 ) // means nothing was found
            return;
		names.push_back( m_data[item]->m_name );
	}
}



#endif
