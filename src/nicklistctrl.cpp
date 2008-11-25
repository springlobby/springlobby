/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
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
#include <algorithm>

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
#include "Helper/sortutil.h"

typedef CompareBase<const User*>  UserCompareBase;

template < int N >
struct UserCompare : public UserCompareBase {
    static int compare ( CompareType u1, CompareType u2, int dir ) {
        //assert(0);//this case should never be actually be called, but is necessary to be defined at compile time
        return 0;
    }
};


template < >
struct UserCompare < 3 > : public UserCompareBase
{
    static int compare ( CompareType u1, CompareType u2, int dir ) {
        return dir * u2->GetNick().CmpNoCase( u1->GetNick() ) ;
    }

};

template < >
struct UserCompare < 2 > : public UserCompareBase
{
    static int compare ( CompareType u1, CompareType u2, int dir ) {
        return dir * compareSimple( u2->GetStatus().rank, u1->GetStatus().rank );
    }
};

template < >
struct UserCompare < 1 > : public UserCompareBase
{
    static int compare ( CompareType u1, CompareType u2, int dir ) {
        return dir * u2->GetCountry().CmpNoCase( u1->GetCountry() );
    }
};

template < typename Type >
int compareSimple( Type o1, Type o2 ) {
    if ( o1 < o2 )
        return -1;
    else if ( o1 > o2 )
        return 1;
    return 0;
}

int CompareOneCrit( const User* u1, const User* u2, int col, int dir )
{
    switch ( col ) {
        case 1: return dir * u2->GetCountry().CmpNoCase( u1->GetCountry() );
        case 2: return dir * compareSimple( u2->GetStatus().rank, u1->GetStatus().rank );
        case 3: return dir * u2->GetNick().CmpNoCase( u1->GetNick() ) ;
        default: return 0;
    }
}

bool CompareUsers( const User* u1, const User* u2, SortOrder& order)
{
    int res = CompareOneCrit( u1, u2, order[0].col, order[0].direction );
    if ( res != 0 )
        return res < 0;

    if ( order[1].direction != 0 ) {
        res = CompareOneCrit( u1, u2, order[1].col, order[1].direction );
        if ( res != 0 )
            return res < 0;

        if ( order[2].direction != 0 ) {
            res = CompareOneCrit( u1, u2, order[2].col, order[2].direction );
            if ( res != 0 )
                return res < 0;
        }
    }
    return false;
}

BEGIN_EVENT_TABLE( NickListCtrl, CustomVirtListCtrl )
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
  CustomVirtListCtrl( parent, NICK_LIST, wxDefaultPosition, wxDefaultSize,
              wxLC_VIRTUAL | wxSUNKEN_BORDER | wxLC_REPORT | (int)(!show_header) * wxLC_NO_HEADER | (int)(singleSelectList) * wxLC_SINGLE_SEL,
              name, highlight ),
  m_menu(popup)
{

#if defined(__WXMAC__)
    const int widths [4] = { 20,20,20,120 };
#else
    const int widths [4] = { 20,20,20,120 };
#endif

  wxListItem col;
  col.SetText( _("s") );
  col.SetImage( -1 );
  AddColumn( 0, widths[0], _("s"), _T("Status") );
  col.SetText( _("c") );
  col.SetImage( -1 );
  AddColumn( 1, widths[1], _("c"), _T("Country") );
  col.SetText( _("r") );
  col.SetImage( -1 );
  AddColumn( 2, widths[2], _("r"), _T("Rank") );
  col.SetText( _("Nickname") );
  col.SetImage( icons().ICON_DOWN );
  AddColumn( 3, widths[3], _("Nickname"), _T("Nickname") );


  m_sortorder[0].col = 0;
  m_sortorder[0].direction = -1;
  m_sortorder[1].col = 3;
  m_sortorder[1].direction = 1;
  m_sortorder[2].col = 2;
  m_sortorder[2].direction = 1;
  m_sortorder[3].col = 1;
  m_sortorder[3].direction = 1;

    m_compare_func = &CompareUsers;

}


