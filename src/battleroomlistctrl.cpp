/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleroomListCtrl
//

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wx/msgdlg.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <stdexcept>

#include "battleroomlistctrl.h"
#include "iconimagelist.h"
#include "iunitsync.h"
#include "battle.h"
#include "uiutils.h"
#include "ui.h"
#include "user.h"
#include "server.h"
#include "utils.h"
#include "uiutils.h"


BEGIN_EVENT_TABLE( BattleroomListCtrl, wxListCtrl )

  EVT_LIST_ITEM_RIGHT_CLICK( BRLIST_LIST, BattleroomListCtrl::OnListRightClick )
  EVT_LIST_COL_CLICK       ( BRLIST_LIST, BattleroomListCtrl::OnColClick )
  EVT_MENU                 ( BRLIST_SPEC, BattleroomListCtrl::OnSpecSelect )
  EVT_MENU                 ( BRLIST_KICK, BattleroomListCtrl::OnKickPlayer )
  EVT_MENU                 ( BRLIST_RING, BattleroomListCtrl::OnRingPlayer )
  EVT_MENU                 ( BRLIST_COLOUR, BattleroomListCtrl::OnColourSelect )
  EVT_MENU                 ( BRLIST_HANDICAP, BattleroomListCtrl::OnHandicapSelect )

END_EVENT_TABLE()

Ui* BattleroomListCtrl::m_ui_for_sort = 0;

BattleroomListCtrl::BattleroomListCtrl( wxWindow* parent, Battle& battle, Ui& ui ) : wxListCtrl(parent, BRLIST_LIST,
  wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL ), m_battle(battle),
  m_sel_user(0), m_sel_bot(0),
  m_ui(ui)
{
  SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
  SetImageList( &icons(), wxIMAGE_LIST_SMALL );
  SetImageList( &icons(), wxIMAGE_LIST_STATE );

  wxListItem col;

  col.SetText( _T("r") );
  col.SetImage( -1 );
  InsertColumn( 0, col );

  col.SetText( _T("s") );
  col.SetImage( -1 );
  InsertColumn( 1, col );

  col.SetText( _T("c") );
  col.SetImage( -1 );
  InsertColumn( 2, col );

  col.SetText( _T("f") );
  col.SetImage( -1 );
  InsertColumn( 3, col );

  col.SetText( _T("r") );
  col.SetImage( -1 );
  InsertColumn( 4, col );

  col.SetText( _("Nickname") );
  col.SetImage( -1 );
  InsertColumn( 5, col );

  col.SetText( _("t") );
  col.SetImage( -1 );
  InsertColumn( 6, col );

  col.SetText( _("a") );
  col.SetImage( -1 );
  InsertColumn( 7, col );

  col.SetText( _("cpu") );
  col.SetImage( -1 );
  InsertColumn( 8, col );

  col.SetText( _("Resource Bonus") );
  col.SetImage( -1 );
  InsertColumn( 9, col );

  m_sortorder[0].col = 7;
  m_sortorder[0].direction = true;
  m_sortorder[1].col = 6;
  m_sortorder[1].direction = true;
  m_sortorder[2].col = 5;
  m_sortorder[2].direction = true;
  m_sortorder[3].col = 1;
  m_sortorder[3].direction = true;
  Sort( );

  SetColumnWidth( 0, 20 );
  SetColumnWidth( 1, 20 );
  SetColumnWidth( 2, 20 );
  SetColumnWidth( 3, 20 );
  SetColumnWidth( 4, 20 );
  SetColumnWidth( 5, 170 );
  SetColumnWidth( 6, 26 );
  SetColumnWidth( 7, 26 );
  SetColumnWidth( 8, 80 );
  SetColumnWidth( 9, 130 );

  m_popup = new wxMenu();
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
  usync()->SetCurrentMod( STD_STRING(m_battle.GetModName() ));
  for ( int i = 0; i < usync()->GetSideCount(); i++ ) {
    wxMenuItem* side = new wxMenuItem( m_sides, BRLIST_SIDE + i, WX_STRING(usync()->GetSideName( i )), wxEmptyString, wxITEM_NORMAL );
    m_sides->Append( side );
    Connect( BRLIST_SIDE + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnSideSelect ) );
  }
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


