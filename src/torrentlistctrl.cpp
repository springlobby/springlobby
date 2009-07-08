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
#include <wx/datetime.h>

#include "torrentlistctrl.h"
#include "torrentwrapper.h"
//#include "utils/.h"
#include "iconimagelist.h"

#include "utils/conversion.h"

template<> SortOrder TorrentListCtrl::BaseType::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE( TorrentListCtrl, TorrentListCtrl::BaseType )

	EVT_LIST_ITEM_RIGHT_CLICK( TLIST_CLICK, TorrentListCtrl::OnListRightClick )
	EVT_LIST_COL_CLICK( TLIST_CLICK, TorrentListCtrl::OnColClick )
	#if wxUSE_TIPWINDOW
	#ifndef __WXMSW__ //disables tooltips on win
	EVT_MOTION( TorrentListCtrl::OnMouseMotion )
	#endif
	#endif
END_EVENT_TABLE()

TorrentListCtrl::TorrentListCtrl( wxWindow* parent, Ui& ui ):
		TorrentListCtrl::BaseType( parent, TLIST_CLICK, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("TorrentListCtrl"), 10, 10, &CompareOneCrit )

{
	const int widths [3] = { wxLIST_AUTOSIZE, wxLIST_AUTOSIZE, wxLIST_AUTOSIZE };

	AddColumn(0, widths[0], _T("Name"), _T("Name"));
	AddColumn(1, widths[0], _T("Numcopies"), _T("# complete copies"));
	AddColumn(2, widths[0], _T("MB downloaded"), _T("MB downloaded"));
	AddColumn(3, widths[0], _T("MB uploaded"), _T("MB uploade"));
	AddColumn(4, widths[0], _T("Status"), _T("Status"));
	AddColumn(5, widths[0], _T("% complete"), _T("% complete"));
	AddColumn(6, widths[0], _T("KB/s up"), _T("KB/s upload"));
	AddColumn(7, widths[0], _T("KB/s down"), _T("KB/s download"));
	AddColumn(8, widths[0], _T("ETA"), _T("Estimated time remaining"));
	AddColumn(9, widths[0], _T("Filesize (MB)"), _T("Filesize"));


// sortorder: name --> percent completed --> mb donwloaded

	if ( m_sortorder.size() == 0 ) 
	{
		m_sortorder[0].col = 0;
		m_sortorder[0].direction = 1;
		m_sortorder[1].col = 5;
		m_sortorder[1].direction = 1;
		m_sortorder[2].col = 2;
		m_sortorder[2].direction = 1;
	}

	Sort( );

	//TODO this'll need fixing on win i assume [koshi]
	/*SetColumnWidth( 0, 250 );
	SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
	SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );
	SetColumnWidth( 7, 80 );
	SetColumnWidth( 8, wxLIST_AUTOSIZE_USEHEADER );
	SetColumnWidth( 9, wxLIST_AUTOSIZE_USEHEADER );


	SetColumnWidth( 3, 100 );
	SetColumnWidth( 4, 70 );
	SetColumnWidth( 5, 100 );
	SetColumnWidth( 6, 70 );*/

//  m_popup = new wxMenu( _T("") );
//  // &m enables shortcout "alt + m" and underlines m
//  m_popup->Append( BLIST_DLMAP, _("Download &map") );
//  m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}


TorrentListCtrl::~TorrentListCtrl()
{
//  delete m_popup;
}


bool TorrentListCtrl::AddTorrentInfo(const DataType& info)
{
	if(AddItem(info))
		return true;
	return false;
}


bool TorrentListCtrl::RemoveTorrentInfo(const DataType& info)
{
	if(RemoveItem(info))
		return true;
	return false;
}

void TorrentListCtrl::UpdateTorrentInfo(const DataType& info)
{
	int index = GetIndexFromData(info);

	if( index < 0)
	{
		if(IsTorrentActive(info))
			AddTorrentInfo(info);
		return;
	}
	
	if(!IsTorrentActive(info))
	{
		RemoveTorrentInfo(info);
		return;
	}
	else
	{
		m_data[index] = info;
	}

	RefreshItem( index );
	MarkDirtySort();
}


void TorrentListCtrl::OnListRightClick( wxListEvent& event )
{
//  PopupMenu( m_popup );
}




void TorrentListCtrl::Sort()
{
	if ( m_data.size() > 0 )
    {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}


void TorrentListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
    m_tiptext = _T("");
}

void TorrentListCtrl::HighlightItem( long item )
{

}

int TorrentListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    switch ( col ) {
        case 0: return dir * u1.name.CmpNoCase( u2.name );
        case 1: return dir * compareSimple( u1.numcopies, u2.numcopies );
        case 2: return dir * compareSimple( u1.downloaded, u2.downloaded );
		case 3: return dir * compareSimple( u1.uploaded, u2.uploaded );
		case 4: return dir * compareSimple( u1.downloadstatus, u2.downloadstatus );
		case 5: return dir * compareSimple( u1.progress, u2.progress );
		case 6: return dir * compareSimple( u1.outspeed, u2.outspeed );
		case 7: return dir * compareSimple( u1.inspeed, u2.inspeed );
		case 8: return dir * compareSimple( u1.eta, u2.eta );
		case 9: return dir * compareSimple( u1.filesize, u2.filesize );
        default: return 0;
    }
}

int TorrentListCtrl::GetItemColumnImage(long item, long column) const
{
    return -1;
}

int TorrentListCtrl::GetItemImage(long item) const
{
    return -1;
}


wxString TorrentListCtrl::GetItemText(long item, long column) const
{

    if ( item > m_data.size() || item < 0 )
        return wxEmptyString;

	float kfactor = 1/float(1024);
	float mfactor = 1/float(1024*1024);

	const TorrentInfos& infos = m_data[item];

	switch ( column ) {
        default: return wxEmptyString;
        case 0: return infos.name;
        case 1: return infos.numcopies > 0 ? wxString::Format(_T("%.2f"), infos.numcopies ) : _("N/A");
        case 2: return wxString::Format(_T("%.2f"), infos.downloaded*mfactor );
        case 3: return wxString::Format(_T("%.2f"), infos.uploaded*mfactor );
		case 4:
			if(infos.downloadstatus == P2P::seeding) return _("seeding");
			else if(infos.downloadstatus == P2P::leeching) return _("leeching");
			else if(infos.downloadstatus == P2P::queued) return _("queued");
			else return wxEmptyString;
		case 5: return wxString::Format(_T("%.2f"), infos.progress * 100 );
		case 6: return wxString::Format(_T("%.2f"), infos.outspeed*kfactor );
		case 7: return wxString::Format(_T("%.2f"), infos.inspeed*kfactor );
		case 8: return infos.eta > -1 ? wxTimeSpan::Seconds(infos.eta).Format("%H:%M:%S") : _T("inf.") ;
		case 9: return wxString::Format(_T("%.2f"), infos.filesize*mfactor);
	}
}

int TorrentListCtrl::GetIndexFromData( const DataType& data ) const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it , ++i) {
        if ( it->hash == data.hash )
            return i;
    }
    return -1;
}

bool TorrentListCtrl::IsTorrentActive(const DataType& info)
{
	 return (info.downloadstatus == P2P::seeding 
		  || info.downloadstatus == P2P::leeching 
		  || info.downloadstatus == P2P::queued);
}

#endif
