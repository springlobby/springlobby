/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleroomListCtrl
//

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <wx/log.h>

#include <stdexcept>
#include <vector>

#include "battleroomlistctrl.h"
#include "iconimagelist.h"
#include "iunitsync.h"
#include "battle.h"
#include "ibattle.h"
#include "uiutils.h"
#include "ui.h"
#include "user.h"
#include "server.h"
#include "utils.h"
#include "uiutils.h"
#include "countrycodes.h"
#include "mainwindow.h"
#include "aui/auimanager.h"
#include "settings++/custom_dialogs.h"


BEGIN_EVENT_TABLE( BattleroomListCtrl,  CustomListCtrl)

  EVT_LIST_ITEM_RIGHT_CLICK( BRLIST_LIST, BattleroomListCtrl::OnListRightClick )
  EVT_LIST_COL_CLICK       ( BRLIST_LIST, BattleroomListCtrl::OnColClick )
  EVT_MENU                 ( BRLIST_SPEC, BattleroomListCtrl::OnSpecSelect )
  EVT_MENU                 ( BRLIST_KICK, BattleroomListCtrl::OnKickPlayer )
  EVT_LIST_ITEM_ACTIVATED( BRLIST_LIST, BattleroomListCtrl::OnActivateItem )
//  EVT_MENU                 ( BRLIST_ADDCREATEGROUP, BattleroomListCtrl::OnPlayerAddToGroup )
//  EVT_MENU                 ( BRLIST_ADDTOGROUP, BattleroomListCtrl::OnPlayerAddToGroup )
  EVT_MENU                 ( BRLIST_RING, BattleroomListCtrl::OnRingPlayer )
  EVT_MENU                 ( BRLIST_COLOUR, BattleroomListCtrl::OnColourSelect )
  EVT_MENU                 ( BRLIST_HANDICAP, BattleroomListCtrl::OnHandicapSelect )
