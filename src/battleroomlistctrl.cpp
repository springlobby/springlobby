/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: BattleroomListCtrl
//

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/numdlg.h>
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
#include "settings.h"

template<> SortOrder CustomVirtListCtrl<User*>::m_sortorder = SortOrder();

IBattle* BattleroomListCtrl::s_battle = 0;

BEGIN_EVENT_TABLE( BattleroomListCtrl,  CustomVirtListCtrl< User *>)

  EVT_LIST_ITEM_RIGHT_CLICK( BRLIST_LIST, BattleroomListCtrl::OnListRightClick )
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


BattleroomListCtrl::BattleroomListCtrl( wxWindow* parent, IBattle* battle, Ui& ui, bool readonly ) :
	CustomVirtListCtrl< User *>(parent, BRLIST_LIST, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL, _T("BattleroomListCtrl"), 10, 3, &CompareOneCrit ),
	m_battle(battle),m_popup(0),
  m_sel_user(0), m_sides(0),m_spec_item(0),m_handicap_item(0),
  m_ui(ui),
  m_ro(readonly)
{
  GetAui().manager->AddPane( this, wxLEFT, _T("battleroomlistctrl") );

  wxListItem col;

    const int hd = wxLIST_AUTOSIZE_USEHEADER;

#if defined(__WXMAC__)
/// on mac, autosize does not work at all
    const int widths[13] = {20,20,20,170,140,130,110,28,28,28, 100, 100, 100}; //!TODO revise plox
#else
    const int widths[13] = {hd,hd,hd,hd,hd,170,hd,hd,80,130, 100, 110, 70};
#endif

    AddColumn( 0, widths[0], _T("r"), _T("Player/Bot") );
    AddColumn( 1, widths[1], _T("s"), _T("Faction icon") );
    AddColumn( 2, widths[2], _T("c"), _T("Teamcolour") );
    AddColumn( 3, widths[3], _T("f"), _T("Country") );
    AddColumn( 4, widths[4], _T("r"), _T("Rank") );
    AddColumn( 5, widths[5], _("Nickname"), _T("Ingame name"));
    AddColumn( 6, widths[6], _("t"), _T("Team number") );
    AddColumn( 7, widths[7], _("a"), _T("Ally number") );
    AddColumn( 8, widths[8], _("cpu"), _T("CPU speed (might not be accurate)") );
    AddColumn( 9, widths[9], _("Resource Bonus"), _T("Resource Bonus") );
    AddColumn( 10, widths[10], _("Custom Rank"), _T("Custom Rank") );
    AddColumn( 11, widths[11], _("Rank Accuracy"), _T("Custom Rank Accuracy") );
    AddColumn( 12, widths[12], _("Trust"), _T("Trust") );


    if ( m_sortorder.size() == 0 ) {
        m_sortorder[0].col = 6;
        m_sortorder[0].direction = true;
        m_sortorder[1].col = 7;
        m_sortorder[1].direction = true;
        m_sortorder[2].col = 5;
        m_sortorder[2].direction = true;
    }

  	if ( !m_ro )
	{
		m_popup = new UserMenu(this);

		wxMenu* m_rank = new wxMenu();

		for ( unsigned int i = (unsigned int)UserStatus::USER_RANK_UNKNOWN; i < (unsigned int)UserStatus::USER_RANK_10; i++ )
		{
			wxMenuItem* rank = new wxMenuItem( m_rank, BRLIST_RANK + i, wxString::Format( _T("%d"), i+1 ) , wxEmptyString, wxITEM_NORMAL );
			m_rank->Append( rank );
			Connect( BRLIST_RANK + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnSetRank ) );
		}
		m_popup->Append( -1, _("Set Rank"), m_rank );

		wxMenu* m_trust = new wxMenu();

		for ( unsigned int i = (unsigned int)UserStatus::USER_TRUST_UNKNOWN; i < (unsigned int)UserStatus::USER_TRUST_10; i++ )
		{
			wxMenuItem* trust = new wxMenuItem( m_trust, BRLIST_TRUST + i, wxString::Format( _T("%d"), i+1 ) , wxEmptyString, wxITEM_NORMAL );
			m_trust->Append( trust );
			Connect( BRLIST_TRUST + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnSetTrust ) );
		}
		m_popup->Append( -1, _("Set Trust"), m_trust );

		wxMenu* m_teams;
		m_teams = new wxMenu();

		for ( unsigned int i = 0; i < SPRING_MAX_TEAMS; i++ )
		{
			wxMenuItem* team = new wxMenuItem( m_teams, BRLIST_TEAM + i, wxString::Format( _T("%d"), i+1 ) , wxEmptyString, wxITEM_NORMAL );
			m_teams->Append( team );
			Connect( BRLIST_TEAM + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnTeamSelect ) );
		}
		m_popup->Append( -1, _("Team"), m_teams );

		wxMenu* m_allies = new wxMenu();
		for ( unsigned int i = 0; i < SPRING_MAX_ALLIES; i++ )
		{
			wxMenuItem* ally = new wxMenuItem( m_allies, BRLIST_ALLY + i, wxString::Format( _T("%d"), i+1 ) , wxEmptyString, wxITEM_NORMAL );
			m_allies->Append( ally );
			Connect( BRLIST_ALLY + i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BattleroomListCtrl::OnAllySelect ) );
		}
		m_popup->Append( -1, _("Ally"), m_allies );

		m_sides = new wxMenu();
		try
		{
			wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
			for ( unsigned int i = 0; i < sides.GetCount(); i++ )
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

void BattleroomListCtrl::AddUser( User& user )
{
    if ( GetIndexFromData( &user ) != -1 ) {
        wxLogWarning( _T("user already in battleroom list.") );
        return;
    }
    m_data.push_back( &user );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() );
    SetColumnWidth( 5, wxLIST_AUTOSIZE ); //! TODO does this really work?
    MarkDirtySort();
}

