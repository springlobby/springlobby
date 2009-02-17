#include "channellistctrl.h"
#include "../iconimagelist.h"
#include "../utils.h"
#include "../ui.h"

BEGIN_EVENT_TABLE( ChannelListctrl, CustomListCtrl )
  EVT_LIST_ITEM_ACTIVATED( CHANNELLIST, ChannelListctrl::OnActivateItem )
  EVT_LIST_COL_CLICK( CHANNELLIST, ChannelListctrl::OnColClick )
END_EVENT_TABLE()


ChannelListctrl::ChannelListctrl(wxWindow* parent, wxWindowID id, const wxString& name,
                    long style, const wxPoint& pt, const wxSize& sz)
    :CustomListCtrl(parent, CHANNELLIST, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("ChannelListCtrl"))

{
  wxListItem col;

  col.SetText( _("Channel name") );
  col.SetImage( icons().ICON_EMPTY );
  InsertColumn( 0, col, _T("Channelname") );

  col.SetText( _("# users") );
  col.SetImage( icons().ICON_EMPTY );
  InsertColumn( 1, col, _T("users") );

  col.SetText( _("topic") );
  col.SetImage( icons().ICON_EMPTY );
  InsertColumn( 2, col, _T("topic") );

  m_sortorder[2].col = 2;
  m_sortorder[2].direction = false;
  m_sortorder[0].col = 0;
  m_sortorder[0].direction = true;
  m_sortorder[1].col = 1;
  m_sortorder[1].direction = true;


#if defined(__WXMSW__)
 // autosize is part-broken on msw.
  SetColumnWidth( 0, 120 );
  SetColumnWidth( 1, 45 );
  SetColumnWidth( 2, 300 );

#elif defined(__WXMAC__)
// autosize is entirely broken on wxmac.
  SetColumnWidth( 0, 120 );
  SetColumnWidth( 1, 45 );
  SetColumnWidth( 2, 300 );
#else
 // on wxGTK it works, sort of.
  SetColumnWidth( 0, wxLIST_AUTOSIZE );
  SetColumnWidth( 1, 30 );
  SetColumnWidth( 2, wxLIST_AUTOSIZE );
#endif

}

ChannelListctrl::~ChannelListctrl()
{
    //dtor
}

/** @brief SetTipWindowText
  *
  * @todo: document this function
  */
void ChannelListctrl::SetTipWindowText(const long item_hit, const wxPoint position)
{

}

/** @brief AddChannel
  *
  * @todo: document this function
  */
void ChannelListctrl::AddChannel(const wxString& channel, unsigned int num_users, const wxString& topic )
{
    SetSelectionRestorePoint();

    //todo: don't add if already in it?

    ChannelInfo data ( channel, num_users, topic );
    int index = InsertItem( GetItemCount(), channel );
    SetItem( index, 0, channel );
    SetItem( index, 1, TowxString(num_users) );
    SetItem( index, 2, topic );
    SetItemImage( index, icons().ICON_EMPTY );
    m_data[index] = data;
    SetItemData(index, (wxUIntPtr) &m_data[index]);
    //highlight
    //HighlightItemUser( index, userdata.first );

    Sort();
    RestoreSelection();
}

/** @brief Sort
  *
  * @todo: document this function
  */
void ChannelListctrl::Sort()
{
    for (int i = 1; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&CompareChannelnameUP:&CompareChannelnameDOWN, 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareNumUsersUP:&CompareNumUsersDOWN , 0 ); break;

    }
  }
}

/** @brief HighlightItem
  *
  * @todo: document this function
  */
void ChannelListctrl::HighlightItem(long item)
{

}
/** @brief CompareNumUsersDOWN
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareNumUsersDOWN(long item1, long item2, long sortData)
{
    return  (*(ChannelInfo*) item1 ).usercount <  (*(ChannelInfo*) item2 ).usercount;
}

/** @brief CompareNumUsersUP
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareNumUsersUP(long item1, long item2, long sortData)
{
    return  (*(ChannelInfo*) item1 ).usercount >=  (*(ChannelInfo*) item2 ).usercount;
}

/** @brief CompareChannelnameDOWN
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareChannelnameDOWN(long item1, long item2, long sortData)
{
    return (*(ChannelInfo*) item1 ).name.Upper() <  (*(ChannelInfo*) item2 ).name.Upper();
}

/** @brief CompareChannelnameUP
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareChannelnameUP(long item1, long item2, long sortData)
{
    return (*(ChannelInfo*) item1 ).name.Upper() >=  (*(ChannelInfo*) item2 ).name.Upper();
}

void ChannelListctrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( -1 );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 2; ++i ) {}
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
  wxString chan_name = (*(ChannelInfo*)event.GetData() ).name;
  ui().JoinChannel( chan_name, _T("") );
}

void ChannelListctrl::ClearChannels()
{
    DeleteAllItems();
    m_data.clear();
}

wxString ChannelListctrl::GetInfo()
{
    int displayed = GetItemCount();
    int total = m_data.size();
    return wxString::Format( _("Displaying %d out of %d channels"), displayed, total );
}

void ChannelListctrl::FilterChannel( const wxString& partial )
{
    DeleteAllItems();

    for ( ChannelInfoIter it = m_data.begin(); it != m_data.end(); ++it ) {
        const ChannelInfo& data = it->second;
        if ( data.name.Contains( partial ) ) {
            int index = InsertItem( GetItemCount(), data.name );
            SetItem( index, 0, data.name );
            SetItem( index, 1, TowxString( data.usercount ) );
            SetItem( index, 2, data.topic );
            SetItemData( index, (wxUIntPtr) &(it->second) );
        }
    }
    //highlight
    //HighlightItemUser( index, userdata.first );

    Sort();
}