BattleroomListCtrl::~BattleroomListCtrl()
{

}


void BattleroomListCtrl::UpdateList()
{

}


void BattleroomListCtrl::AddUser( User& user )
{
  int index = InsertItem( 0, ICON_NREADY );
  ASSERT_LOGIC( index != -1, "index = -1" );
  SetItemData(index, (wxUIntPtr)&user );

  UpdateUser( index );
}


void BattleroomListCtrl::RemoveUser( User& user )
{
  if ( &user == m_sel_user ) m_sel_user = 0;
  DeleteItem( GetUserIndex( user ) );
}


void BattleroomListCtrl::UpdateUser( User& user )
{
  UpdateUser( GetUserIndex( user ) );
}


void BattleroomListCtrl::UpdateUser( const int& index )
{
  ASSERT_LOGIC( index != -1, "index = -1" );

  wxListItem item;
  item.SetId( index );

  ASSERT_LOGIC( GetItem( item ), "!GetItem" );

  User& user = *((User*)GetItemData( index ));

  icons().SetColourIcon( user.BattleStatus().team, wxColour( user.BattleStatus().color_r, user.BattleStatus().color_g, user.BattleStatus().color_b ) );

  int statimg;
  if ( &m_battle.GetFounder() == &user ) {
    statimg = IconImageList::GetHostIcon( user.BattleStatus().spectator );
  } else {
    statimg = user.BattleStatus().spectator?ICON_SPECTATOR:IconImageList::GetReadyIcon( user.BattleStatus().ready, user.BattleStatus().sync );
  }
  SetItemImage( index, statimg );

  SetItemColumnImage( index, 1, -1 );

  if ( !user.BattleStatus().spectator ) {

    try {
      usync()->SetCurrentMod( STD_STRING(m_battle.GetModName()) );
      int sideimg = icons().GetSideIcon( usync()->GetSideName( user.BattleStatus().side ) );
      if ( sideimg >= 0 ) SetItemColumnImage( index, 1, sideimg );
      else SetItem( index, 1, WX_STRING(usync()->GetSideName( user.BattleStatus().side )) );
    } catch ( ... ) {
      SetItem( index, 1, wxString::Format( _T("s%d"), user.BattleStatus().side + 1 ) );
    }

    SetItemColumnImage( index, 2, IconImageList::GetColourIcon( user.BattleStatus().team ) );

  } else {
    SetItemColumnImage( index, 2, -1 );
  }

  SetItemColumnImage( index, 3, IconImageList::GetFlagIcon( user.GetCountry() ) );
  SetItemColumnImage( index, 4, IconImageList::GetRankIcon( user.GetStatus().rank ) );

  SetItem( index, 5, WX_STRING( user.GetNick() ) );
  if ( !user.Status().bot ) SetItemColumnImage( index, 5, -1 );
  else SetItemColumnImage( index, 5, ICON_BOT );

  if ( !user.BattleStatus().spectator ) {
    SetItem( index, 6, wxString::Format( _T("%d"), user.BattleStatus().team + 1 ) );
    SetItem( index, 7, wxString::Format( _T("%d"), user.BattleStatus().ally + 1 ) );
    SetItem( index, 9, wxString::Format( _T("%d%%"), user.BattleStatus().handicap ) );
  } else {
    SetItem( index, 6, _T("") );
    SetItem( index, 7, _T("") );
    SetItem( index, 9, _T("") );
  }

  SetItem( index, 8, wxString::Format( _T("%.1f GHz"), user.GetCpu() / 1000.0 ) );
}


