#include "channellistctrl.h"
#include "iconimagelist.h"
#include "utils.h"
#include "ui.h"

BEGIN_EVENT_TABLE( ChannelListctrl, customListCtrl )
  EVT_LIST_ITEM_ACTIVATED( CHANNELLIST, ChannelListctrl::OnActivateItem )
  EVT_LIST_COL_CLICK( CHANNELLIST, ChannelListctrl::OnColClick )
END_EVENT_TABLE()


ChannelListctrl::ChannelListctrl(wxWindow* parent, wxWindowID id, const wxString& name, long style, const wxPoint& pt,const wxSize& sz)
    :customListCtrl(parent, CHANNELLIST, pt, sz, style, name, false)
{
     wxListItem col;

  col.SetImage( -1 );
  InsertColumn( 0, col, _T("Icon") );

  col.SetText( _("Channel name") );
  col.SetImage( -1 );
  InsertColumn( 1, col, _T("Channelname") );

  col.SetText( _("# users") );
  col.SetImage( -1 );
  InsertColumn( 2, col, _T("users") );

  m_sortorder[2].col = 0;
  m_sortorder[2].direction = false;
  m_sortorder[0].col = 1;
  m_sortorder[0].direction = true;
  m_sortorder[1].col = 2;
  m_sortorder[1].direction = true;


#if defined(__WXMSW__)
 /// autosize is part-broken on msw.
  SetColumnWidth( 0, 55 );
  SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 1, wxLIST_AUTOSIZE );
#elif defined(__WXMAC__)
/// autosize is entirely broken on wxmac.
  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 128 );
  SetColumnWidth( 2, 128 );
#else
 /// on wxGTK it works, sort of.
  SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 1, wxLIST_AUTOSIZE );
  SetColumnWidth( 2, wxLIST_AUTOSIZE );
#endif

  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );
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
void ChannelListctrl::AddChannel(const wxString& channel, unsigned int num_users )
{
    SetSelectionRestorePoint();

    //todo: don't add if already in it?

    ChannelData data;
    data.num_users = num_users;
    data.name = channel ;
    int index = InsertItem( GetItemCount(), icons().GetHostIcon() );
    SetItem( index, 1, channel );
    SetItem( index, 2, TowxString(num_users) );
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
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareChannelnameUP:&CompareChannelnameDOWN, 0 ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&CompareNumUsersUP:&CompareNumUsersDOWN , 0 ); break;

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
    return  (*(ChannelData*) item1 ).num_users <  (*(ChannelData*) item2 ).num_users;
}

/** @brief CompareNumUsersUP
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareNumUsersUP(long item1, long item2, long sortData)
{
    return  (*(ChannelData*) item1 ).num_users >=  (*(ChannelData*) item2 ).num_users;
}

/** @brief CompareChannelnameDOWN
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareChannelnameDOWN(long item1, long item2, long sortData)
{
    return (*(ChannelData*) item1 ).name.Upper() <  (*(ChannelData*) item2 ).name.Upper();
}

/** @brief CompareChannelnameUP
  *
  * @todo: document this function
  */
int wxCALLBACK ChannelListctrl::CompareChannelnameUP(long item1, long item2, long sortData)
{
    return (*(ChannelData*) item1 ).name.Upper() >=  (*(ChannelData*) item2 ).name.Upper();
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
  wxString chan_name = (*(ChannelData*)event.GetData() ).name;
  ui().JoinChannel( chan_name, _T("") );
}

