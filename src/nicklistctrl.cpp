//
// Class: NickListCtrl
//

#include <wx/imaglist.h>
#include <cctype>
#include "nicklistctrl.h"
#include "utils.h"

#include "images/bot.xpm"
#include "images/admin.xpm"
#include "images/admin_ingame.xpm"
#include "images/admin_away.xpm"
#include "images/away.xpm"
#include "images/ingame.xpm"
#include "images/up.xpm"
#include "images/down.xpm"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"

#include "flagimages.h"

static wxImageList* _imagelist = NULL;
static int _imagelist_users = 0;

NickListCtrl::NickListCtrl( wxWindow* parent )
: wxListCtrl( parent, -1, wxDefaultPosition, wxDefaultSize,
             wxSUNKEN_BORDER | wxLC_REPORT | /*wxLC_VIRTUAL | wxLC_NO_HEADER |*/ wxLC_SINGLE_SEL )
{
  wxListItem col;
  col.SetText( _("") );
  col.SetImage( -1 );
  InsertColumn( 0, col );
  col.SetText( _("") );
  col.SetImage( -1 );
  InsertColumn( 1, col );
  col.SetImage( -1 );
  InsertColumn( 2, col );
  col.SetText( _("Nickname") );
  col.SetImage( ICON_DOWN );
  InsertColumn( 3, col );
  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 128 );

  if ( _imagelist == NULL ) {
    _imagelist = new wxImageList( 16, 16 );
    _imagelist->Add( wxBITMAP(admin) );
    _imagelist->Add( wxBITMAP(admin_away) );
    _imagelist->Add( wxBITMAP(admin_ingame) );
    _imagelist->Add( wxBITMAP(bot) );
    _imagelist->Add( wxBITMAP(away) );
    _imagelist->Add( wxBITMAP(ingame) );
    _imagelist->Add( wxBITMAP(up) );
    _imagelist->Add( wxBITMAP(down) );
    _imagelist->Add( wxBITMAP(rank0) );
    _imagelist->Add( wxBITMAP(rank1) );
    _imagelist->Add( wxBITMAP(rank2) );
    _imagelist->Add( wxBITMAP(rank3) );
    _imagelist->Add( wxBITMAP(rank4) );

    ADD_FLAGS_IMAGES(_imagelist)
  }
  _imagelist_users++;
  SetImageList( _imagelist, wxIMAGE_LIST_NORMAL );
  SetImageList( _imagelist, wxIMAGE_LIST_SMALL );
  SetImageList( _imagelist, wxIMAGE_LIST_STATE );

}


NickListCtrl::~NickListCtrl()
{
  _imagelist_users--;
  if ( _imagelist_users == 0 )
    delete _imagelist;
}

void NickListCtrl::AddUser( User& user )
{
  int index = InsertItem(0, GetStateIcon( user.GetStatus() ) );
  assert( index != -1 );
  SetItemColumnImage( index, 1, ICON_FLAGS_BASE + GetFlagIndex( user.GetCountry() ) );
  SetItemColumnImage( index, 2, ICON_RANK0 + GetRankIcon( user.GetStatus() ) );
  SetItem( index, 3, WX_STRING(user.GetNick()) );
  SetItemData(index, (long)&user );
  SortItems( NickListSortCallback, 0 );
}

void NickListCtrl::RemoveUser( const User& user )
{
  bool found = true;
  wxListItem item;
  int index = -1;
  while ( true ) {
    index++;
    item.SetId( index );
    found = GetItem( item );
    if (!found) return;
      
    item.SetColumn( 3 );
    if ( item.GetText() == WX_STRING(user.GetNick() ) ) {
      DeleteItem( index );
      return;
    }
    
    if ( index > 1000000 ) assert( false ); // Just in case :)
    
  }
}

void NickListCtrl::UpdateSize()
{
  assert( m_users != NULL );
  //SetItemCount( m_users->GetNumUsers() );
  //RefreshItems( 0, GetItemCount() );
}

wxString NickListCtrl::OnGetItemText(long item, long column) const
{
  if ( column == 0 ) {
    return _("");
  }

  if ( m_users == NULL ) return _("Error: no list");
  if ( item >= m_users->GetNumUsers() ) return _("Error: index >= max");

  return WX_STRING( m_users->GetUser( item ).GetNick() );

}


int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
  std::cout << "** NickListCtrl::OnGetItemColumnImage()" << std::endl;

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
  std::cout << "** NickListCtrl::OnGetItemImage()" << std::endl;

  if ( m_users == NULL ) return ICON_NOSTATE;
  if ( item >= m_users->GetNumUsers() ) return ICON_NOSTATE;

  return GetStateIcon( m_users->GetUser( item ).GetStatus() );
}


wxListItemAttr* NickListCtrl::OnGetItemAttr(long item) const
{
  return NULL;
}

int NickListCtrl::GetStateIcon( const UserStatus& us ) const
{
  if ( us.bot ) return ICON_BOT;
  if (us.moderator ) {
    if ( us.in_game ) return ICON_ADMIN_INGAME;
    if ( us.away ) return ICON_ADMIN_AWAY;
    return ICON_ADMIN;
  }
  
  if ( us.in_game ) return ICON_INGAME;
  if ( us.away ) return ICON_AWAY;
  
  return ICON_NOSTATE;
}

int NickListCtrl::GetRankIcon( const UserStatus& us ) const
{
  if ( us.rank <= RANK_0 ) return 0;
  if ( us.rank <= RANK_1 ) return 1;
  if ( us.rank <= RANK_2 ) return 2;
  if ( us.rank <= RANK_3 ) return 3;
  return 4;
}

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

