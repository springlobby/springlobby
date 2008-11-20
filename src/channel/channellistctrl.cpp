#include "channellistctrl.h"
#include "../iconimagelist.h"
#include "../utils.h"
#include "../ui.h"
#include <algorithm>
#include "../Helper/sortutil.h"



BEGIN_EVENT_TABLE( ChannelListctrl, CustomVirtListCtrl )
  EVT_LIST_ITEM_ACTIVATED( CHANNELLIST, ChannelListctrl::OnActivateItem )
  EVT_LIST_COL_CLICK( CHANNELLIST, ChannelListctrl::OnColClick )
END_EVENT_TABLE()


ChannelListctrl::ChannelListctrl(wxWindow* parent, wxWindowID id, const wxString& name,
                    long style, const wxPoint& pt, const wxSize& sz)
    :CustomVirtListCtrl(parent, CHANNELLIST, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("ChannelListCtrl"), 3)

{
  wxListItem col;
  #if defined(__WXMSW__)
 /// autosize is part-broken on msw.
  SetColumnWidth( 0, 120 );
  SetColumnWidth( 1, 45 );
  SetColumnWidth( 2, 300 );

#elif defined(__WXMAC__)
/// autosize is entirely broken on wxmac.
  SetColumnWidth( 0, 120 );
  SetColumnWidth( 1, 45 );
  SetColumnWidth( 2, 300 );
#else
 /// on wxGTK it works, sort of.
  SetColumnWidth( 0, wxLIST_AUTOSIZE );
  SetColumnWidth( 1, 30 );
  SetColumnWidth( 2, wxLIST_AUTOSIZE );
#endif

  col.SetText( _("Channel name") );
  col.SetImage( icons().ICON_EMPTY );
  AddColumn( 0, -1, _("Channel"), _T("Channelname") );

  col.SetText( _("# users") );
  col.SetImage( icons().ICON_EMPTY );
  AddColumn( 1, -1, _("# users"), _T("users") );

  col.SetText( _("topic") );
  col.SetImage( icons().ICON_EMPTY );
  AddColumn( 2, -1, _T("topic"), _T("topic") );

  m_sortorder[2].col = 2;
  m_sortorder[2].direction = false;
  m_sortorder[0].col = 0;
  m_sortorder[0].direction = true;
  m_sortorder[1].col = 1;
  m_sortorder[1].direction = true;

}

ChannelListctrl::~ChannelListctrl()
{
    //dtor
}

void ChannelListctrl::HighlightItem( long item ){}

void ChannelListctrl::SetTipWindowText(const long item_hit, const wxPoint position)
{

}

/** @brief AddChannel
  *
  * @todo: document this function
  */
void ChannelListctrl::AddChannel(const wxString& channel, unsigned int num_users, const wxString& topic )
{
    //SetSelectionRestorePoint();

    ChannelInfo data ( channel, num_users, topic );
    m_data.push_back( data );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() );
    SetColumnWidth( 0, wxLIST_AUTOSIZE );
    Sort();
    //RestoreSelection();
}

struct ChannelCompareBase {
    typedef const ChannelListctrl::ChannelInfo& CompareType;
};

template < int N, bool dir >
struct ChannelCompare : public ChannelCompareBase {
};

template < int N >
struct ChannelCompare<N,true> : public ChannelCompareBase {
    static bool compare ( CompareType u1, CompareType u2 ) {
        return ChannelCompare<N,false>::compare( u2, u1);
    }
};

template < >
struct ChannelCompare < 0, false > : public ChannelCompareBase
{
    static bool compare ( CompareType u1, CompareType u2 ) {
        wxString n1 = u1.name;
        wxString n2 = u2.name;
        return n1 < n2;
    }
};
//template < >
//struct ChannelCompare < 0, true > : public ChannelCompareBase
//{
//    static bool compare ( CompareType u1, CompareType u2 ) {
//        return u2.name <  u1.name ;
//    }
//};
template < >
struct ChannelCompare < 1, false > : public ChannelCompareBase
{
    static bool compare ( CompareType u1, CompareType u2 ) {
        return u1.usercount <  u2.usercount ;
    }
};
//template < >
//struct ChannelCompare < 1, true > : public ChannelCompareBase
//{
//    static bool compare ( CompareType u1, CompareType u2 ) {
//        return u2.usercount <  u1.usercount ;
//    }
//};

void ChannelListctrl::Sort()
{
//    std::sort( m_data.begin(), m_data.end(), CompareSelector<ChannelCompare>::GetFunctor( 3,true,2,true,1,true ) );
    //SLInsertionSort( m_data, CompareSelector<ChannelCompare>::GetFunctor( 3,true,2,true,1,true ) );
    RefreshItems( 0, m_data.size() );
}

void ChannelListctrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( -1 );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 3; ++i ) {}
  if (i > 2) { i = 2; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}

/** @brief OnActivateItem
  *
  * @todo: document this function
  */
void ChannelListctrl::OnActivateItem(wxListEvent& event)
{
  int index = event.GetIndex();
  if ( index == -1 ) return;
  wxString chan_name = m_data[index].name;
  ui().JoinChannel( chan_name, _T("") );
}

void ChannelListctrl::ClearChannels()
{
    m_data.clear();
    SetItemCount( 0 );
}

wxString ChannelListctrl::GetInfo()
{
    int displayed = GetItemCount();
    int total = m_data.size();
    return wxString::Format( _("Displaying %d out of %d channels"), displayed, total );
}

void ChannelListctrl::FilterChannel( const wxString& partial )
{

//    for ( ChannelInfoIter it = m_data.begin(); it != m_data.end(); ++it ) {
//        const ChannelInfo& data = it->second;
//        if ( data.name.Contains( partial ) ) {
//            int index = InsertItem( GetItemCount(), data.name );
//            SetItem( index, 0, data.name );
//            SetItem( index, 1, TowxString( data.usercount ) );
//            SetItem( index, 2, data.topic );
//            SetItemData( index, (wxUIntPtr) &(it->second) );
//        }
//    }
    //highlight
    //HighlightItemUser( index, userdata.first );

    Sort();
}
//SetItem( index, 0, channel );
//    SetItem( index, 1, TowxString(num_users) );
//    SetItem( index, 2, topic );
//    SetItemImage( index, icons().ICON_EMPTY );

int ChannelListctrl::OnGetItemColumnImage(long item, long column) const
{
    return -1;
}

int ChannelListctrl::OnGetItemImage(long item) const
{
    return -1;
}

wxString ChannelListctrl::OnGetItemText(long item, long column) const
{
    switch ( column ) {
        case 0: return m_data[item].name;
        case 1: return TowxString( m_data[item].usercount );
        case 2: return m_data[item].topic;
        default: return wxEmptyString;
    }
}

