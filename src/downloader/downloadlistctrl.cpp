/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/datetime.h>
#include <boost/algorithm/string.hpp>

#include "downloadlistctrl.h"
#include "prdownloader.h"
#include "../iconimagelist.h"
#include "../utils/conversion.h"

static const wxString na_str = wxString(_("N/A"));

template<> SortOrder DownloadListCtrl::BaseType::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE( DownloadListCtrl, DownloadListCtrl::BaseType )

	EVT_LIST_ITEM_RIGHT_CLICK	( TLIST_CLICK, DownloadListCtrl::OnListRightClick )
	//EVT_LIST_COL_CLICK			( TLIST_CLICK, DownloadListCtrl::OnColClick )
	EVT_MENU					( TLIST_CANCEL, DownloadListCtrl::OnCancel )
	EVT_MENU					( TLIST_RETRY, DownloadListCtrl::OnRetry )
	#if wxUSE_TIPWINDOW
	#ifndef __WXMSW__ //disables tooltips on win
	EVT_MOTION( DownloadListCtrl::OnMouseMotion )
	#endif
	#endif
END_EVENT_TABLE()

DownloadListCtrl::DownloadListCtrl( wxWindow* parent )
:	DownloadListCtrl::BaseType( parent, TLIST_CLICK, wxDefaultPosition, wxDefaultSize,
							   wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT,
                               _T("DownloadListCtrl"), 3, &DownloadListCtrl::CompareOneCrit )
, m_popup(0)
{
#if defined(__WXMAC__)
/// on mac, autosize does not work at all
    const int widths[9] = { 200, 100, 80, 80, 80, 80, 80, 80, 80 };
#else
    const int widths[9] = { 200, wxLIST_AUTOSIZE_USEHEADER, wxLIST_AUTOSIZE_USEHEADER, 80, wxLIST_AUTOSIZE_USEHEADER, 80, 80, 80, wxLIST_AUTOSIZE_USEHEADER };
#endif

	AddColumn(0, widths[0], _T("Name"), _T("Name"));
    AddColumn(1, widths[1], _T("Status"), _T("Status"));
    AddColumn(2, widths[2], _T("% complete"), _T("% complete"));
    AddColumn(3, widths[3], _T("KB/s down"), _T("KB/s download"));
    AddColumn(4, widths[4], _T("ETA"), _T("Estimated time remaining"));
    AddColumn(5, widths[5], _T("Filesize (MB)"), _T("Filesize"));


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

}


DownloadListCtrl::~DownloadListCtrl()
{
  delete m_popup;
  m_popup = 0;
}


bool DownloadListCtrl::AddTorrentInfo(const DataType& info)
{
	if(AddItem(info))
		return true;
	return false;
}


bool DownloadListCtrl::RemoveTorrentInfo(const DataType& info)
{
	if(RemoveItem(info))
	{
		if(m_data.size() == 0)
			Refresh();
		return true;
	}
	return false;
}


void DownloadListCtrl::UpdateTorrentInfo(const DataType& info)
{
	int index = GetIndexFromData(info);

	if( index < 0)
	{
		if(IsTorrentActive(info))
			AddTorrentInfo(info);
		return;
	}

	m_data[index] = info;
	RefreshItem( index );
	MarkDirtySort();
}


void DownloadListCtrl::OnListRightClick( wxListEvent& /*event*/ )
{
//	int idx = event.GetIndex();
//    if ( idx < (long)m_data.size() && idx > -1 ) {

//        DataType dt = m_data[idx];
//		delete m_popup;
//        m_popup = new wxMenu( _T("") );
//		if(dt.downloadstatus == P2P::not_stored)
//		{
//			m_popup->Append( TLIST_CANCEL, _("Cancel download") );
//			m_popup->Append( TLIST_RETRY, _("Retry download") );
//		}
//		else if(dt.downloadstatus == P2P::queued || dt.downloadstatus == P2P::leeching)
//			m_popup->Append( TLIST_CANCEL, _("Cancel download") );
//		else if(dt.downloadstatus == P2P::complete)
//			m_popup->Append( TLIST_CANCEL, _("Remove download (keeping downloaded file)") );

//        PopupMenu( m_popup );
//    }
}