#if wxUSE_TIPWINDOW
#ifndef __WXMSW__ //disables tooltips on win
  EVT_MOTION(BattleroomListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()

Ui* BattleroomListCtrl::m_ui_for_sort = 0;

BattleroomListCtrl::BattleroomListCtrl( wxWindow* parent, IBattle* battle, Ui& ui, bool readonly ) :
	CustomListCtrl(parent, BRLIST_LIST, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL, _T("BattleroomListCtrl"), 10 ),
	m_battle(battle),m_popup(0),
  m_sel_user(0), m_sides(0),m_spec_item(0),m_handicap_item(0),
  m_ui(ui),
  m_ro(readonly)
{
  GetAui().manager->AddPane( this, wxLEFT, _T("battleroomlistctrl") );

  wxListItem col;

  col.SetText( _T("r") );
  col.SetImage(icons().ICON_NONE );
  InsertColumn( 0, col, _T("Player/Bot") );

  col.SetText( _T("s") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 1, col,_T("Faction icon") );

  col.SetText( _T("c") );
  col.SetImage(  icons().ICON_NONE);
  InsertColumn( 2, col, _T("Teamcolour") );

  col.SetText( _T("f") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 3, col, _T("Country") );

  col.SetText( _T("r") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 4, col, _T("Rank") );

  col.SetText( _("Nickname") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 5, col, _T("Ingame name"));

  col.SetText( _("t") );
  col.SetImage(icons().ICON_NONE );
  InsertColumn( 6, col, _T("Team number") );

  col.SetText( _("a") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 7, col, _T("Ally number") );

  col.SetText( _("cpu") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 8, col, _T("CPU speed (might not be accurate)") );

  col.SetText( _("Resource Bonus") );
  col.SetImage( icons().ICON_NONE );
  InsertColumn( 9, col, _T("Resource Bonus") );

  m_sortorder[0].col = 6;
  m_sortorder[0].direction = true;
  m_sortorder[1].col = 7;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 5;
  m_sortorder[2].direction = true;

  SetColumnWidth( 0, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 1, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 2, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 3, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 4, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 6, wxLIST_AUTOSIZE_USEHEADER );
  SetColumnWidth( 7, wxLIST_AUTOSIZE_USEHEADER );

  SetColumnWidth( 5, 170 );

  SetColumnWidth( 8, 80 );
  SetColumnWidth( 9, 130 );

	if ( !m_ro )
	{
		m_popup = new UserMenu(this);
		wxMenu* m_teams;
		m_teams = new wxMenu();

		for ( int i = 0; i < 16; i++ ) {
			wxMenuItem* team = new wxMenuItem( m_teams, BRLIST_TEAM + i, wxString::Format( _T("%d"), i+1 ) , wxEmptyString, wxITEM_NORMAL );
			m_teams->Append( team );
			Connect( BRLIST_TEAM + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnTeamSelect ) );
		}
		m_popup->Append( -1, _("Team"), m_teams );

		wxMenu* m_allies = new wxMenu();
		for ( int i = 0; i < 16; i++ ) {
			wxMenuItem* ally = new wxMenuItem( m_allies, BRLIST_ALLY + i, wxString::Format( _T("%d"), i+1 ) , wxEmptyString, wxITEM_NORMAL );
			m_allies->Append( ally );
			Connect( BRLIST_ALLY + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnAllySelect ) );
		}
		m_popup->Append( -1, _("Ally"), m_allies );

		m_sides = new wxMenu();
		try
		{
			wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
			for ( int i = 0; i < sides.GetCount(); i++ )
			{
				wxMenuItem* side = new wxMenuItem( m_sides, BRLIST_SIDE + i, sides[i], wxEmptyString, wxITEM_NORMAL );
				m_sides->Append( side );
				Connect( BRLIST_SIDE + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnSideSelect ) );
			}
		} catch (...) {}
		m_popup->Append( -1, _("Side"), m_sides );

		m_popup->AppendSeparator();

		wxMenuItem* m_colours = new wxMenuItem( m_popup, BRLIST_COLOUR, _("Set color"), wxEmptyString, wxITEM_NORMAL );
		m_popup->Append( m_colours );

		m_popup->AppendSeparator();

		m_handicap_item = new wxMenuItem( m_popup, BRLIST_HANDICAP, _("Set Resource Bonus"), wxEmptyString, wxITEM_NORMAL );
		m_popup->Append( m_handicap_item );

		m_popup->AppendSeparator();

		m_spec_item = new wxMenuItem( m_popup, BRLIST_SPEC, wxString( _("Spectator") ) , wxEmptyString, wxITEM_CHECK );
		m_popup->Append( m_spec_item );

		m_popup->AppendSeparator();

		wxMenuItem* kick = new wxMenuItem( m_popup, BRLIST_KICK, wxString( _("Kick") ) , wxEmptyString, wxITEM_NORMAL );
		m_popup->Append( kick );
		wxMenuItem* ring = new wxMenuItem( m_popup, BRLIST_RING, wxString( _("Ring") ) , wxEmptyString, wxITEM_NORMAL );
		m_popup->Append( ring );
	}
}


BattleroomListCtrl::~BattleroomListCtrl()
{

}

void BattleroomListCtrl::SetBattle( IBattle* battle )
{
	m_battle = battle;
}

IBattle& BattleroomListCtrl::GetBattle()
{
	ASSERT_EXCEPTION( m_battle, _T("m_battle == 0") );
	return *m_battle;
}

void BattleroomListCtrl::UpdateList()
{

}


void BattleroomListCtrl::AddUser( User& user )
{
	int index;
  if( !user.BattleStatus().IsBot() ) index = InsertItem( GetItemCount(),icons().ICON_NREADY );
	else index = InsertItem( GetItemCount(),icons().ICON_BOT );
  try
  {
    ASSERT_LOGIC( index != -1, _T("index = -1") );
  } catch (...) { return; }

  wxLogMessage(_T("BattleroomListCtrl::AddUser index=%d name=%s"),index,user.GetNick().c_str());

  items.push_back(&user);

  SetItemData(index, (wxUIntPtr)(items.size()-1) );

  UpdateUser( index );
  SetColumnWidth( 5, wxLIST_AUTOSIZE );
  MarkDirtySort();
}


