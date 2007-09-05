/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: NickListCtrl
//

#include <wx/imaglist.h>
#include <cctype>
#include <stdexcept>
#include <wx/menu.h>

#include "nicklistctrl.h"
#include "utils.h"
#include "iconimagelist.h"
#include "user.h"
#include "ui.h"
#include "mainwindow.h"


int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

BEGIN_EVENT_TABLE( NickListCtrl, wxListCtrl )
  EVT_LIST_ITEM_ACTIVATED( NICK_LIST, NickListCtrl::OnActivateItem )
END_EVENT_TABLE()


NickListCtrl::NickListCtrl( wxWindow* parent,Ui& ui, bool show_header ):
  wxListCtrl( parent, NICK_LIST, wxDefaultPosition, wxDefaultSize,
              wxSUNKEN_BORDER | wxLC_REPORT | (int)(!show_header) * wxLC_NO_HEADER | wxLC_SINGLE_SEL ),
  m_ui(ui)
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

	wxMenu* m_user_menu;
	m_user_menu = new wxMenu();
	wxMenuItem* chatitem = new wxMenuItem( m_user_menu, wxID_ANY, wxString( wxT("Open Chat") ) , wxEmptyString, wxITEM_NORMAL );
	m_user_menu->Append( chatitem );
	wxMenuItem* joinbattleitem = new wxMenuItem( m_user_menu, wxID_ANY, wxString( wxT("Join same battle") ) , wxEmptyString, wxITEM_NORMAL );
	m_user_menu->Append( joinbattleitem );

	m_user_menu->AppendSeparator();
	wxMenuItem* slapitem = new wxMenuItem( m_user_menu, wxID_ANY, wxString( wxT("Slap!") ) , wxEmptyString, wxITEM_NORMAL );
	m_user_menu->Append( slapitem );

	m_user_menu->AppendSeparator();
	wxMenu* m_chanserv;
	m_chanserv = new wxMenu();
	wxMenuItem* chmuteitem = new wxMenuItem( m_chanserv, wxID_ANY, wxString( wxT("Mute...") ) , wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chmuteitem );
	wxMenuItem* chkickitem = new wxMenuItem( m_chanserv, wxID_ANY, wxString( wxT("Kick...") ) , wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chkickitem );

	m_chanserv->AppendSeparator();
	wxMenuItem* chopitem = new wxMenuItem( m_chanserv, wxID_ANY, wxString( wxT("Op") ) , wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chopitem );
	wxMenuItem* chdeopitem = new wxMenuItem( m_chanserv, wxID_ANY, wxString( wxT("DeOp") ) , wxEmptyString, wxITEM_NORMAL );
	m_chanserv->Append( chdeopitem );
	m_user_menu->Append( -1, wxT("ChanServ"), m_chanserv );
	wxMenu* m_admin;
	m_admin = new wxMenu();
	wxMenu* m_admin_info;
	m_admin_info = new wxMenu();
	wxMenuItem* admingameitem = new wxMenuItem( m_admin_info, wxID_ANY, wxString( wxT("Ingame time") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin_info->Append( admingameitem );
	wxMenuItem* admlastloginitem = new wxMenuItem( m_admin_info, wxID_ANY, wxString( wxT("Last login") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin_info->Append( admlastloginitem );

	m_admin_info->AppendSeparator();
	wxMenuItem* admipitem = new wxMenuItem( m_admin_info, wxID_ANY, wxString( wxT("Current IP") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin_info->Append( admipitem );
	wxMenuItem* admfindipitem = new wxMenuItem( m_admin_info, wxID_ANY, wxString( wxT("Find IPs") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin_info->Append( admfindipitem );
	m_admin->Append( -1, wxT("Info"), m_admin_info );

	m_admin->AppendSeparator();
	wxMenuItem* admkickitem = new wxMenuItem( m_admin, wxID_ANY, wxString( wxT("Kick...") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin->Append( admkickitem );

	m_admin->AppendSeparator();
	wxMenuItem* admbanitem = new wxMenuItem( m_admin, wxID_ANY, wxString( wxT("Ban...") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin->Append( admbanitem );
	wxMenuItem* admunbanitem = new wxMenuItem( m_admin, wxID_ANY, wxString( wxT("Unban") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin->Append( admunbanitem );

	m_admin->AppendSeparator();
	wxMenuItem* admmuteitem = new wxMenuItem( m_admin, wxID_ANY, wxString( wxT("Mute...") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin->Append( admmuteitem );
	wxMenuItem* admunmuteitem = new wxMenuItem( m_admin, wxID_ANY, wxString( wxT("Unmute") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin->Append( admunmuteitem );

	m_admin->AppendSeparator();
	wxMenuItem* admringitem = new wxMenuItem( m_admin, wxID_ANY, wxString( wxT("Ring") ) , wxEmptyString, wxITEM_NORMAL );
	m_admin->Append( admringitem );
	m_user_menu->Append( -1, wxT("Admin"), m_admin );


}


NickListCtrl::~NickListCtrl()
{

}

void NickListCtrl::AddUser( User& user )
{
  int index = InsertItem( 0, IconImageList::GetUserStateIcon( user.GetStatus() ) );
  SetItemData( index, (wxUIntPtr)&user );
  ASSERT_LOGIC( index != -1, "index = -1" );
  UserUpdated( index );
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
  int index = GetUserIndex( user );
  ASSERT_LOGIC( index != -1, "index = -1" );
  UserUpdated( index );
}


void NickListCtrl::UserUpdated( const int& index )
{
  User& user = *((User*)GetItemData( index ));
  SetItemImage( index, IconImageList::GetUserStateIcon( user.GetStatus() ) );
  SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 2, IconImageList::GetRankIcon( user.GetStatus().rank ) );
  SetItem( index, 3, WX_STRING(user.GetNick()) );
  SetItemData(index, (long)&user );
  SortItems( NickListSortCallback, 0 );
}


void NickListCtrl::ClearUsers()
{
  while ( GetItemCount() > 0 ) {
    DeleteItem( 0 );
  }
}


int NickListCtrl::GetUserIndex( User& user )
{
  for ( int i = 0; i < GetItemCount() ; i++ ) {
    if ( (unsigned long)&user == GetItemData( i ) ) return i;
  }
  debug_error( "didn't find the user." );
  return -1;
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
  ASSERT_LOGIC( user1 != 0, "user1 = 0" );
  ASSERT_LOGIC( user2 != 0, "user2 = 0" );

  int u1 = 0, u2 = 0;

  if ( user1->GetStatus().moderator )
    u1 += 1000;
  if ( user2->GetStatus().moderator )
    u2 += 1000;
  if ( user1->GetStatus().bot )
    u1 += 100;
  if ( user2->GetStatus().bot )
    u2 += 100;

  wxString nick1 = WX_STRING(user1->GetNick());
  wxString nick2 = WX_STRING(user2->GetNick());

  int cmp = nick1.CmpNoCase( nick2 );
  if ( cmp < 0 ) u1 += 10;
  else if ( cmp > 0 ) u2 += 10;

  if ( u1 > u2 ) return -1;
  if ( u1 < u2 ) return 1;
  return 0;
}


void NickListCtrl::OnActivateItem( wxListEvent& event )
{
  int index = event.GetIndex();
  if ( index == -1 ) return;
  User* user = (User*)event.GetData();
  m_ui.mw().OpenPrivateChat( *user );
}