void BattleroomListCtrl::RemoveUser( User& user )
{
    int index = GetIndexFromData( &user );

    if ( index != -1 ) {
        m_data.erase( m_data.begin() + index );
        SetItemCount( m_data.size() );
        RefreshVisibleItems( );
        SetColumnWidth( 5, wxLIST_AUTOSIZE );
        return;
    }
    wxLogError( _T("Didn't find the user to remove in battleroomlistctrl.") );
}

void BattleroomListCtrl::UpdateUser( User& user )
{
    int index = GetIndexFromData( &user );
    RefreshItem( index );
    MarkDirtySort();
}

wxListItemAttr * BattleroomListCtrl::OnGetItemAttr(long item) const
{
    if ( item == -1 || item >= (long)m_data.size())
        return NULL;

    const User& user = *GetDataFromIndex( item );
    bool is_bot = user.BattleStatus().IsBot();

    if ( !is_bot ) {
        return HighlightItemUser( user.GetNick() );
    }

    return NULL;
}

int BattleroomListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if ( item == -1 || item >= (long)m_data.size())
        return -1;

    const User& user = *GetDataFromIndex( item );
    bool is_bot = user.BattleStatus().IsBot();
    bool is_spec = user.BattleStatus().spectator;

    if ( !is_spec )
        icons().SetColourIcon( user.BattleStatus().team, user.BattleStatus().colour );

    switch ( column ) {
        case 0: {
            if ( !is_bot ) {
                if ( m_battle->IsFounder( user ) ) {

                    return icons().GetHostIcon( user.BattleStatus().spectator );
                }
                else {
                    return icons().GetReadyIcon( user.BattleStatus().spectator, user.BattleStatus().ready, user.BattleStatus().sync, user.BattleStatus().IsBot() );
                }
            }
            else
                return icons().ICON_BOT;
        }
        case 2: return is_spec ? -1: icons().GetColourIcon( user.BattleStatus().team );
        case 3: return is_bot ? -1 : icons().GetFlagIcon( user.GetCountry() );
        case 4: return is_bot ? -1 : icons().GetRankIcon( user.GetStatus().rank );
        case 1:
        {
        	if ( is_spec ) return -1;
            try {
                wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
                ASSERT_EXCEPTION( user.BattleStatus().side < (long)sides.GetCount(), _T("Side index too high") );
                int sideimg = icons().GetSideIcon( m_battle->GetHostModName(), user.BattleStatus().side );
                if ( sideimg >= 0 )
                    return sideimg;
            }
            catch ( ... ) {}

            return -1;
        }
        case 6:
        case 7:
        case 8:
        case 9:
        case 5: return -1;
        default: {
            wxLogWarning( _T("coloumn oob in battelroomlistctrl OnGetItemColoumnImage") );
            return -1;
        }
    }
}

