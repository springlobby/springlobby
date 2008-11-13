/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: NickListCtrl
//

#include <wx/platform.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <stdexcept>
#include <wx/log.h>

#include "nicklistctrl.h"
#include "utils.h"
#include "iconimagelist.h"
#include "user.h"
#include "settings.h"
#include "ui.h"
#include "mainwindow.h"
#include "countrycodes.h"
#include "chatpanel.h"
#include "userlist.h"
#include "usermenu.h"


int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

BEGIN_EVENT_TABLE( NickListCtrl, CustomListCtrl )
  EVT_LIST_ITEM_ACTIVATED( NICK_LIST, NickListCtrl::OnActivateItem )
  EVT_LIST_COL_CLICK( NICK_LIST, NickListCtrl::OnColClick )
  EVT_CONTEXT_MENU( NickListCtrl::OnShowMenu )
#if wxUSE_TIPWINDOW
#ifndef __WXMSW__ //disables tooltips on win and mac
  EVT_MOTION(NickListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()


NickListCtrl::NickListCtrl( wxWindow* parent, bool show_header, NickListCtrl::UserMenu* popup, bool singleSelectList,
                            const wxString& name, bool highlight):
  CustomListCtrl( parent, NICK_LIST, wxDefaultPosition, wxDefaultSize,
              wxSUNKEN_BORDER | wxLC_REPORT | (int)(!show_header) * wxLC_NO_HEADER | (int)(singleSelectList) * wxLC_SINGLE_SEL,
              name, highlight ),
  m_menu(popup)
{
  wxListItem col;
  col.SetText( _("s") );
  col.SetImage( -1 );
  InsertColumn( 0, col, _T("Status") );
  col.SetText( _("c") );
  col.SetImage( -1 );
  InsertColumn( 1, col, _T("Country") );
  col.SetText( _("r") );
  col.SetImage( -1 );
  InsertColumn( 2, col, _T("Rank") );
  col.SetText( _("Nickname") );
  col.SetImage( icons().ICON_DOWN );
  InsertColumn( 3, col, _T("Nickname") );

  m_sortorder[0].col = 0;
  m_sortorder[0].direction = false;
  m_sortorder[1].col = 3;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 2;
  m_sortorder[2].direction = true;
  m_sortorder[3].col = 1;
  m_sortorder[3].direction = true;

  long width = GetSize().x -( GetColumnWidth( 0 ) + GetColumnWidth( 1 ) + GetColumnWidth( 2 ) );
#if defined(__WXMAC__)
/// autosize is entirely broken on wxmac.
  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, width );
#else
 /// on wxGTK it works, sort of.
  SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 3, width );
#endif

}


NickListCtrl::~NickListCtrl()
{

}

void NickListCtrl::AddUser( const UserList& userlist )
{
    for ( unsigned int i = 0; i < userlist.GetNumUsers(); ++i)
    {
        AddUser( userlist.GetUser( i ) );
    }
}

void NickListCtrl::AddUser( const User& user )
{
  wxLogDebugFunc(_T(""));
  assert(&user);

  int index = InsertItem( GetItemCount(), icons().GetUserListStateIcon( user.GetStatus(), false, user.GetBattle() != 0 ) );
  if(index==-1){
    wxLogMessage(_T("NickListCtrl::AddUser : index==-1"));
    return;
  }

  SetItemData( index, (wxUIntPtr)&user );
  try
  {
  ASSERT_LOGIC( index != -1, _T("index = -1") );
  } catch (...) { return; }

  UserUpdated( index );
  SetColumnWidth( 3, wxLIST_AUTOSIZE );
  MarkDirtySort();
}

void NickListCtrl::RemoveUser( const User& user )
{
  int index = GetUserIndex( user );
  if ( index != -1 )
  {
    DeleteItem( index );
    SetColumnWidth( 3, wxLIST_AUTOSIZE );
    return;
  }
  wxLogError( _T("Didn't find the user to remove.") );
}


