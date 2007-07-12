//
// Class: NickListCtrl
//

#include <wx/imaglist.h>
#include <cctype>
#include "nicklistctrl.h"
#include "utils.h"
#include "iconimagelist.h"


NickListCtrl::NickListCtrl( wxWindow* parent )
: wxListCtrl( parent, -1, wxDefaultPosition, wxDefaultSize,
             wxSUNKEN_BORDER | wxLC_REPORT | /*wxLC_VIRTUAL | wxLC_NO_HEADER |*/ wxLC_SINGLE_SEL )
{
  wxListItem col;
  col.SetText( _T("") );
  col.SetImage( -1 );
  InsertColumn( 0, col );
  col.SetText( _T("") );
  col.SetImage( -1 );
  InsertColumn( 1, col );
  col.SetImage( -1 );
  InsertColumn( 2, col );
  col.SetText( _T("Nickname") );
  col.SetImage( ICON_DOWN );
  InsertColumn( 3, col );
#ifdef __WXMSW__
  SetColumnWidth( 0, 45 );
#else
  SetColumnWidth( 0, 20 );
#endif
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 128 );

  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );

}


NickListCtrl::~NickListCtrl()
{

}

void NickListCtrl::AddUser( User& user )
{
  int index = InsertItem(0, IconImageList::GetUserStateIcon( user.GetStatus() ) );
  assert( index != -1 );
  SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 2, IconImageList::GetRankIcon( user.GetStatus().rank ) );
  SetItem( index, 3, WX_STRING(user.GetNick()) );
  SetItemData(index, (long)&user );
  SortItems( NickListSortCallback, 0 );
}

void NickListCtrl::RemoveUser( const User& user )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    if ( &user == (User*)GetItemData( i ) ) {
      DeleteItem( i );
      return;
    }
  }
  debug_error( "Didn't fild the user to remove." );
}


void NickListCtrl::UserUpdated( User& user )
{
}


void NickListCtrl::UserUpdated( const int& index )
{
}


/*
void NickListCtrl::UpdateSize()
{
  assert( m_users != NULL );
  //SetItemCount( m_users->GetNumUsers() );
  //RefreshItems( 0, GetItemCount() );
}

wxString NickListCtrl::OnGetItemText(long item, long column) const
{
  if ( column == 0 ) {
    return _T("");
  }

  if ( m_users == NULL ) return _T("Error: no list");
  if ( item >= m_users->GetNumUsers() ) return _T("Error: index >= max");

  return WX_STRING( m_users->GetUser( item ).GetNick() );

}


int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
  debug_func( "" );

  if ( column == 0 ) {
    if ( m_users == NULL ) return -1;
    if ( item >= m_users->GetNumUsers() ) return -1;
    if (m_users->GetUser( item ).GetStatus().bot ) return 1;
    else if (m_users->GetUser( item ).GetStatus().moderator ) return 0;
  }
  return -1;
}


int NickListCtrl::OnGetItemImage(long item) const
{
  debug_func( "" );

  if ( m_users == NULL ) return ICON_NOSTATE;
  if ( item >= m_users->GetNumUsers() ) return ICON_NOSTATE;

  return GetStateIcon( m_users->GetUser( item ).GetStatus() );
}


wxListItemAttr* NickListCtrl::OnGetItemAttr(long item) const
{
  return NULL;
}

*/
struct upper {
 int operator()(int c)
 {
   return std::toupper((unsigned char)c);
 }
};

int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData)
{
  User* user1 = (User*)item1;
  User* user2 = (User*)item2;
  assert( user1 != NULL );
  assert( user2 != NULL );
  
  int u1 = 0, u2 = 0;
  
  if ( user1->GetStatus().moderator )
    u1 += 1000;
  if ( user2->GetStatus().moderator )
    u2 += 1000;
  if ( user1->GetStatus().bot )
    u1 += 100;
  if ( user2->GetStatus().bot )
    u2 += 100;
  
  std::string nick1 = user1->GetNick();
  std::transform( nick1.begin(), nick1.end(), nick1.begin(), upper() );

  std::string nick2 = user2->GetNick();
  std::transform( nick2.begin(), nick2.end(), nick2.begin(), upper() );

  int cmp = nick1.compare( nick2 );
  if ( cmp < 0 ) u1 += 10;
  else if ( cmp > 0 ) u2 += 10;
  
  if ( u1 > u2 ) return -1;
  if ( u1 < u2 ) return 1;
  return 0;
}