int BattleroomListCtrl::GetUserIndex( User& user )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    wxListItem item;
    item.SetId( i );
    GetItem( item );
    if ( (unsigned long)&user == GetItemData( i ) ) {
      if ( item.GetImage() == ICON_BOT )
        return -1;
      else
        return i;
    }
  }
  debug_error( "didn't find the user." );
  return -1;
}


void BattleroomListCtrl::AddBot( BattleBot& bot )
{
  int index = InsertItem( 0, ICON_BOT );
  ASSERT_LOGIC( index != -1, "index = -1" );
  SetItemData(index, (wxUIntPtr)&bot );

  UpdateBot( index );
}


void BattleroomListCtrl::RemoveBot( BattleBot& bot )
{
  if ( &bot == m_sel_bot ) m_sel_bot = 0;
  DeleteItem( GetBotIndex( bot ) );
}


void BattleroomListCtrl::UpdateBot( BattleBot& bot )
{
  UpdateBot( GetBotIndex( bot ) );
}


void BattleroomListCtrl::UpdateBot( const int& index )
{
  ASSERT_LOGIC( index != -1, "index = -1" );

  wxListItem item;
  item.SetId( index );

  ASSERT_LOGIC( GetItem( item ), "!GetItem" );

  BattleBot& bot = *((BattleBot*)GetItemData( index ));

  icons().SetColourIcon( bot.bs.team, wxColour( bot.bs.color_r, bot.bs.color_g, bot.bs.color_b ) );

  SetItemImage( index, ICON_BOT );

  SetItemColumnImage( index, 1, -1 );

  try {
    usync()->SetCurrentMod( STD_STRING(m_battle.GetModName()) );
    int sideimg = icons().GetSideIcon( usync()->GetSideName( bot.bs.side ) );
    if ( sideimg >= 0 ) SetItemColumnImage( index, 1, sideimg );
    else SetItem( index, 1, WX_STRING(usync()->GetSideName( bot.bs.side )) );
  } catch ( ... ) {
    SetItem( index, 1, wxString::Format( _T("s%d"), bot.bs.side + 1 ) );
  }

  SetItemColumnImage( index, 2, IconImageList::GetColourIcon( bot.bs.team ) );

  SetItemColumnImage( index, 3, ICON_NONE );
  SetItemColumnImage( index, 4, ICON_NONE );
  SetItem( index, 5, WX_STRING( bot.name ) + _T(" (") + WX_STRING( bot.owner) + _T(")") );

  SetItem( index, 6, wxString::Format( _T("%d"), bot.bs.team + 1 ) );
  SetItem( index, 7, wxString::Format( _T("%d"), bot.bs.ally + 1 ) );
  SetItem( index, 9, wxString::Format( _T("%d%%"), bot.bs.handicap ) );

  SetItem( index, 8, WX_STRING( bot.aidll ) );
}


int BattleroomListCtrl::GetBotIndex( BattleBot& bot )
{
  for (int i = 0; i < GetItemCount() ; i++ ) {
    wxListItem item;
    item.SetId( i );
    GetItem( item );
    if ( item.GetImage() != ICON_BOT ) continue;
    if ( (unsigned long)&bot == GetItemData( i ) ) return i;
  }
  debug_error( "didn't find the bot." );
  return -1;
}


void BattleroomListCtrl::OnListRightClick( wxListEvent& event )
{
  debug_func("");

  if ( event.GetIndex() == -1 ) return;

  if ( event.GetImage() == ICON_BOT ) {
    debug("Bot");
    m_sel_user = 0;
    m_sel_bot = (BattleBot*)event.GetData();
    int item = m_popup->FindItem( _("Spectator") );
    m_popup->Enable( item, false );
    m_popup->Check( item, false );
    m_popup->Enable( m_popup->FindItem( _("Ring") ), false );
  } else {
    debug("User");
    wxMessageBox( _("USER USER USER"), _("Invalid number") );
    m_sel_bot = 0;
    m_sel_user = (User*)event.GetData();
    int item = m_popup->FindItem( _("Spectator") );
    m_popup->Check( item, m_sel_user->BattleStatus().spectator );
    m_popup->Enable( item, true );
    m_popup->Enable( m_popup->FindItem( _("Ring") ), true );
  }

  debug("Popup");
  PopupMenu( m_popup );
  debug("Done");
}