void BattleroomListCtrl::RemoveUser( User& user )
{
  if ( &user == m_sel_user ) m_sel_user = 0;
  DeleteItem( GetUserIndex( user ) );
  SetColumnWidth( 5, wxLIST_AUTOSIZE );
}


void BattleroomListCtrl::UpdateUser( User& user )
{
  int index=GetUserIndex( user );
  UpdateUser( index );
}


void BattleroomListCtrl::UpdateUser( const int& index )
{
  try
  {
    ASSERT_LOGIC( index != -1, _T("index = -1") );
  } catch (...) { return; }



  wxListItem item;
  item.SetId( index );

  if( !GetItem( item ) ) return;

  User* tmp = items[(size_t)GetItemData( index )];
  if ( !tmp ) return;
	User& user = *tmp;

  if( !user.BattleStatus().IsBot() )
  {
			SetItemColumnImage( index, 4, icons().GetRankIcon( user.GetStatus().rank ) );
  	 SetItem( index, 5,  user.GetNick() );
  	 SetItem( index, 8, wxString::Format( _T("%.1f GHz"), user.GetCpu() / 1000.0 ) );
  	 SetItemColumnImage( index, 3,icons().GetFlagIcon( user.GetCountry() ) );
		if ( &m_battle->GetFounder() == &user )
		{
			SetItemImage( index, icons().GetHostIcon( user.BattleStatus().spectator ) );
		}
		else
		{
			SetItemImage( index, icons().GetReadyIcon( user.BattleStatus().spectator, user.BattleStatus().ready, user.BattleStatus().sync, user.BattleStatus().IsBot() ) );
		}
  }
  else
  {
  	SetItemImage( index, index,icons().ICON_BOT );
  	SetItemColumnImage( index, 0, icons().ICON_BOT );
		SetItemColumnImage( index, 3,icons().ICON_NONE );
		SetItemColumnImage( index, 4,icons().ICON_NONE );
		SetItem( index, 5, user.GetNick() + _T(" (") + user.BattleStatus().owner + _T(")") );

		wxString botname = user.BattleStatus().aishortname;
		if ( !user.BattleStatus().aiversion.IsEmpty() ) botname += _T(" ") + user.BattleStatus().aiversion;
		if ( !usync().VersionSupports( IUnitSync::USYNC_GetSkirmishAI ) )
		{
			if ( botname.Contains(_T('.')) ) botname = botname.BeforeLast(_T('.'));
			if ( botname.Contains(_T('/')) ) botname = botname.AfterLast(_T('/'));
			if ( botname.Contains(_T('\\')) ) botname = botname.AfterLast(_T('\\'));
			if ( botname.Contains(_T("LuaAI:")) ) botname = botname.AfterFirst(_T(':'));
		}

		SetItem( index, 8, botname );

  }
  SetItemColumnImage( index, 1, -1 );
	SetItemColumnImage( index, 5, -1 );

  if ( !user.BattleStatus().spectator )
  {
		icons().SetColourIcon( user.BattleStatus().team, user.BattleStatus().colour );
    SetItem( index, 6, wxString::Format( _T("%d"), user.BattleStatus().team + 1 ) );
    SetItem( index, 7, wxString::Format( _T("%d"), user.BattleStatus().ally + 1 ) );
    SetItem( index, 9, wxString::Format( _T("%d%%"), user.BattleStatus().handicap ) );
    SetItemColumnImage( index, 2, icons().GetColourIcon( user.BattleStatus().team ) );
    try
    {
    	wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
    	ASSERT_EXCEPTION( user.BattleStatus().side < sides.GetCount(), _T("Side index too high") );
      int sideimg = icons().GetSideIcon( m_battle->GetHostModName(), user.BattleStatus().side );
      if ( sideimg >= 0 ) SetItemColumnImage( index, 1, sideimg );
      else SetItem( index, 1, sides[user.BattleStatus().side]);
    } catch ( ... )
    {
      SetItem( index, 1, wxString::Format( _T("s%d"), user.BattleStatus().side + 1 ) );
    }

  }
  else
  {
  	SetItemColumnImage( index, 2, -1 );
    SetItem( index, 6, _T("") );
    SetItem( index, 7, _T("") );
    SetItem( index, 9, _T("") );
  }
  HighlightItemUser( index, user.GetNick() );

  MarkDirtySort();
}


