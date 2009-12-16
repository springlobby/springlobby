#include "userlistctrl.h"
#include "ui.h"
#include "tasserver.h"
#include "user.h"
#include "iconimagelist.h"

BEGIN_EVENT_TABLE( UserListctrl, CustomListCtrl )
  EVT_LIST_COL_CLICK( USERLIST, UserListctrl::OnColClick )
END_EVENT_TABLE()

UserListctrl::UserListctrl(wxWindow* parent, const wxString& name, bool highlight,wxWindowID id)
  : CustomListCtrl( parent,id,wxDefaultPosition, wxDefaultSize,
        wxSUNKEN_BORDER | wxLC_REPORT, name, 2, highlight )
{
    wxListItem col;

    col.SetImage( -1 );
    InsertColumn( 0, col, _T("Country") );

    col.SetText( _("Nickname") );
    col.SetImage( -1 );
    InsertColumn( 1, col, _T("Nickname") );

    m_sortorder[0].col = 0;
    m_sortorder[0].direction = false;
    m_sortorder[1].col = 1;
    m_sortorder[1].direction = true;

    SetColumnWidths();

}

void UserListctrl::SetColumnWidths()
{
    #if defined(__WXMAC__)
    // autosize is entirely broken on wxmac.
      SetColumnWidth( 0, 20 );
      SetColumnWidth( 1, 228 );
    #else
     // on wxGTK it works, sort of.
      SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
      SetColumnWidth( 1, 160 );
    #endif
}

UserListctrl::~UserListctrl()
{
    //dtor
}

void UserListctrl::AddUser( const UserDataMap& userdata )
{
    for ( UserDataMapConstIter it = userdata.begin(); it != userdata.end(); ++it)
    {
        AddUser( it->second );
    }
    //SetColumnWidths();
}

const UserListctrl::UserDataMap& UserListctrl::GetUserData() const
{
    return m_userdata;
}

void UserListctrl::AddUser( const UserData userdata )
{
    SetSelectionRestorePoint();

    if ( IsInList ( userdata ) )
        return;

    int index = InsertItem( GetItemCount(), icons().GetFlagIcon( userdata.second ) );
    SetItem( index, 1, userdata.first );

    //highlight
    HighlightItemUser( index, userdata.first );
    m_userdata[index] = userdata;
    SetItemData(index, (long)&m_userdata[index] );

    Sort();
    RestoreSelection();

}


void UserListctrl::Sort()
{
  for (int i = 1; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayercountryUP:&ComparePlayercountryDOWN , 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayernameUP:&ComparePlayernameDOWN , 0 ); break;
      default: break;
    }
  }
}

void UserListctrl::SetTipWindowText( const long /*unused*/, const wxPoint /*unused*/){}

wxArrayString UserListctrl::GetUserNicks( ) const
{
    wxArrayString ret;
    for ( UserDataMapConstIter it = m_userdata.begin(); it != m_userdata.end(); ++it)
    {
        ret.Add( (*it).second.first );
    }
    return ret;
}

UserListctrl::UserDataMap UserListctrl::GetSelectedUserData() const
{
    UserDataMap ret;
    long item = -1;
	for ( long i = 0; i < GetSelectedItemCount(); ++i )
	{
		item = GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
		if ( item == -1 ) // means nothing was found
            return ret;
        UserData data = *((UserData*) GetItemData( item ));
        ret[item] = data;
	}

	return ret;
}

void UserListctrl::RemoveUsers( const UserDataMap& userdata )
{
    std::vector<UserDataMapIter> todelete;
    for ( UserDataMapConstIter it = userdata.begin(); it != userdata.end(); ++it)
    {
        UserDataMapIter data = FindData( it->second );
        if ( data != m_userdata.end() )
        {
            todelete.push_back( data );
        }
    }
    std::vector<UserDataMapIter>::const_iterator it = todelete.begin();
    for( ; it != todelete.end(); ++it)
        m_userdata.erase( (*it) );

    UserDataMap tmp = m_userdata;
    m_userdata.clear();
    DeleteAllItems();
    AddUser( tmp );
    SelectNone();


}

UserListctrl::UserDataMapIter UserListctrl::FindData( const UserData userdata )
{
    for ( UserDataMapIter it = m_userdata.begin(); it != m_userdata.end(); ++it)
    {
        if ( it->second.first == userdata.first )
            return it;
    }
    return m_userdata.end();
}

bool UserListctrl::IsInList( const UserData userdata )
{
    UserDataMapConstIter it = FindData( userdata );
    return ( it != m_userdata.end() );
}

void UserListctrl::HighlightItem( long /*item */)
{

}

int wxCALLBACK UserListctrl::ComparePlayercountryDOWN(long item1, long item2, long /*unused*/)
{
    return ( (UserData*) item1)->first.Upper().Cmp( ( (UserData*) item2)->first.Upper() ) ;
}

int wxCALLBACK UserListctrl::ComparePlayercountryUP(long item1, long item2, long /*unused*/)
{
    return ( (UserData*) item2)->first.Upper().Cmp( ( (UserData*) item1)->first.Upper() ) ;
}

int wxCALLBACK UserListctrl::ComparePlayernameDOWN(long item1, long item2, long /*unused*/)
{
    return ( (UserData*) item1)->second.Upper().Cmp( ( (UserData*) item2)->second.Upper() ) ;
}

int wxCALLBACK UserListctrl::ComparePlayernameUP(long item1, long item2, long /*unused*/)
{
    return ( (UserData*) item2)->second.Upper().Cmp( ( (UserData*) item1)->second.Upper()) ;
}

void UserListctrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( -1 );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 2; ++i ) {}
  if (i > 1) { i = 1; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}

void UserListctrl::Clear()
{
    DeleteAllItems();
    m_userdata.clear();
    SelectNone();
}