NickListCtrl::~NickListCtrl()
{

}

//void NickListCtrl::AddUser( const UserList& userlist )
//{
//    m_data.reserve( userlist.GetNumUsers() );
//    for ( unsigned int i = 0; i < userlist.GetNumUsers(); ++i)
//    {
//        AddUser( userlist.GetUser( i ) );
//    }
//}

void NickListCtrl::AddUser( const User& user )
{
    wxLogDebugFunc(_T(""));
    assert(&user);

//  int index = InsertItem( GetItemCount(), wxEmptyString );
//  if(index==-1){
//    wxLogMessage(_T("NickListCtrl::AddUser : index==-1"));
//    return;
//  }

    m_data.push_back( &user );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() );

//  try
//  {
//  ASSERT_LOGIC( index != -1, _T("index = -1") );
//  } catch (...) { return; }


    SetColumnWidth( 3, wxLIST_AUTOSIZE );
    SetColumnWidth( 0, wxLIST_AUTOSIZE );
    MarkDirtySort();
}

void NickListCtrl::RemoveUser( const User& user )
{
  int index = GetUserIndex( user );

  if ( index != -1 )
  {
    //erase
    m_data.erase( m_data.begin() + index );
    SetItemCount( m_data.size() );
    SetColumnWidth( 3, wxLIST_AUTOSIZE );
    return;
  }
  wxLogError( _T("Didn't find the user to remove.") );
}


void NickListCtrl::UserUpdated( const User& user )
{
    int index = GetUserIndex( user );
    if ( index != -1 ) {
        m_data[index] = &user;
        HighlightItemUser( index, user.GetNick() );
        MarkDirtySort();
        RefreshItem( index );
    }
    else {
        wxLogWarning(_T("NickListCtrl::UserUpdated error, index == -1 ."));
    }
}

void NickListCtrl::ClearUsers()
{
    m_data.clear();
    SetItemCount( 0 );
}


int NickListCtrl::GetUserIndex( const User& user )const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it, ++i ) {
        if ( *it != 0 && user.Equals( *(*it) ) ) return i;
    }
    wxLogError( _T("didn't find the user.") );
    return -1;
}


void NickListCtrl::OnActivateItem( wxListEvent& event )
{
  int index = event.GetIndex();
  if ( index == -1 )
    return;

  const User* user = m_data[index];
  if ( user ) {
    ui().mw().OpenPrivateChat( *user );
  }
  SetSelectedIndex( index );
}