int BattleroomListCtrl::GetUserIndex( User& user )
{
  for (int i = 0; i < GetItemCount() ; i++ )
  {
    wxListItem item;
    item.SetId( i );
    GetItem( item );

    User* usr = items[(size_t)GetItemData( i )];

    if ( &user == usr )
      return i;
  }
  wxLogError( _T("GetUserIndex didn't find the user in this battle.") );
  return -1;
}



void BattleroomListCtrl::OnListRightClick( wxListEvent& event )
{
  wxLogDebugFunc( _T("") );
	if ( m_ro ) return;
  if ( event.GetIndex() == -1 ) return;

  User* user = items[(size_t)GetItemData( event.GetIndex() )];
	m_sel_user = user;
  if ( m_sel_user->BattleStatus().IsBot() )
  {
    wxLogMessage(_T("Bot"));

    int item = m_popup->FindItem( _("Spectator") );
    m_popup->Enable( item, false );
    m_popup->Check( item, false );
    m_popup->Enable( m_popup->FindItem( _("Ring") ), false );
    m_popup->Enable( m_popup->FindItem( _("Kick") ),true);
  }
  else
  {
    wxLogMessage(_T("User"));
    int item = m_popup->FindItem( _("Spectator") );
    m_popup->Check( item, m_sel_user->BattleStatus().spectator );
    m_popup->Enable( item, true );
    m_popup->Enable( m_popup->FindItem( _("Ring") ), true );
    bool isSelUserMe =  ( m_ui.IsThisMe(*m_sel_user) );
    m_popup->Enable( m_popup->FindItem( _("Kick") ),!isSelUserMe);
  }

  wxLogMessage(_T("Popup"));
  m_popup->EnableItems(!m_sel_user->BattleStatus().IsBot(), GetSelectedUserNick() );
  PopupMenu( m_popup );
  wxLogMessage(_T("Done"));
}


void BattleroomListCtrl::OnTeamSelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  int team = event.GetId() - BRLIST_TEAM;
	if( m_sel_user ) ((Battle*)m_battle)->ForceTeam( *m_sel_user, team );
}


void BattleroomListCtrl::OnAllySelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  int ally = event.GetId() - BRLIST_ALLY;
	if( m_sel_user ) ((Battle*)m_battle)->ForceAlly( *m_sel_user, ally );
}


void BattleroomListCtrl::OnColourSelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );

	wxColour CurrentColour = m_sel_user->BattleStatus().colour;
	CurrentColour = GetColourFromUser(this, CurrentColour);
	if ( !CurrentColour.IsColourOk() ) return;
	if( m_sel_user ) ((Battle*)m_battle)->ForceColour( *m_sel_user, CurrentColour );

}


void BattleroomListCtrl::OnSideSelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  int side = event.GetId() - BRLIST_SIDE;
  if( m_sel_user ) ((Battle*)m_battle)->ForceSide( *m_sel_user, side );
}


void BattleroomListCtrl::OnHandicapSelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  wxTextEntryDialog dlg( this , _("Please enter a value between 0 and 100"), _("Set Resource Bonus"), _T("0"), wxOK, wxDefaultPosition );
  if ( dlg.ShowModal() == wxID_OK ) {
    long handicap;
    if ( !dlg.GetValue().ToLong( &handicap ) ) {
     wxLogWarning( _T("input is not a number") );
     customMessageBox(SL_MAIN_ICON, _("Not a number"), _("Invalid number") );
     return;
    }
    if ( handicap < 0 || handicap > 100 ) {
      wxLogWarning( _T("input value is out of range") );
      customMessageBox(SL_MAIN_ICON, _("Value out of range.\n Enter an integer between 0 & 100."), _("Invalid number") );
      return;
    }
    if( m_sel_user ) ((Battle*)m_battle)->SetHandicap( *m_sel_user, handicap );
  }
}


void BattleroomListCtrl::OnSpecSelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_sel_user ) ((Battle*)m_battle)->ForceSpectator( *m_sel_user, m_spec_item->IsChecked() );
}


