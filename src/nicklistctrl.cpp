/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: NickListCtrl
//

#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <stdexcept>

#include "nicklistctrl.h"
#include "utils.h"
#include "iconimagelist.h"
#include "user.h"
#include "ui.h"
#include "mainwindow.h"

#define TOOLTIP_DELAY 1000

int wxCALLBACK NickListSortCallback(long item1, long item2, long sortData);

BEGIN_EVENT_TABLE( NickListCtrl, customListCtrl )
  EVT_LIST_ITEM_ACTIVATED( NICK_LIST, NickListCtrl::OnActivateItem )
  EVT_LIST_COL_CLICK( NICK_LIST, NickListCtrl::OnColClick )
  EVT_CONTEXT_MENU( NickListCtrl::OnShowMenu )
#ifndef __WXMSW__ //disables tooltips on win
  EVT_MOTION(NickListCtrl::OnMouseMotion)
#endif
END_EVENT_TABLE()


NickListCtrl::NickListCtrl( wxWindow* parent,Ui& ui, bool show_header, wxMenu* popup ):
  customListCtrl( parent, NICK_LIST, wxDefaultPosition, wxDefaultSize,
              wxSUNKEN_BORDER | wxLC_REPORT | (int)(!show_header) * wxLC_NO_HEADER | wxLC_SINGLE_SEL ),
  m_ui(ui),
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
  col.SetImage( ICON_DOWN );
  InsertColumn( 3, col, _T("Nickname") );

  m_sortorder[0].col = 0;
  m_sortorder[0].direction = false;
  m_sortorder[1].col = 3;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 2;
  m_sortorder[2].direction = true;
  m_sortorder[3].col = 1;
  m_sortorder[3].direction = true;
  Sort( );

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
  int index = InsertItem( 0, IconImageList::GetUserListStateIcon( user.GetStatus(), false, user.GetBattle() != 0 ) );
  SetItemData( index, (wxUIntPtr)&user );
  ASSERT_LOGIC( index != -1, _T("index = -1") );
  UserUpdated( index );
  Sort();
}

void NickListCtrl::RemoveUser( const User& user )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    if ( &user == (User*)GetItemData( i ) ) {
      DeleteItem( i );
      Sort();
      return;
    }
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
  SetItemImage( index, IconImageList::GetUserListStateIcon( user.GetStatus(), false, user.GetBattle() != 0 ) );
  SetItemColumnImage( index, 1, IconImageList::GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 2, IconImageList::GetRankIcon( user.GetStatus().rank ) );
  SetItem( index, 3, WX_STRING(user.GetNick()) );
  SetItemData(index, (long)&user );
  Sort();

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
  wxLogError( _T("didn't find the user.") );
  return -1;
}


void NickListCtrl::OnActivateItem( wxListEvent& event )
{
  int index = event.GetIndex();
  if ( index == -1 ) return;
  User* user = (User*)event.GetData();
  m_ui.mw().OpenPrivateChat( *user );
  //m_ui.mw().OnTabsChanged( event2 );
}


void NickListCtrl::OnShowMenu( wxContextMenuEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_menu != 0 ) PopupMenu( m_menu );
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
  col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
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
    // inverse the order

    if ( WX_STRING(((User*)item1)->GetNick()).MakeUpper() < WX_STRING(((User*)item2)->GetNick()).MakeUpper() )
        return -1;
    if ( WX_STRING(((User*)item1)->GetNick()).MakeUpper() > WX_STRING(((User*)item2)->GetNick()).MakeUpper() )
        return 1;

    return 0;
}


int wxCALLBACK NickListCtrl::ComparePlayernameDOWN(long item1, long item2, long sortData )
{
    // inverse the order
    if ( WX_STRING(((User*)item1)->GetNick()).MakeUpper() < WX_STRING(((User*)item2)->GetNick()).MakeUpper() )
        return 1;
    if ( WX_STRING(((User*)item1)->GetNick()).MakeUpper() > WX_STRING(((User*)item2)->GetNick()).MakeUpper() )
        return -1;

    return 0;
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
    // inverse the order

    if ( WX_STRING(((User*)item1)->GetCountry()).MakeUpper() < WX_STRING(((User*)item2)->GetCountry()).MakeUpper() )
        return -1;
    if ( WX_STRING(((User*)item1)->GetCountry()).MakeUpper() > WX_STRING(((User*)item2)->GetCountry()).MakeUpper() )
        return 1;

    return 0;
}

int wxCALLBACK NickListCtrl::ComparePlayercountryDOWN(long item1, long item2, long sortData )
{
    // inverse the order
    if ( WX_STRING(((User*)item1)->GetCountry()).MakeUpper() < WX_STRING(((User*)item2)->GetCountry()).MakeUpper() )
        return 1;
    if ( WX_STRING(((User*)item1)->GetCountry()).MakeUpper() > WX_STRING(((User*)item2)->GetCountry()).MakeUpper() )
        return -1;

    return 0;
}

void NickListCtrl::OnMouseMotion(wxMouseEvent& event)
{

	tipTimer.Start(TOOLTIP_DELAY, wxTIMER_ONE_SHOT);
	wxPoint position = event.GetPosition();

	int flag = wxLIST_HITTEST_ONITEM;
	long *ptrSubItem = new long;

	try{
		long item = GetItemData(HitTest(position, flag, ptrSubItem));
		int coloumn = getColoumnFromPosition(position);

		if (item != wxNOT_FOUND)
		{
			User* user = (User*) item;
			
			if (coloumn > (int)m_colinfovec.size() || coloumn < 0)
			{
				m_tiptext = _T("");
			}
			else
			{
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
					m_tiptext =  WX_STRING(user->GetCountry()).MakeUpper();
					break;

				case 2: // rank
					m_tiptext = user->GetRankName(user->GetStatus().rank);
					break;	
					
				case 3: // nickname
					m_tiptext = WX_STRING(user->GetNick());
					break;	

				default: 
					m_tiptext = m_colinfovec[coloumn].first;
					break;
				}
			}
		}
	}catch(...){}

}