void NickListCtrl::UserUpdated( User& user )
{
  int index = GetUserIndex( user );
  //ASSERT_LOGIC( index != -1, _T("index = -1") );
  if(index!=-1){
    UserUpdated( index );
  }else{
    wxLogWarning(_T("NickListCtrl::UserUpdated error, index == -1 ."));
  }
}


void NickListCtrl::UserUpdated( const int& index )
{
  User& user = *((User*)GetItemData( index ));
  const UserStatus user_st = user.GetStatus();
  SetItemImage( index, icons().GetUserListStateIcon( user_st, false, user.GetBattle() != 0 ) );
  SetItemColumnImage( index, 1, icons().GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 2, icons().GetRankIcon( user.GetStatus().rank ) );
  SetItem( index, 3, user.GetNick() );
  SetItemData(index, (long)&user );
  //highlight
  HighlightItemUser( index, user.GetNick() );
  MarkDirtySort();
}


void NickListCtrl::ClearUsers()
{
  while ( GetItemCount() > 0 ) {
    DeleteItem( 0 );
  }
}


int NickListCtrl::GetUserIndex( const User& user )const
{
  for ( int i = 0; i < GetItemCount() ; i++ ) {
    if ( (unsigned long)&user == GetItemData( i ) ) return i;
  }
  wxLogError( _T("didn't find the user.") );
  return -1;
}


void NickListCtrl::OnActivateItem( wxListEvent& event )
{
  int index = event.GetIndex();
  if ( index == -1 ) return;
  User* user = (User*)event.GetData();
  ui().mw().OpenPrivateChat( *user );
  //FIXME why was this lfet here, what was it supposed to do?
  //m_ui.mw().OnTabsChanged( event2 );
}


void NickListCtrl::OnShowMenu( wxContextMenuEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_menu != 0 )
  {
      //no need to popup the menu when there's no user selected
      if ( GetSelectedIndex() != -1 ){
          m_menu->EnableItems( (GetSelectedIndex()!=-1), _("JK") );
          PopupMenu( m_menu );
      }
  }
}

void NickListCtrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( -1 );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}
  if (i > 3) { i = 3; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );
  Sort();
}


void NickListCtrl::Sort()
{
  for (int i = 3; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayerstatusUP:&ComparePlayerstatusDOWN , 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayercountryUP:&ComparePlayercountryDOWN , 0 ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayerrankUP:&ComparePlayerrankDOWN , 0 ); break;
      case 3 : SortItems( ( m_sortorder[ i ].direction )?&ComparePlayernameUP:&ComparePlayernameDOWN , 0 ); break;
    }
  }
}


int wxCALLBACK NickListCtrl::ComparePlayernameUP(long item1, long item2, long sortData )
{
    return ((User*)item1)->GetNick().CmpNoCase(((User*)item2)->GetNick());
}


int wxCALLBACK NickListCtrl::ComparePlayernameDOWN(long item1, long item2, long sortData )
{
    // inverse the order
    return ((User*)item2)->GetNick().CmpNoCase(((User*)item1)->GetNick());
}


int wxCALLBACK NickListCtrl::ComparePlayerstatusUP(long item1, long item2, long sortData )
{
  User* user1 = (User*)item1;
  User* user2 = (User*)item2;
  ASSERT_LOGIC( user1 != 0, _T("user1 = 0") );
  ASSERT_LOGIC( user2 != 0, _T("user2 = 0") );

  int u1 = 0, u2 = 0;

  if ( user1->GetStatus().bot )
    u1 += 1000;
  if ( user2->GetStatus().bot )
    u2 += 1000;
  if ( user1->GetStatus().moderator )
    u1 += 100;
  if ( user2->GetStatus().moderator )
    u2 += 100;
  if ( user1->GetStatus().in_game )
    u1 += -10;
  if ( user2->GetStatus().in_game )
    u2 += -10;

    // inverse the order
    if ( u1 < u2 )
        return -1;
    if ( u1 > u2 )
        return 1;

    return 0;
}