wxString BattleroomListCtrl::OnGetItemText(long item, long column) const
{
    if ( item == -1 || item >= (long)m_data.size())
        return _T("");

    User& user = *GetDataFromIndex( item );
    bool is_bot = user.BattleStatus().IsBot();
    bool is_spec = user.BattleStatus().spectator;

    switch ( column ) {
        case 1: {
            try {
                wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
                ASSERT_EXCEPTION( user.BattleStatus().side < (long)sides.GetCount(), _T("Side index too high") );
                int sideimg = icons().GetSideIcon( m_battle->GetHostModName(), user.BattleStatus().side );
                if ( sideimg < 0 )
                    return sides[user.BattleStatus().side];
            }
            catch ( ... ) {
                return wxString::Format( _T("s%d"), user.BattleStatus().side + 1 );
            }
            return _T("");
        }
        case 5: return is_bot ? user.GetNick() + _T(" (") + user.BattleStatus().owner + _T(")") : user.GetNick();
        case 6: return is_spec ? _T("") : wxString::Format( _T("%d"), user.BattleStatus().team + 1 );
        case 7: return is_spec ? _T("") : wxString::Format( _T("%d"), user.BattleStatus().ally + 1 );
        case 8: {
            if (!is_bot )
                return wxString::Format( _T("%.1f GHz"), user.GetCpu() / 1000.0 );
            else { //!TODO could prolly be cached
                wxString botname = user.BattleStatus().aishortname;
                if ( !user.BattleStatus().aiversion.IsEmpty() ) botname += _T(" ") + user.BattleStatus().aiversion;
                if ( !usync().VersionSupports( IUnitSync::USYNC_GetSkirmishAI ) )
                {
                    if ( botname.Contains(_T('.')) ) botname = botname.BeforeLast(_T('.'));
                    if ( botname.Contains(_T('/')) ) botname = botname.AfterLast(_T('/'));
                    if ( botname.Contains(_T('\\')) ) botname = botname.AfterLast(_T('\\'));
                    if ( botname.Contains(_T("LuaAI:")) ) botname = botname.AfterFirst(_T(':'));
                }
                return botname;
            }
        }
        case 9: return is_spec ? _T("") : wxString::Format( _T("%d%%"), user.BattleStatus().handicap );
        case 10:
        {
        	 if ( is_bot) return _T("");
        	 UserStatus::UserRankContainer rank = user.GetCustomRank( m_battle->GetModShortName() );
					 if ( rank == UserStatus::USER_RANK_UNKNOWN ) return _("Unknown");
					 return wxString::Format( _T("%d"), rank + 1 );
        }
        case 11: return is_bot ? _T("") : wxString::Format( _T("%d%%"), user.GetCustomRankAccuracy( m_battle->GetModShortName() ) );
        case 12:
        {
        	 if ( is_bot) return _T("");
        	 UserStatus::UserTrustContainer trust = user.GetTrust();
					 if ( trust == UserStatus::USER_TRUST_UNKNOWN ) return _("Unknown");
					 return wxString::Format( _T("%d"), trust + 1 );
        }
        case 0:
        case 2:
        case 3:
        case 4: return _T("");
        default: {
            wxLogWarning( _T("coloumn oob in battelroomlistctrl OnGetItemText") );
            return _T("");
        }
    }
}

void BattleroomListCtrl::UpdateUser( const int& index )
{
    RefreshItem( index );
    MarkDirtySort();
}