void BattleroomListCtrl::OnTeamSelect( wxCommandEvent& event )
{
  debug_func("");
  int team = event.GetId() - BRLIST_TEAM;
  if ( m_sel_bot != 0 ) {
    m_battle.SetBotTeam( m_sel_bot->name, team );
  } else if ( m_sel_user != 0 ) {
    m_battle.ForceTeam( *m_sel_user, team );
  }
}


void BattleroomListCtrl::OnAllySelect( wxCommandEvent& event )
{
  debug_func("");
  int ally = event.GetId() - BRLIST_ALLY;
  if ( m_sel_bot != 0 ) {
    m_battle.SetBotAlly( m_sel_bot->name, ally );
  } else if ( m_sel_user != 0 ) {
    //m_battle.ForceAlly( *m_sel_user, ally );
  }
}


void BattleroomListCtrl::OnColourSelect( wxCommandEvent& event )
{
  debug_func("");

  if ( m_sel_bot != 0 ) {
    wxColour CurrentColour;
    CurrentColour.Set( m_sel_bot->bs.color_r, m_sel_bot->bs.color_g, m_sel_bot->bs.color_b );
    CurrentColour = wxGetColourFromUser(this, CurrentColour);
    if ( !CurrentColour.IsColourOk() ) return;
    m_battle.SetBotColour( m_sel_bot->name, CurrentColour.Red(), CurrentColour.Green(), CurrentColour.Blue() );
  } else if ( m_sel_user != 0 ) {
    wxColour CurrentColour;
    CurrentColour.Set( m_sel_user->BattleStatus().color_r, m_sel_user->BattleStatus().color_g, m_sel_user->BattleStatus().color_b );
    CurrentColour = wxGetColourFromUser(this, CurrentColour);
    if ( !CurrentColour.IsColourOk() ) return;
    m_battle.ForceColour( *m_sel_user, CurrentColour.Red(), CurrentColour.Green(), CurrentColour.Blue() );
  }

}


void BattleroomListCtrl::OnSideSelect( wxCommandEvent& event )
{
  debug_func("");
  int side = event.GetId() - BRLIST_SIDE;
  if ( m_sel_bot != 0 ) {
    m_battle.SetBotSide( m_sel_bot->name, side );
  } else if ( m_sel_user != 0 ) {
    m_battle.ForceSide( *m_sel_user, side );
  }
}


void BattleroomListCtrl::OnHandicapSelect( wxCommandEvent& event )
{
  debug_func("");
  wxTextEntryDialog dlg( this , _("Please enter a value between 0 and 100"), _("Set Resource Bonus"), _T("0"), wxOK, wxDefaultPosition );
  if ( dlg.ShowModal() == wxID_OK ) {
    long handicap;
    if ( !dlg.GetValue().ToLong( &handicap ) ) {
     wxMessageBox( _("Not a number"), _("Invalid number") );
     return;
    }
    if ( handicap < 0 || handicap > 100 ) {
      wxMessageBox( _("Value out of range.\n Enter an integer between 0 & 100."), _("Invalid number") );
      return;
    }
    if ( m_sel_bot != 0 ) {
      m_battle.SetBotHandicap( m_sel_bot->name, handicap );
    } else if ( m_sel_user != 0 ) {
      m_battle.SetHandicap( *m_sel_user, handicap );
    }
  }
}


void BattleroomListCtrl::OnSpecSelect( wxCommandEvent& event )
{
  debug_func("");
  if ( m_sel_user != 0 ) {
    m_battle.ForceSpectator( *m_sel_user, m_spec_item->IsChecked() );
  }
}


