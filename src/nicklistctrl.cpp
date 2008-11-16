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


int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

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
  m_sortorder[0].direction = false;
  m_sortorder[1].col = 3;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 2;
  m_sortorder[2].direction = true;
  m_sortorder[3].col = 1;
  m_sortorder[3].direction = true;

}


NickListCtrl::~NickListCtrl()
{

}

void NickListCtrl::AddUser( const UserList& userlist )
{
    m_data.reserve( userlist.GetNumUsers() );
    for ( unsigned int i = 0; i < userlist.GetNumUsers(); ++i)
    {
        AddUser( userlist.GetUser( i ) );
    }
}

void NickListCtrl::AddUser( const User& user )
{
    wxLogDebugFunc(_T(""));
    assert(&user);

//  int index = InsertItem( GetItemCount(), wxEmptyString );
//  if(index==-1){
//    wxLogMessage(_T("NickListCtrl::AddUser : index==-1"));
//    return;
//  }

    m_data.push_back( user );
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
    //setitemcount
    SetColumnWidth( 3, wxLIST_AUTOSIZE );
    return;
  }
  wxLogError( _T("Didn't find the user to remove.") );
}


void NickListCtrl::UserUpdated( const User& user )
{
    int index = GetUserIndex( user );
    m_data[index] = user;
    RefreshItem( index );
  //ASSERT_LOGIC( index != -1, _T("index = -1") );
//  if(index!=-1){
//    UserUpdated( index );
//  }else{
//    wxLogWarning(_T("NickListCtrl::UserUpdated error, index == -1 ."));
//  }
    HighlightItemUser( index, user.GetNick() );
    MarkDirtySort();
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
        if ( user.Equals( *it ) ) return i;
    }
    wxLogError( _T("didn't find the user.") );
    return -1;
}


void NickListCtrl::OnActivateItem( wxListEvent& event )
{
  int index = event.GetIndex();
  if ( index == -1 )
    return;

  const User& user = m_data[index];
  ui().mw().OpenPrivateChat( user );
  SetSelectedIndex( index );
}


void NickListCtrl::OnShowMenu( wxContextMenuEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_menu != 0 )
  {
      //no need to popup the menu when there's no user selected
      int selected = GetSelectedIndex();
      if ( selected != -1 ){
          const User& user = m_data[selected];
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
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );
  Sort();
}


//int wxCALLBACK NickListCtrl::ComparePlayernameUP(long item1, long item2, long sortData )
//{
//    return 0;//( m_data[item1].GetNick().CmpNoCase( m_data[item2].GetNick() );
//}
//
//
//int wxCALLBACK NickListCtrl::ComparePlayernameDOWN(long item1, long item2, long sortData )
//{
//    // inverse the order
//    return ((User*)item2)->GetNick().CmpNoCase(((User*)item1)->GetNick());
//}
//
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
//
//int wxCALLBACK NickListCtrl::ComparePlayerrankUP(long item1, long item2, long sortData )
//{
//    // inverse the order
//
//    if ( ((User*)item1)->GetStatus().rank < ((User*)item2)->GetStatus().rank )
//        return -1;
//    if ( ((User*)item1)->GetStatus().rank > ((User*)item2)->GetStatus().rank )
//        return 1;
//
//    return 0;
//}
//
//int wxCALLBACK NickListCtrl::ComparePlayerrankDOWN(long item1, long item2, long sortData )
//{
//    // inverse the order
//    if ( ((User*)item1)->GetStatus().rank < ((User*)item2)->GetStatus().rank )
//        return 1;
//    if ( ((User*)item1)->GetStatus().rank > ((User*)item2)->GetStatus().rank )
//        return -1;
//
//    return 0;
//}
//
//
//int wxCALLBACK NickListCtrl::ComparePlayercountryUP(long item1, long item2, long sortData )
//{
//    return ((User*)item1)->GetCountry().CmpNoCase(((User*)item2)->GetCountry());
//}
//
//int wxCALLBACK NickListCtrl::ComparePlayercountryDOWN(long item1, long item2, long sortData )
//{
//    // inverse the order
//    return ((User*)item2)->GetCountry().CmpNoCase(((User*)item1)->GetCountry());
//}

void NickListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{

    int coloumn = getColoumnFromPosition(position);
    if (coloumn > (int)m_colinfovec.size() || coloumn < 0 || item_hit < 0 || item_hit > m_data.size() )
    {
        m_tiptext = _T("");
    }
    else
    {
        const User& user = m_data[item_hit];
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
    const User& u = m_data[item];
    wxString name = u.GetNick();
    HighlightItemUser( item, name );
}


void NickListCtrl::SortList()
{
//  if ( !m_dirty_sort ) return;
//  SetSelectionRestorePoint();
  Freeze();
  Sort();
  Thaw();
  RefreshItems(0, m_data.size()-1 );
//  RestoreSelection();
//  m_dirty_sort = false;
}

bool userCompare(  const User u1,  const User u2 )
{
    return u1.GetNick() <  u2.GetNick() ;
}

void NickListCtrl::Sort()
{
    //if ( m_dirty_sort && m_data.size() > 0 )
    {
        //UserCompare cmp( m_sortorder, 2 );
        DataIter b = m_data.begin();
        DataIter e = m_data.end();
        std::sort( b, e, userCompare );

    }
}

wxString NickListCtrl::OnGetItemText(long item, long column) const
{
    switch ( column ) {
        case 0:
        case 1:
        case 2:
        default: return wxEmptyString;

        case 3: return m_data[item].GetNick();
    }
}

int NickListCtrl::OnGetItemColumnImage(long item, long column) const
{
    const User& user = m_data[item];
    const UserStatus& user_st = user.GetStatus();
    switch ( column ) {
        case 0: return icons().GetUserListStateIcon( user_st, false, user.GetBattle() != 0 );
        case 1: return icons().GetFlagIcon( user.GetCountry() );
        case 2: return icons().GetRankIcon( user.GetStatus().rank );

        case 3:
        default: return -1;

    }
}

int NickListCtrl::OnGetItemImage(long item) const
{
    return -1;
}
//  //SetItemColumnImage( index, 0, icons().GetUserListStateIcon( user_st, false, user.GetBattle() != 0 ) );
//  SetItemColumnImage( index, 1, icons().GetFlagIcon( user.GetCountry() ) );
//  SetItemColumnImage( index, 2, icons().GetRankIcon( user.GetStatus().rank ) );

NickListCtrl::UserCompare::UserCompare( const SortOrder& sortorder, unsigned int depth )
    : m_depth( depth ), m_sortorder( sortorder )
{

}

bool NickListCtrl::UserCompare::operator() ( User& u1, User& u2 )
{
    return true;//u1.GetNick().CmpNoCase( u2.GetNick() );
}
