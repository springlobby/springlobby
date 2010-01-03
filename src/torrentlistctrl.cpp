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

static const wxString na_str = wxString(_("N/A"));

template<> SortOrder TorrentListCtrl::BaseType::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE( TorrentListCtrl, TorrentListCtrl::BaseType )

	EVT_LIST_ITEM_RIGHT_CLICK	( TLIST_CLICK, TorrentListCtrl::OnListRightClick )
	//EVT_LIST_COL_CLICK			( TLIST_CLICK, TorrentListCtrl::OnColClick )
	EVT_MENU					( TLIST_CANCEL, TorrentListCtrl::OnCancel )
	EVT_MENU					( TLIST_RETRY, TorrentListCtrl::OnRetry )
	#if wxUSE_TIPWINDOW
	#ifndef __WXMSW__ //disables tooltips on win
	EVT_MOTION( TorrentListCtrl::OnMouseMotion )
	#endif
	#endif
END_EVENT_TABLE()

TorrentListCtrl::TorrentListCtrl( wxWindow* parent )
:	TorrentListCtrl::BaseType( parent, TLIST_CLICK, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("TorrentListCtrl"), 10, &CompareOneCrit )
, m_popup(0)
{
#if defined(__WXMAC__)
/// on mac, autosize does not work at all
    const int widths[10] = { 200, 80, 100, 80, 80, 80, 80, 80, 80, 80 };
#else
	const int widths[10] = { 200, wxLIST_AUTOSIZE_USEHEADER, wxLIST_AUTOSIZE_USEHEADER, wxLIST_AUTOSIZE_USEHEADER, 80, wxLIST_AUTOSIZE_USEHEADER, 80, 80, 80, wxLIST_AUTOSIZE_USEHEADER };
#endif

	AddColumn(0, widths[0], _T("Name"), _T("Name"));
	AddColumn(1, widths[1], _T("Numcopies"), _T("# complete copies"));
	AddColumn(2, widths[2], _T("Status"), _T("Status"));
	AddColumn(3, widths[3], _T("% complete"), _T("% complete"));
	AddColumn(4, widths[4], _T("KB/s down"), _T("KB/s download"));
	AddColumn(5, widths[5], _T("ETA"), _T("Estimated time remaining"));
	AddColumn(6, widths[6], _T("Filesize (MB)"), _T("Filesize"));


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


TorrentListCtrl::~TorrentListCtrl()
{
  delete m_popup;
  m_popup = 0;
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
	{
		if(m_data.size() == 0)
			Refresh();
		return true;
	}
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
	int idx = event.GetIndex();
    if ( idx < (long)m_data.size() && idx > -1 ) {

        DataType dt = m_data[idx];
		delete m_popup;
        m_popup = new wxMenu( _T("") );
		if(dt.downloadstatus == P2P::not_stored)
		{
			m_popup->Append( TLIST_CANCEL, _("Cancel torrent") );
			m_popup->Append( TLIST_RETRY, _("Retry torrent") );
		}
		else if(dt.downloadstatus == P2P::queued || dt.downloadstatus == P2P::leeching)
			m_popup->Append( TLIST_CANCEL, _("Cancel torrent") );
		else if(dt.downloadstatus == P2P::stored || dt.downloadstatus == P2P::seeding)
			m_popup->Append( TLIST_CANCEL, _("Cancel torrent (keeping downloaded file)") );

        PopupMenu( m_popup );
    }
}

void TorrentListCtrl::OnCancel(wxCommandEvent &/*event*/)
{
	torrent().RemoveTorrentByName(GetSelectedData().name);
	RemoveTorrentInfo(GetSelectedData());
}


void TorrentListCtrl::OnRetry(wxCommandEvent &/*event*/)
{
//	torrent().RequestFileByHash(GetSelectedData().hash);
    assert( false );
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


void TorrentListCtrl::SetTipWindowText( const long /*item_hit*/, const wxPoint /*position*/)
{
    m_tiptext = _T("");
}

void TorrentListCtrl::HighlightItem( long /*item*/ )
{

}

int TorrentListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    switch ( col ) {
        case 0: return dir * u1.name.CmpNoCase( u2.name );
        case 1: return dir * compareSimple( u1.numcopies, u2.numcopies );
		case 2: return dir * compareSimple( u1.downloadstatus, u2.downloadstatus );
		case 3: return dir * compareSimple( u1.progress, u2.progress );
		case 4: return dir * compareSimple( u1.inspeed, u2.inspeed );
		case 5: return dir * compareSimple( u1.eta, u2.eta );
		case 6: return dir * compareSimple( u1.filesize, u2.filesize );
        default: return 0;
    }
}

int TorrentListCtrl::GetItemColumnImage(long /*item*/, long /*column*/) const
{
    return -1;
}

int TorrentListCtrl::GetItemImage(long /*item*/) const
{
    return -1;
}


wxString TorrentListCtrl::GetItemText(long item, long column) const
{

    if ( item > (long)m_data.size() || item < 0 )
        return wxEmptyString;

	float kfactor = 1/float(1024);
	float mfactor = 1/float(1024*1024);

	const TorrentInfos& infos = m_data[item];

	switch ( column ) {
        default: return wxEmptyString;
        case 0: return infos.name;
        case 1: return infos.numcopies > 0 ? wxString::Format(_T("%.2f"), infos.numcopies ) : na_str;
		case 2:
			if(infos.downloadstatus == P2P::not_stored) return _("not found");
			else if(infos.downloadstatus == P2P::queued) return _("queued");
			else if(infos.downloadstatus == P2P::leeching) return _("leeching");
			else if(infos.downloadstatus == P2P::stored) return _("complete");
			else if(infos.downloadstatus == P2P::seeding) return _("seeding");
			else return wxEmptyString;
		case 3: return infos.progress > -0.01 ? wxString::Format(_T("%.2f"), infos.progress * 100 ) : na_str;
		case 4: return infos.inspeed > -0.01 ? wxString::Format(_T("%.2f"), infos.inspeed*kfactor ) : na_str;
		case 5: return infos.eta > -1 ? wxTimeSpan::Seconds(infos.eta).Format( _T("%H:%M:%S") ) : _T("inf.") ;
		case 6: return infos.filesize > 0 ? wxString::Format(_T("%.2f"), infos.filesize*mfactor) : na_str;
	}
}

int TorrentListCtrl::GetIndexFromData( const DataType& data ) const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it , ++i) {
        if ( it->name == data.name )
            return i;
    }
    return -1;
}

bool TorrentListCtrl::IsTorrentActive(const DataType& info)
{
	 return (info.downloadstatus == P2P::leeching
		  || info.downloadstatus == P2P::queued);
}

#endif