void BattleroomListCtrl::OnKickPlayer( wxCommandEvent& event )
{
  debug_func("");
  if ( m_sel_bot != 0 ) {
    m_battle.RemoveBot( m_sel_bot->name );
  } else if ( m_sel_user != 0 ) {
    m_battle.KickPlayer( *m_sel_user );
  }
}


void BattleroomListCtrl::OnRingPlayer( wxCommandEvent& event )
{
  debug_func("");
  if ( m_sel_bot != 0 ) {
  } else if ( m_sel_user != 0 ) {
    m_battle.GetServer().Ring( m_sel_user->GetNick() );
  }
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
  col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  Sort();
}


void BattleroomListCtrl::Sort()
{
  BattleroomListCtrl::m_ui_for_sort = &m_ui;
  if (!m_ui_for_sort || !m_ui_for_sort->GetServerStatus()  ) return;
  for (int i = 0; i >= 0; i--) { // FIXME: set to i = 3
    switch ( m_sortorder[ i ].col ) {
      case 0 : SortItems( ( m_sortorder[ i ].direction )?&CompareStatusUP:&CompareStatusDOWN , 0 ); break;
      case 1 : SortItems( ( m_sortorder[ i ].direction )?&CompareSideUP:&CompareSideDOWN , 0 ); break;
      case 2 : SortItems( ( m_sortorder[ i ].direction )?&CompareColorUP:&CompareColorDOWN , 0 ); break;
      case 3 : SortItems( ( m_sortorder[ i ].direction )?&CompareCountryUP:&CompareCountryDOWN , 0 ); break;
      case 4 : SortItems( ( m_sortorder[ i ].direction )?&CompareRankUP:&CompareRankDOWN , 0 ); break;
      case 5 : SortItems( ( m_sortorder[ i ].direction )?&CompareNicknameUP:&CompareNicknameDOWN , (wxUIntPtr)this ); break;
      case 6 : SortItems( ( m_sortorder[ i ].direction )?&CompareTeamUP:&CompareTeamDOWN , 0 ); break;
      case 7 : SortItems( ( m_sortorder[ i ].direction )?&CompareAllyUP:&CompareAllyDOWN , 0 ); break;
      case 8 : SortItems( ( m_sortorder[ i ].direction )?&CompareCpuUP:&CompareCpuDOWN , 0 ); break;
      case 9 : SortItems( ( m_sortorder[ i ].direction )?&CompareHandicapUP:&CompareHandicapDOWN , 0 ); break;
    }
  }
}