int wxCALLBACK NickListCtrl::ComparePlayerstatusDOWN(long item1, long item2, long sortData )
{
  User* user1 = (User*)item1;
  User* user2 = (User*)item2;
  ASSERT_LOGIC( user1 != 0, _T("user1 = 0") );
  ASSERT_LOGIC( user2 != 0, _T("user2 = 0") );

  int u1 = 0, u2 = 0;

  if ( user1->GetStatus().bot )
    u1 += 1000;
  if ( user2->GetStatus().bot )
    u2 += 1000;
  if ( user1->GetStatus().moderator )
    u1 += 100;
  if ( user2->GetStatus().moderator )
    u2 += 100;
  if ( user1->GetStatus().in_game )
    u1 += -10;
  if ( user2->GetStatus().in_game )
    u2 += -10;


    // inverse the order
    if ( u1 < u2 )
        return 1;
    if ( u1 > u2 )
        return -1;

    return 0;
}


int wxCALLBACK NickListCtrl::ComparePlayerrankUP(long item1, long item2, long sortData )
{
    // inverse the order

    if ( ((User*)item1)->GetStatus().rank < ((User*)item2)->GetStatus().rank )
        return -1;
    if ( ((User*)item1)->GetStatus().rank > ((User*)item2)->GetStatus().rank )
        return 1;

    return 0;
}

int wxCALLBACK NickListCtrl::ComparePlayerrankDOWN(long item1, long item2, long sortData )
{
    // inverse the order
    if ( ((User*)item1)->GetStatus().rank < ((User*)item2)->GetStatus().rank )
        return 1;
    if ( ((User*)item1)->GetStatus().rank > ((User*)item2)->GetStatus().rank )
        return -1;

    return 0;
}


int wxCALLBACK NickListCtrl::ComparePlayercountryUP(long item1, long item2, long sortData )
{
    return ((User*)item1)->GetCountry().CmpNoCase(((User*)item2)->GetCountry());
}

int wxCALLBACK NickListCtrl::ComparePlayercountryDOWN(long item1, long item2, long sortData )
{
    // inverse the order
    return ((User*)item2)->GetCountry().CmpNoCase(((User*)item1)->GetCountry());
}

void NickListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
    User* user = (User*) GetItemData(item_hit);
    int coloumn = getColoumnFromPosition(position);
    if (coloumn > (int)m_colinfovec.size() || coloumn < 0)
    {
        m_tiptext = _T("");
    }
    else
    {
        if (user) {
            switch (coloumn)
            {
            case 0: // status
                m_tiptext = _T("This ");
                if (user->GetStatus().bot)
                    m_tiptext << _T("bot ");
                else if (user->GetStatus().moderator)
                    m_tiptext << _T("moderator ");
                else
                    m_tiptext << _T("player ");

                if (user->GetStatus().in_game)
                    m_tiptext <<  _T("is ingame");
                else if (user->GetStatus().away)
                    m_tiptext <<  _T("is away");
                else
                    m_tiptext << _T("is available");
                break;

            case 1: // country
                m_tiptext =  GetFlagNameFromCountryCode(user->GetCountry().Upper());
                break;

            case 2: // rank
                m_tiptext = user->GetRankName(user->GetStatus().rank);
                break;

            case 3: // nickname
                m_tiptext = user->GetNick();
                break;

            default:
                m_tiptext = m_colinfovec[coloumn].first;
                break;
            }
        }
        else {
            m_tiptext = _T("");
        }
    }
}

void NickListCtrl::HighlightItem( long item )
{
    User* u = (User*)GetItemData( item ) ;
    if ( u != 0 ) {
        wxString name = u->GetNick();
        HighlightItemUser( item, name );
    }
}


void NickListCtrl::SortList()
{
  if ( !m_dirty_sort ) return;
  SetSelectionRestorePoint();
  Sort();
  RestoreSelection();
  m_dirty_sort = false;
}