void BattleroomListCtrl::OnKickPlayer( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
	if ( m_sel_user ) ((Battle*)m_battle)->KickPlayer( *m_sel_user );
}


void BattleroomListCtrl::OnRingPlayer( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_sel_user ) ((Battle*)m_battle)->GetServer().Ring( m_sel_user->GetNick() );
}


void BattleroomListCtrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( -1 );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}
  if ( i > 3 ) { i = 3; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  //col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}


void BattleroomListCtrl::Sort()
{
  BattleroomListCtrl::m_ui_for_sort = &m_ui;
  if (!m_ui_for_sort || !m_ui_for_sort->GetServerStatus()  ) return;
  for (int i = 2; i >= 0; i--) {
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&CompareStatusUP:&CompareStatusDOWN , (wxUIntPtr)this ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareSideUP:&CompareSideDOWN , (wxUIntPtr)this ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&CompareColorUP:&CompareColorDOWN , (wxUIntPtr)this ); break;
      case 3 : SortItems( ( m_sortorder[ i ].direction )?&CompareCountryUP:&CompareCountryDOWN , (wxUIntPtr)this ); break;
      case 4 : SortItems( ( m_sortorder[ i ].direction )?&CompareRankUP:&CompareRankDOWN , (wxUIntPtr)this ); break;
      case 5 : SortItems( ( m_sortorder[ i ].direction )?&CompareNicknameUP:&CompareNicknameDOWN , (wxUIntPtr)this ); break;
      case 6 : SortItems( ( m_sortorder[ i ].direction )?&CompareTeamUP:&CompareTeamDOWN , (wxUIntPtr)this ); break;
      case 7 : SortItems( ( m_sortorder[ i ].direction )?&CompareAllyUP:&CompareAllyDOWN , (wxUIntPtr)this ); break;
      case 8 : SortItems( ( m_sortorder[ i ].direction )?&CompareCpuUP:&CompareCpuDOWN , (wxUIntPtr)this ); break;
      case 9 : SortItems( ( m_sortorder[ i ].direction )?&CompareHandicapUP:&CompareHandicapDOWN , (wxUIntPtr)this ); break;
    }
  }
}