int wxCALLBACK BattleroomListCtrl::CompareStatusUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  int b1 = 0, b2 = 0;

  if ( battle1.GetInGame() )
    b1 += 1000;
  if ( battle2.GetInGame() )
    b2 += 1000;
  if ( battle1.IsLocked() )
    b1 += 100;
  if ( battle2.IsLocked() )
    b2 += 100;
  if ( battle1.IsPassworded() )
    b1 += 50;
  if ( battle2.IsPassworded() )
    b2 += 50;
  if ( battle1.IsFull() )
    b1 += 25;
  if ( battle2.IsFull() )
    b2 += 25;

  // inverse the order
  if ( b1 < b2 )
      return -1;
  if ( b1 > b2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareStatusDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  int b1 = 0, b2 = 0;

  if ( battle1.GetInGame() )
    b1 += 1000;
  if ( battle2.GetInGame() )
    b2 += 1000;
  if ( battle1.IsLocked() )
    b1 += 100;
  if ( battle2.IsLocked() )
    b2 += 100;
  if ( battle1.IsPassworded() )
    b1 += 50;
  if ( battle2.IsPassworded() )
    b2 += 50;
  if ( battle1.IsFull() )
    b1 += 25;
  if ( battle2.IsFull() )
    b2 += 25;

  // inverse the order
  if ( b1 < b2 )
      return 1;
  if ( b1 > b2 )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareSideUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetDescription()).MakeUpper() < WX_STRING(battle2.GetDescription()).MakeUpper() )
      return -1;
  if ( WX_STRING(battle1.GetDescription()).MakeUpper() > WX_STRING(battle2.GetDescription()).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareSideDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( (WX_STRING(battle1.GetDescription()).MakeUpper()) < (WX_STRING(battle2.GetDescription()).MakeUpper()) )
   {
      return 1;
   }
  if ( (WX_STRING(battle1.GetDescription()).MakeUpper()) > (WX_STRING(battle2.GetDescription()).MakeUpper()) )
    {
      return -1;
    }

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareColorUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() < RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return -1;
  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() > RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareColorDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() < RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return 1;
  if ( RefineMapname( battle1.GetMapName() ).MakeUpper() > RefineMapname( battle2.GetMapName() ).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareCountryUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() < WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return -1;
  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() > WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareCountryDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() < WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return 1;
  if ( WX_STRING(battle1.GetFounder().GetCountry()).MakeUpper() > WX_STRING(battle2.GetFounder().GetCountry()).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareRankUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
      return -1;
  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareRankDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
      return 1;
  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareNicknameUP(long item1, long item2, long sortData)
{
  wxString name1;
  BattleroomListCtrl& bl = *(BattleroomListCtrl*)sortData;

  if ( bl.GetUserIndex( *(User*)item1 ) == -1 )
    name1 = _T("aa"); //(WX_STRING(((BattleBot*)item1)->name ) + _T(" (") + WX_STRING( ((BattleBot*)item1)->owner) + _T(")")).MakeUpper();
  else
    name1 = _T("aa"); //WX_STRING(((User*)item1)->GetNick()).MakeUpper();

  wxString name2;
  if ( bl.GetUserIndex( *(User*)item2 ) == -1 )
    name2 = _T("aa"); //(WX_STRING(((BattleBot*)item2)->name ) + _T(" (") + WX_STRING( ((BattleBot*)item2)->owner) + _T(")")).MakeUpper();
  else
    name2 = _T("aa"); //WX_STRING(((User*)item2)->GetNick()).MakeUpper();

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
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() < WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return -1;
  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() > WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareTeamDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() < WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return 1;
  if ( WX_STRING(battle1.GetFounder().GetNick()).MakeUpper() > WX_STRING(battle2.GetFounder().GetNick()).MakeUpper() )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareAllyUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetSpectators() < battle2.GetSpectators() )
      return -1;
  if ( battle1.GetSpectators() > battle2.GetSpectators() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareAllyDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetSpectators() < battle2.GetSpectators() )
      return 1;
  if ( battle1.GetSpectators() > battle2.GetSpectators() )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareCpuUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetNumUsers() - battle1.GetSpectators() < battle2.GetNumUsers() - battle2.GetSpectators() )
      return -1;
  if ( battle1.GetNumUsers() - battle1.GetSpectators() > battle2.GetNumUsers() - battle2.GetSpectators() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareCpuDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetNumUsers() - battle1.GetSpectators() < battle2.GetNumUsers() - battle2.GetSpectators() )
      return 1;
  if ( battle1.GetNumUsers() - battle1.GetSpectators() > battle2.GetNumUsers() - battle2.GetSpectators() )
      return -1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareHandicapUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetMaxPlayers() < battle2.GetMaxPlayers() )
      return -1;
  if ( battle1.GetMaxPlayers() > battle2.GetMaxPlayers() )
      return 1;

  return 0;
}


int wxCALLBACK BattleroomListCtrl::CompareHandicapDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetMaxPlayers() < battle2.GetMaxPlayers() )
      return 1;
  if ( battle1.GetMaxPlayers() > battle2.GetMaxPlayers() )
      return -1;

  return 0;
}



