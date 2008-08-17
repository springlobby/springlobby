#include "userlistctrl.h"
#include "ui.h"
#include "tasserver.h"
#include "user.h"
#include "iconimagelist.h"


UserListctrl::UserListctrl(wxWindow* parent, const wxString& name, bool highlight)
  : customListCtrl( parent,wxID_ANY,wxDefaultPosition, wxDefaultSize,
        wxSUNKEN_BORDER | wxLC_REPORT, name, highlight )
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

#if defined(__WXMAC__)
/// autosize is entirely broken on wxmac.
  SetColumnWidth( 0, 20 );
  SetColumnWidth( 3, 128 );
#else
 /// on wxGTK it works, sort of.
  SetColumnWidth( 0, wxLIST_AUTOSIZE );
  SetColumnWidth( 1, wxLIST_AUTOSIZE );
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
    SetItemData(index, index );
    //highlight
    HighlightItemUser( index, userdata.first );
    m_userdata[index] = userdata;

    Sort();
    RestoreSelection();

}


void UserListctrl::Sort(){}

void UserListctrl::SetTipWindowText( const long item_hit, const wxPoint position){}

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
        UserDataMapConstIter el = m_userdata.find(item);
        if ( el != m_userdata.end() )
            ret[item] = ( el->second );
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

void UserListctrl::HighlightItem( long item )
{
//    User* u = (User*)GetItemData( item ) ;
//    if ( u != 0 ) {
//        wxString name = u->GetNick();
//        HighlightItemUser( item, name );
//    }
}