void DownloadListCtrl::OnCancel(wxCommandEvent &/*event*/)
{
	prDownloader().RemoveTorrentByName(GetSelectedData().name);
	RemoveTorrentInfo(GetSelectedData());
}


void DownloadListCtrl::OnRetry(wxCommandEvent &/*event*/)
{
	DataType info( GetSelectedData() );
	prDownloader().RemoveTorrentByName( info.name );
	RemoveTorrentInfo( info );
    assert(false);
//	prDownloader().RequestFileByName( info.name );
}


void DownloadListCtrl::Sort()
{
	if ( m_data.size() > 0 )
    {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}


void DownloadListCtrl::SetTipWindowText( const long /*item_hit*/, const wxPoint& /*position*/)
{
    m_tiptext = _T("");
}

void DownloadListCtrl::HighlightItem( long /*item*/ )
{

}

int DownloadListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const
{
    switch ( col ) {
        case 0: return dir * compareSimple(boost::to_lower_copy(u1.name), boost::to_lower_copy(u2.name));
        case 1: return dir * compareSimple( u1.numcopies, u2.numcopies );
		case 2: return dir * compareSimple( u1.downloadstatus, u2.downloadstatus );
		case 3: return dir * compareSimple( u1.progress, u2.progress );
		case 4: return dir * compareSimple( u1.inspeed, u2.inspeed );
		case 5: return dir * compareSimple( u1.eta, u2.eta );
		case 6: return dir * compareSimple( u1.filesize, u2.filesize );
        default: return 0;
    }
}

int DownloadListCtrl::GetItemColumnImage(long /*item*/, long /*column*/) const
{
    return -1;
}

int DownloadListCtrl::GetItemImage(long /*item*/) const
{
    return -1;
}


wxString DownloadListCtrl::GetItemText(long item, long column) const
{

    if ( item > (long)m_data.size() || item < 0 )
        return wxEmptyString;

	float kfactor = 1/float(1024);
	float mfactor = 1/float(1024*1024);

	const DataType& infos = m_data[item];

	switch ( column ) {
        default: return wxEmptyString;
        case 0: return TowxString(infos.name);
		case 1: return infos.numcopies > 0 ? (wxFormat(_T("%.2f") ) % infos.numcopies).str() : na_str;
		case 2:
			if(infos.downloadstatus == P2P::not_stored) return _("not found");
			else if(infos.downloadstatus == P2P::queued) return _("queued");
			else if(infos.downloadstatus == P2P::leeching) return _("downloading");
			else if(infos.downloadstatus == P2P::complete) return _("complete");
			else return wxEmptyString;
		case 3: return infos.progress > -0.01 ? (wxFormat(_T("%.2f") ) % ( infos.progress * 100 )).str() : na_str;
		case 4: return infos.inspeed > -0.01 ? (wxFormat(_T("%.2f") ) % ( infos.inspeed*kfactor )).str() : na_str;
		case 5: return infos.eta > -1 ? wxTimeSpan::Seconds(infos.eta).Format( _T("%H:%M:%S") ) : _T("inf.") ;
		case 6: return infos.filesize > 0 ? (wxFormat(_T("%.2f") ) % ( infos.filesize*mfactor)).str() : na_str;
	}
}

int DownloadListCtrl::GetIndexFromData( const DataType& data ) const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it , ++i) {
        if ( it->name == data.name )
            return i;
    }
    return -1;
}

bool DownloadListCtrl::IsTorrentActive(const DataType& info) const
{
	 return (info.downloadstatus == P2P::leeching
		  || info.downloadstatus == P2P::queued);
}