void BattleroomListCtrl::OnListRightClick( wxListEvent& event )
{
    wxLogDebugFunc( _T("") );
	if ( m_ro ) return;
	int index = event.GetIndex();

    if ( index == -1 || index >= (long)m_data.size()) return;

    User& user = *GetDataFromIndex( event.GetIndex() );
    m_sel_user = &user; //this is set for event handlers

    if ( user.BattleStatus().IsBot() )
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
        bool isSelUserMe =  ( m_ui.IsThisMe(user) );
        m_popup->Enable( m_popup->FindItem( _("Kick") ),!isSelUserMe);
    }

    wxLogMessage(_T("Popup"));
    m_popup->EnableItems( !user.BattleStatus().IsBot(), GetSelectedUserNick() );
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
  if( !m_sel_user ) return;
  long handicap = wxGetNumberFromUser( _("Please enter a value between 0 and 100"), _("Set Resource Bonus"), _T(""), m_sel_user->BattleStatus().handicap, 0, 100, (wxWindow*)&ui().mw(), wxDefaultPosition );
	if ( handicap != -1 )
	{
     ((Battle*)m_battle)->SetHandicap( *m_sel_user, handicap );
  }
}


void BattleroomListCtrl::OnSpecSelect( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( m_sel_user ) ((Battle*)m_battle)->ForceSpectator( *m_sel_user, m_spec_item->IsChecked() );
}

void BattleroomListCtrl::OnSetRank( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( !m_sel_user ) return;
  if ( m_sel_user->BattleStatus().IsBot() ) return;
	int rank = event.GetId() - BRLIST_RANK;
	m_battle->SetRank( *m_sel_user, (UserStatus::UserRankContainer)rank );
}