void NickListCtrl::OnShowMenu( wxContextMenuEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_menu != 0 )
  {
      //no need to popup the menu when there's no user selected
      int selected = GetSelectedIndex();
      if ( selected != -1 && m_data[selected] ){
          const User& user = *m_data[selected];
          wxString nick = user.GetNick();
          m_menu->EnableItems( ( selected !=-1 ), nick );
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
  m_sortorder[0].direction *= -1;


  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

//  m_compare_func = CompareSelector<UserCompare>::
//                                GetFunctor( m_sortorder[0].col,m_sortorder[1].col,m_sortorder[2].col );

  Sort();
}

//
//int wxCALLBACK NickListCtrl::ComparePlayerstatusUP(long item1, long item2, long sortData )
//{
//  User* user1 = (User*)item1;
//  User* user2 = (User*)item2;
//  ASSERT_LOGIC( user1 != 0, _T("user1 = 0") );
//  ASSERT_LOGIC( user2 != 0, _T("user2 = 0") );
//
//  int u1 = 0, u2 = 0;
//
//  if ( user1->GetStatus().bot )
//    u1 += 1000;
//  if ( user2->GetStatus().bot )
//    u2 += 1000;
//  if ( user1->GetStatus().moderator )
//    u1 += 100;
//  if ( user2->GetStatus().moderator )
//    u2 += 100;
//  if ( user1->GetStatus().in_game )
//    u1 += -10;
//  if ( user2->GetStatus().in_game )
//    u2 += -10;
//
//    // inverse the order
//    if ( u1 < u2 )
//        return -1;
//    if ( u1 > u2 )
//        return 1;
//
//    return 0;
//}
//
//int wxCALLBACK NickListCtrl::ComparePlayerstatusDOWN(long item1, long item2, long sortData )
//{
//  User* user1 = (User*)item1;
//  User* user2 = (User*)item2;
//  ASSERT_LOGIC( user1 != 0, _T("user1 = 0") );
//  ASSERT_LOGIC( user2 != 0, _T("user2 = 0") );
//
//  int u1 = 0, u2 = 0;
//
//  if ( user1->GetStatus().bot )
//    u1 += 1000;
//  if ( user2->GetStatus().bot )
//    u2 += 1000;
//  if ( user1->GetStatus().moderator )
//    u1 += 100;
//  if ( user2->GetStatus().moderator )
//    u2 += 100;
//  if ( user1->GetStatus().in_game )
//    u1 += -10;
//  if ( user2->GetStatus().in_game )
//    u2 += -10;
//
//
//    // inverse the order
//    if ( u1 < u2 )
//        return 1;
//    if ( u1 > u2 )
//        return -1;
//
//    return 0;
//}
//

void NickListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{

    int coloumn = getColoumnFromPosition(position);
    if (coloumn > (int)m_colinfovec.size() || coloumn < 0 || item_hit < 0 || item_hit > m_data.size() || m_data[item_hit]==NULL )
    {
        m_tiptext = _T("");
    }
    else
    {
        const User& user = *m_data[item_hit];
        {
            switch (coloumn)
            {
            case 0: // status
                m_tiptext = _T("This ");
                if (user.GetStatus().bot)
                    m_tiptext << _T("bot ");
                else if (user.GetStatus().moderator)
                    m_tiptext << _T("moderator ");
                else
                    m_tiptext << _T("player ");

                if (user.GetStatus().in_game)
                    m_tiptext <<  _T("is ingame");
                else if (user.GetStatus().away)
                    m_tiptext <<  _T("is away");
                else
                    m_tiptext << _T("is available");
                break;

            case 1: // country
                m_tiptext =  GetFlagNameFromCountryCode(user.GetCountry().Upper());
                break;

            case 2: // rank
                m_tiptext = user.GetRankName(user.GetStatus().rank);
                break;

            case 3: // nickname
                m_tiptext = user.GetNick();
                break;

            default:
                m_tiptext = m_colinfovec[coloumn].first;
                break;
            }
        }
    }
}

void NickListCtrl::HighlightItem( long item )
{
    if ( m_data[item] ) {
        const User& u = *m_data[item];
        wxString name = u.GetNick();
        HighlightItemUser( item, name );
    }
}

void NickListCtrl::SortList()
{
  if ( !m_dirty_sort ) return;
//  SetSelectionRestorePoint();
    Freeze();
    Sort();
    Thaw();

}


void NickListCtrl::Sort()
{
    if ( m_data.size() > 0 )
    {

       SLInsertionSort( m_data, m_compare_func
            , m_sortorder );

        RefreshItems(0, m_data.size()-1 );
        m_dirty_sort = false;

    }
}

wxString NickListCtrl::OnGetItemText(long item, long column) const
{
    switch ( column ) {
        case 0:
        case 1:
        case 2:
        default: return wxEmptyString;

        case 3: return ( m_data[item] ? m_data[item]->GetNick() : wxString() );
    }
}

int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if ( m_data[item] ) {
        const User& user = *m_data[item];
        const UserStatus& user_st = user.GetStatus();
        switch ( column ) {
            case 0: return icons().GetUserListStateIcon( user_st, false, user.GetBattle() != 0 );
            case 1: return icons().GetFlagIcon( user.GetCountry() );
            case 2: return icons().GetRankIcon( user.GetStatus().rank );

            case 3:
            default: return -1;

        }
    }
    return -1;
}

int NickListCtrl::OnGetItemImage(long item) const
{
    return -1;
}