int wxCALLBACK BattleroomListCtrl::CompareStatusUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int status1 = 0;
  if ( user1->BattleStatus().IsBot() )
  {
    status1 = 9;
  }
  else
  {
    if ( &bl.m_battle->GetFounder() != user1 )
      status1 = 1;
    if ( user1->BattleStatus().ready )
      status1 += 5;
    if ( user1->BattleStatus().sync )
      status1 += 7;
    if ( user1->BattleStatus().spectator )
      status1 += 10;
  }

  int status2 = 0;
  if ( user1->BattleStatus().IsBot() )
  {
    status2 = 9;
  }
  else
  {
    if ( &bl.m_battle->GetFounder() != user2 )
      status2 = 1;
    if ( user2->BattleStatus().ready )
      status2 += 5;
    if ( user2->BattleStatus().sync )
      status2 += 7;
    if ( user2->BattleStatus().spectator )
      status2 += 10;
  }

  if ( status1 < status2 )
      return -1;
  if ( status1 > status2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareStatusDOWN(long item1, long item2, long sortData)
{
  return CompareStatusUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareSideUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int side1;
  if ( user1->BattleStatus().spectator )
    side1 = 1000;
  else
    side1 = user1->BattleStatus().side;

  int side2;
  if ( user2->BattleStatus().spectator )
    side2 = 1000;
  else
    side2 = user2->BattleStatus().side;

  if ( side1 < side2 )
      return -1;
  if ( side1 > side2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareSideDOWN(long item1, long item2, long sortData)
{
  return CompareSideUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareColorUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int color1_r, color1_g, color1_b;

	if ( user1->BattleStatus().spectator ) return -1;
	color1_r = user1->BattleStatus().colour.Red();
	color1_g = user1->BattleStatus().colour.Green();
	color1_b = user1->BattleStatus().colour.Blue();

  int color2_r, color2_g, color2_b;

	if ( user2->BattleStatus().spectator ) return 1;
	color2_r = user2->BattleStatus().colour.Red();
	color2_g = user2->BattleStatus().colour.Green();
	color2_b = user2->BattleStatus().colour.Blue();

  if ( (color1_r + color1_g + color1_b)/3 < (color2_r + color2_g + color2_b)/3 )
      return -1;
  if ( (color1_r + color1_g + color1_b)/3 > (color2_r + color2_g + color2_b)/3 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareColorDOWN(long item1, long item2, long sortData)
{
  return CompareColorUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareCountryUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  wxString country1 = user1->GetCountry().Upper();

  wxString country2 = user2->GetCountry().Upper();

  if ( country1 < country2 )
      return -1;
  if ( country1 > country2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareCountryDOWN(long item1, long item2, long sortData)
{
  return CompareCountryUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareRankUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int rank1;
  if ( user1->BattleStatus().IsBot() )
    rank1 = 1000;
  else
    rank1 = user1->GetStatus().rank;

  int rank2;
  if ( user2->BattleStatus().IsBot() )
    rank2 = 1000;
  else
    rank2 = user2->GetStatus().rank;

  if ( rank1 < rank2 )
      return -1;
  if ( rank1 > rank2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareRankDOWN(long item1, long item2, long sortData)
{
  return CompareRankUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareNicknameUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  wxString name1 = user1->GetNick().Upper();

  wxString name2 = user2->GetNick().Upper();

  if ( name1 < name2 )
      return -1;
  if ( name1 > name2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareNicknameDOWN(long item1, long item2, long sortData)
{
  return CompareNicknameUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareTeamUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int team1;
  if ( user1->BattleStatus().spectator )
    team1 = 1000;
  else
    team1 = user1->BattleStatus().team;

  int team2;
  if ( user2->BattleStatus().spectator )
    team2 = 1000;
  else
    team2 = user2->BattleStatus().team;

  if ( team1 < team2 )
      return -1;
  if ( team1 > team2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareTeamDOWN(long item1, long item2, long sortData)
{
  return CompareTeamUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareAllyUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int ally1;
  if ( user1->BattleStatus().spectator )
    ally1 = 1000;
  else
    ally1 = user1->BattleStatus().ally;

  int ally2;
  if ( user2->BattleStatus().spectator )
    ally2 = 1000;
  else
    ally2 = user2->BattleStatus().ally;

  if ( ally1 < ally2 )
      return -1;
  if ( ally1 > ally2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareAllyDOWN(long item1, long item2, long sortData)
{
  return CompareAllyUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareCpuUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  if ( user1->BattleStatus().IsBot() )
  {
    wxString ailib1 = user1->BattleStatus().aishortname.Upper() + _T(" ") + user1->BattleStatus().aiversion.Upper();
    if ( user2->BattleStatus().IsBot() )
    {
      wxString ailib2 = user2->BattleStatus().aishortname.Upper() + _T(" ") + user2->BattleStatus().aiversion.Upper();
      if ( ailib1 < ailib2 )
        return -1;
      if ( ailib1 > ailib2 )
        return 1;
      return 0;
    } else {
      return 1;
    }
  }
  else
  {
    int cpu1 = user1->GetCpu();
    if ( user1->BattleStatus().IsBot() )
    {
      return -1;
    } else {
      int cpu2 = user2->GetCpu();
      if ( cpu1 < cpu2 )
        return -1;
      if ( cpu1 > cpu2 )
        return 1;
      return 0;
    }
  }
}


int wxCALLBACK BattleroomListCtrl::CompareCpuDOWN(long item1, long item2, long sortData)
{
  return CompareCpuUP(item1, item2, sortData)*-1;
}


int wxCALLBACK BattleroomListCtrl::CompareHandicapUP(long item1, long item2, long sortData)
{
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;
  User* user1= bl.items[(size_t)item1];
  User* user2= bl.items[(size_t)item2];

	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int handicap1;
	if ( user1->BattleStatus().spectator )
    handicap1 = 1000;
  else
    handicap1 = user1->BattleStatus().handicap;

  int handicap2;
	if ( user2->BattleStatus().spectator )
    handicap2 = 1000;
  else
    handicap2 = user2->BattleStatus().handicap;

  if ( handicap1 < handicap2 )
      return -1;
  if ( handicap1 > handicap2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareHandicapDOWN(long item1, long item2, long sortData)
{
  return CompareHandicapUP(item1, item2, sortData)*-1;
}

void BattleroomListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
    long item = GetItemData(item_hit);
    User* user = this->items[(size_t)item];
    if ( !user )
    {
    	 m_tiptext = _T("");
    	 return;
    }
    int coloumn = getColoumnFromPosition( position );
    if (coloumn > (int)m_colinfovec.size() || coloumn < 0)
    {
        m_tiptext = _T("");
    }
    else
    {
        switch (coloumn)
        {
        case 0: // is bot?
            if ( user->BattleStatus().IsBot() )
                m_tiptext = _T("This is an AI controlled Player (bot)");
            else if  ( user->BattleStatus().spectator )
                m_tiptext = _T("Spectator");
            else
                m_tiptext =  _T("Human Player");
            break;
        case 1: // icon
            if ( user->BattleStatus().spectator )
                m_tiptext = _T("Spectators have no side");
            else
            {
								wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
								int side = user->BattleStatus().side;
								if ( side < (int)sides.GetCount() ) m_tiptext = sides[side];
            }
            break;

        case 3: // country
            m_tiptext = user->BattleStatus().IsBot() ? _T("This bot is from nowhere particluar") : GetFlagNameFromCountryCode(user->GetCountry().Upper());
            break;
        case 4: // rank
            m_tiptext = user->BattleStatus().IsBot() ? _T("This bot has no rank") : user->GetRankName(user->GetStatus().rank);
            break;

        case 5: //name
            m_tiptext = user->BattleStatus().IsBot() ?user->GetNick() : user->GetNick();
            break;

        case 8: // cpu
            m_tiptext = user->BattleStatus().IsBot() ? ( user->BattleStatus().aishortname + _T(" ") + user->BattleStatus().aiversion ) : m_colinfovec[coloumn].first;
            break;

        default:
            m_tiptext =m_colinfovec[coloumn].first;
            break;
        }
    }
}

void BattleroomListCtrl::HighlightItem( long item )
{
    User* usr = items[(size_t)GetItemData( item )];
    if ( !usr ) return;
		HighlightItemUser( item, usr->GetNick() );
}

void BattleroomListCtrl::OnUserMenuAddToGroup( wxCommandEvent& event )
{
    int id  = event.GetId() - GROUP_ID;
    wxString groupname = m_popup->GetGroupByEvtID(id);
    wxString nick = GetSelectedUserNick();
    useractions().AddUserToGroup( groupname, nick );
}

void BattleroomListCtrl::OnUserMenuDeleteFromGroup( wxCommandEvent& event )
{
    wxString nick = GetSelectedUserNick();
    useractions().RemoveUser( nick );
}

void BattleroomListCtrl::OnUserMenuCreateGroup( wxCommandEvent& event )
{
    wxString name;
    if ( ui().AskText( _("Enter name"),
        _("Please enter the name for the new group.\nAfter clicking ok you will be taken to adjust its settings."), name ) )
    {
        wxString nick = GetSelectedUserNick();

        useractions().AddGroup( name );
        useractions().AddUserToGroup( name, nick );
        ui().mw().ShowConfigure( OPT_PAGE_GROUPS );
    }
}

wxString BattleroomListCtrl::GetSelectedUserNick()
{
    User* usr = items[(size_t)GetSelectedData()];
    if ( !usr ) return _T("");
    return usr->GetNick();
}

void BattleroomListCtrl::SortList()
{
  if ( !m_dirty_sort ) return;
  SetSelectionRestorePoint();
  Sort();
  RestoreSelection();
  m_dirty_sort = false;
}

void BattleroomListCtrl::OnActivateItem( wxListEvent& event )
{
		if ( m_ro ) return;
    User* usr = items[(size_t)GetSelectedData()];
    if ( usr != NULL && !usr->BattleStatus().IsBot() )
        ui().mw().OpenPrivateChat( *usr );
}