void BattleroomListCtrl::OnSetTrust( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  if ( !m_sel_user ) return;
  if ( m_sel_user->BattleStatus().IsBot() ) return;
  int trust = event.GetId() - BRLIST_TRUST;
	m_battle->SetTrust( *m_sel_user, (UserStatus::UserTrustContainer)trust );
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

void BattleroomListCtrl::Sort()
{
    if ( m_data.size() > 0 )
    {
        SaveSelection();
        s_battle = m_battle;
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}

int BattleroomListCtrl::CompareOneCrit(DataType u1, DataType u2, int col, int dir)
{
    switch ( col ) {
        case 0: return dir * CompareStatus( u1, u2, s_battle );
        case 1: return dir * CompareSide( u1, u2 );
        case 2: return dir * CompareColor( u1, u2 );
        case 3: return dir * compareSimple( u1, u2 );
        case 4: return dir * CompareRank( u1, u2 );
        case 5: return dir * u1->GetNick().CmpNoCase( u2->GetNick() );
        case 6: return dir * CompareTeam( u1, u2 );
        case 7: return dir * CompareAlly( u1, u2 );
        case 8: return dir * CompareCpu( u1, u2 );
        case 9: return dir * CompareHandicap( u1, u2 );
        case 10: return dir * CompareCustomRank( u1, u2 );
        case 11: return dir * CompareAccuracy( u1, u2 );
        case 12: return dir * CompareTrust( u1, u2 );
        default: return 0;
    }
}

int BattleroomListCtrl::CompareStatus(const DataType user1, const DataType user2, const IBattle* m_battle )
{
  int status1 = 0;
  if ( user1->BattleStatus().IsBot() )
  {
    status1 = 9;
  }
  else
  {
    if ( &m_battle->GetFounder() != user1 )
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
    if ( &m_battle->GetFounder() != user2 )
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

int BattleroomListCtrl::CompareSide(const DataType user1, const DataType user2)
{
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

int BattleroomListCtrl::CompareColor(const DataType user1, const DataType user2)
{
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


int BattleroomListCtrl::CompareRank(const DataType user1, const DataType user2)
{
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

int BattleroomListCtrl::CompareTeam(const DataType user1, const DataType user2)
{
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

int BattleroomListCtrl::CompareAlly(const DataType user1, const DataType user2)
{
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

int BattleroomListCtrl::CompareCpu(const DataType user1, const DataType user2)
{
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

int BattleroomListCtrl::CompareHandicap(const DataType user1, const DataType user2)
{
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

int BattleroomListCtrl::CompareCustomRank(const DataType user1, const DataType user2)
{
	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int rank1;
  if ( user1->BattleStatus().IsBot() )
    rank1 = 1000;
  else
    rank1 = user1->GetCustomRank( s_battle->GetModShortName() );

  int rank2;
  if ( user2->BattleStatus().IsBot() )
    rank2 = 1000;
  else
    rank2 = user2->GetCustomRank( s_battle->GetModShortName() );

  if ( rank1 < rank2 )
      return -1;
  if ( rank1 > rank2 )
      return 1;

  return 0;
}

int BattleroomListCtrl::CompareAccuracy(const DataType user1, const DataType user2)
{
	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int accuracy1;
  if ( user1->BattleStatus().IsBot() )
    accuracy1 = 1000;
  else
    accuracy1 = user1->GetCustomRankAccuracy( s_battle->GetModShortName() );

  int accuracy2;
  if ( user2->BattleStatus().IsBot() )
    accuracy2 = 1000;
  else
    accuracy2 = user2->GetCustomRankAccuracy( s_battle->GetModShortName() );

  if ( accuracy1 < accuracy2 )
      return -1;
  if ( accuracy1 > accuracy2 )
      return 1;

  return 0;
}

int BattleroomListCtrl::CompareTrust(const DataType user1, const DataType user2)
{
	if ( !user1 && !user2 ) return 0;
	else if ( !user1 ) return 1;
	else if ( !user2 ) return -1;

  int trust1;
  if ( user1->BattleStatus().IsBot() )
    trust1 = 1000;
  else
    trust1 = user1->GetTrust();

  int trust2;
  if ( user2->BattleStatus().IsBot() )
    trust2 = 1000;
  else
    trust2 = user2->GetTrust();

  if ( trust1 < trust2 )
      return -1;
  if ( trust1 > trust2 )
      return 1;

  return 0;
}

void BattleroomListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
    long item = GetItemData(item_hit);

    if ( item < 0 || item >= (long)m_data.size() )
        return;

    const User& user = *GetDataFromIndex( item );

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
            if ( user.BattleStatus().IsBot() )
                m_tiptext = _T("This is an AI controlled Player (bot)");
            else if  ( user.BattleStatus().spectator )
                m_tiptext = _T("Spectator");
            else
                m_tiptext =  _T("Human Player");
            break;
        case 1: // icon
            if ( user.BattleStatus().spectator )
                m_tiptext = _T("Spectators have no side");
            else
            {
								wxArrayString sides = usync().GetSides( m_battle->GetHostModName() );
								int side = user.BattleStatus().side;
								if ( side < (int)sides.GetCount() ) m_tiptext = sides[side];
            }
            break;

        case 3: // country
            m_tiptext = user.BattleStatus().IsBot() ? _T("This bot is from nowhere particluar") : GetFlagNameFromCountryCode(user.GetCountry());
            break;
        case 4: // rank
            m_tiptext = user.BattleStatus().IsBot() ? _T("This bot has no rank") : user.GetRankName(user.GetStatus().rank);
            break;

        case 5: //name
            m_tiptext = user.BattleStatus().IsBot() ?user.GetNick() : user.GetNick();
            break;

        case 8: // cpu
            m_tiptext = user.BattleStatus().IsBot() ? ( user.BattleStatus().aishortname + _T(" ") + user.BattleStatus().aiversion ) : m_colinfovec[coloumn].tip;
            break;

        default:
            m_tiptext =m_colinfovec[coloumn].tip;
            break;
        }
    }
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
    if ( m_selected_index < 0 || m_selected_index >= (long)m_data.size() )
        return wxEmptyString;
    else
        return m_data[m_selected_index]->GetNick();
}

void BattleroomListCtrl::OnActivateItem( wxListEvent& event )
{
    if ( m_ro )
        return;
    if ( m_selected_index < 0 || m_selected_index >= (long)m_data.size() )
        return;

    const User* usr = m_data[m_selected_index];
    if ( usr != NULL && !usr->BattleStatus().IsBot() )
        ui().mw().OpenPrivateChat( *usr );
}

int BattleroomListCtrl::GetIndexFromData(const DataType& data) const
{
    const User* user = data;
    int i = 0;
    for ( DataCIter it = m_data.begin(); it != m_data.end(); ++it, ++i ) {
        if ( user == *it )
            return i;
    }
    return -1;
}

