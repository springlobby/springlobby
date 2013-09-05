/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "battlelistctrl.h"

#include <wx/intl.h>
#include <wx/menu.h>

#include "user.h"
#include "iconimagelist.h"
#include "uiutils.h"
#include "ui.h"
#include "server.h"
#include "countrycodes.h"
#include "settings.h"
#include "utils/customdialogs.h"
#include "useractions.h"
#include "helper/sortutil.h"
#include "aui/auimanager.h"

template<> SortOrder CustomVirtListCtrl<IBattle*,BattleListCtrl>::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE(BattleListCtrl, BattleListCtrl::BaseType )

  EVT_LIST_ITEM_RIGHT_CLICK( BLIST_LIST, BattleListCtrl::OnListRightClick )
  EVT_MENU                 ( BLIST_DLMAP, BattleListCtrl::OnDLMap )
  EVT_MENU                 ( BLIST_DLMOD, BattleListCtrl::OnDLMod )
#if wxUSE_TIPWINDOW
#if !defined(__WXMSW__) /* && !defined(__WXMAC__) */ //disables tooltips on msw /* and mac */
  EVT_MOTION(BattleListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()

BattleListCtrl::BattleListCtrl( wxWindow* parent )
	: CustomVirtListCtrl< IBattle *,BattleListCtrl>(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize,
													wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT,
													_T("BattleListCtrl"), 4, &BattleListCtrl::CompareOneCrit,
													true /*highlight*/, UserActions::ActHighlight, true /*periodic sort*/ ),
    m_popup( 0 )
{
    GetAui().manager->AddPane( this, wxLEFT, _T("battlelistctrl") );



#if defined(__WXMAC__)
/// on mac, autosize does not work at all
    const int widths[12] = {20,20,20,170,140,130,110,28,28,28,30,30};
#else
    const int hd = wxLIST_AUTOSIZE_USEHEADER;
    const int widths[12] = {hd,hd,hd,170,140,130,110,hd,hd,hd,hd,hd};
#endif

	AddColumn( 0, widths[0], _("Status"), _("Status") );
	AddColumn( 1, widths[1], _("Country"), _("Country") );
	AddColumn( 2, widths[2], _("Rank"), _("Minimum rank to join") );
	AddColumn( 3, widths[3], _("Description"), _("Battle description") );
	AddColumn( 4, widths[4], _("Map"), _("Mapname") );
	AddColumn( 5, widths[5], _("Game"), _("Gamename") );
	AddColumn( 6, widths[6], _("Host"), _("Name of the Host") );
	AddColumn( 7, widths[7], _("Spectators"), _("Number of Spectators") );
	AddColumn( 8, widths[8], _("Players"), _("Number of Players joined") );
	AddColumn( 9, widths[9], _("Max"), _("Maximum number of Players that can join") );
	AddColumn( 10, widths[10], _("Running"), _("How long the game has been running for") );
	AddColumn( 11, widths[11], _("Version"), _("Version of the engine") );

    if ( m_sortorder.size() == 0 ) {
        m_sortorder[0].col = 0;
        m_sortorder[0].direction = true;
        m_sortorder[1].col = 5;
        m_sortorder[1].direction = true;
        m_sortorder[2].col = 9;
        m_sortorder[2].direction = true;
        m_sortorder[3].col = 4;
        m_sortorder[3].direction = true;
    }

}


BattleListCtrl::~BattleListCtrl()
{
    delete m_popup;
}

wxString BattleListCtrl::GetItemText(long item, long column) const
{
    if ( m_data[item] == NULL )
        return wxEmptyString;

    const IBattle& battle= *m_data[item];
    const BattleOptions& opts = battle.GetBattleOptions();

    switch ( column ) {
        case 0:
        case 1:
        case 2:
        default: return wxEmptyString;

        case 3: return ( opts.description );
		case 4: return ( battle.GetHostMapName() );
		case 5: return ( battle.GetHostModName() );
        case 6: return ( opts.founder );
		case 7: return wxFormat(_T("%d") ) % int(battle.GetSpectators());
		case 8: return wxFormat(_T("%d") ) % (int(battle.GetNumUsers()) - int(battle.GetSpectators()));
		case 9: return wxFormat(_T("%d") ) % int(battle.GetMaxPlayers());
        case 10: return ( wxTimeSpan(0/*h*/,0/*m*/,
                                     battle.GetBattleRunningTime()).Format(_T("%H:%M")) );
		case 11: return battle.GetEngineVersion();
    }
}

int BattleListCtrl::GetItemImage(long item) const
{
    if ( m_data[item] == NULL )
        return -1;

    return icons().GetBattleStatusIcon( *m_data[item] );
}

int BattleListCtrl::GetItemColumnImage(long item, long column) const
{
    if ( m_data[item] == NULL )
        return -1;

    const IBattle& battle= *m_data[item];

    switch ( column ) {
        default: return -1;

        case 0: return icons().GetBattleStatusIcon( battle );
        case 1:
        {
        	try
        	{
        	 return icons().GetFlagIcon( battle.GetFounder().GetCountry() );
        	}catch(...){}
					break;
        }
		case 2: return icons().GetRankLimitIcon( battle.GetRankNeeded(), false );
        case 4: return battle.MapExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
        case 5: return battle.ModExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
    }
    return -1; // simply to avoid compiler warning
}

wxListItemAttr* BattleListCtrl::GetItemAttr(long item) const
{
    if ( item < (long)m_data.size() && item > -1 ) {
        const IBattle& b = *m_data[item];
        try
        {
					wxString host = b.GetFounder().GetNick();
					wxListItemAttr* attr = HighlightItemUser( host );
					if ( attr != NULL )
							return attr;

					//to avoid color flicker check first if highlighting should be done
					//and return if it should
					for ( unsigned int i = 0; i < b.GetNumUsers(); ++i){
							wxString name = b.GetUser(i).GetNick();
							attr = HighlightItemUser( name );
							if ( attr != NULL )
									return attr;

					}
				}catch(...){}
    }
    return NULL;
}


void BattleListCtrl::AddBattle( IBattle& battle )
{
    if ( AddItem( &battle ) )
        return;

    wxLogWarning( _T("Battle already in list.") );
}

void BattleListCtrl::RemoveBattle( IBattle& battle )
{
    if ( RemoveItem( &battle ) )
        return;

    wxLogError( _T("Didn't find the battle to remove.") );
}

void BattleListCtrl::UpdateBattle( IBattle& battle )
{
    int index = GetIndexFromData( &battle );

    RefreshItem( index );
    MarkDirtySort();
}

void BattleListCtrl::OnListRightClick( wxListEvent& event )
{
    int idx = event.GetIndex();
    if ( idx < (long)m_data.size() && idx > -1 ) {

        DataType dt = m_data[idx];
        bool mod_missing = !dt->ModExists();
        bool map_missing = !dt->MapExists();
        m_popup = new wxMenu( _T("") );
        // &m enables shortcout "alt + m" and underlines m
        if ( map_missing )
            m_popup->Append( BLIST_DLMAP, _("Download &map") );

        if ( mod_missing )
			m_popup->Append( BLIST_DLMOD, _("Download &game") );

        if ( map_missing || mod_missing )
            PopupMenu( m_popup );
    }
}

void BattleListCtrl::OnDLMap( wxCommandEvent& /*unused*/  )
{
    if ( m_selected_index > 0 &&  (long)m_data.size() > m_selected_index ) {
        DataType dt = m_data[m_selected_index];
        ui().Download( _T("map"), dt->GetHostMapName(), dt->GetHostMapHash() );
    }
}

void BattleListCtrl::OnDLMod( wxCommandEvent& /*unused*/  )
{
    if ( m_selected_index > 0 &&  (long)m_data.size() > m_selected_index ) {
        DataType dt = m_data[m_selected_index];
        ui().Download(_T("game"), dt->GetHostModName(), dt->GetHostModHash() );
    }
}

void BattleListCtrl::Sort()
{
    if ( m_data.size() > 0 )
    {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}

int BattleListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const
{
    switch ( col ) {
        case 0: return dir * CompareStatus( u1, u2 );
        case 1:
        {
					try
					{
        	 return dir * u1->GetFounder().GetCountry().CmpNoCase( u2->GetFounder().GetCountry() );
        	 }catch(...){}
					break;
        }
        case 2: return dir * compareSimple( u1->GetRankNeeded(), u2->GetRankNeeded() );
        case 3: return dir * u1->GetDescription().CmpNoCase( u2->GetDescription() );
		case 4: return dir * u1->GetHostMapName().CmpNoCase( u2->GetHostMapName() );
		case 5: return dir * u1->GetHostModName().CmpNoCase( u2->GetHostModName() );
        case 6:
        {
        	try
					{
        	 return dir * u1->GetFounder().GetNick().CmpNoCase( u2->GetFounder().GetNick() );
        	 }catch(...){}
        	 break;
        }
        case 7: return dir * compareSimple( u1->GetSpectators(), u2->GetSpectators() );
        case 8: return dir * ComparePlayer( u1, u2 );
        case 9: return dir * compareSimple( u1->GetMaxPlayers(), u2->GetMaxPlayers() );
        case 10: return dir * compareSimple( u1->GetBattleRunningTime(), u2->GetBattleRunningTime());
        default: return 0;
    }
    return 0; // simply to avoid compiler warning
}

int BattleListCtrl::CompareStatus( DataType u1, DataType u2 )
{
  const IBattle& battle1 = *u1;
  const IBattle& battle2 = *u2;

  int b1 = 0, b2 = 0;

  if ( battle1.GetNumActivePlayers() == 0 )
  	b1 += 2000;
  if ( battle2.GetNumActivePlayers() == 0 )
  	b2 += 2000;
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


int BattleListCtrl::ComparePlayer( DataType u1, DataType u2 )
{
    const IBattle& battle1 = *u1;
    const IBattle& battle2 = *u2;

    int n1 = battle1.GetNumUsers() - battle1.GetSpectators();
    int n2 = battle2.GetNumUsers() - battle2.GetSpectators();
    return compareSimple( n1, n2 );
}

void BattleListCtrl::SetTipWindowText( const long item_hit, const wxPoint& position)
{
    if ( (long)m_data.size() < item_hit ) {
        m_tiptext = _T("");
        return;
    }

    const IBattle& battle= *m_data[item_hit];

    int column = getColumnFromPosition(position);
    switch (column)
    {
        case 0: // status
        m_tiptext = icons().GetBattleStatus(battle);
            break;
        case 1: // country
						try
						{
							m_tiptext = GetFlagNameFromCountryCode(battle.GetFounder().GetCountry());
            }catch(...){}
            break;
        case 2: // rank_min
            m_tiptext = m_colinfovec[column].tip;
            break;
        case 3: // descrp
            m_tiptext = battle.GetDescription();
            break;
        case 4: //map
			m_tiptext = battle.GetHostMapName();
            break;
        case 5: //mod
			m_tiptext = battle.GetHostModName();
            break;
        case 6: // host
					try
					{
            m_tiptext = battle.GetFounder().GetNick();
					}catch(...){}
            break;
        case 7: // specs
            m_tiptext = _("Spectators:");
            for (unsigned int i = 0; i < battle.GetNumUsers(); ++i )
            {
                if ( battle.GetUser(i).BattleStatus().spectator ) m_tiptext << _T("\n") << battle.GetUser(i).GetNick();
            }
            break;
        case 8: // player
            m_tiptext = _("Active Players:");
            for ( unsigned int i = 0; i < battle.GetNumUsers(); ++i )
            {
                if ( !battle.GetUser(i).BattleStatus().spectator ) m_tiptext << _T("\n") << battle.GetUser(i).GetNick();
            }
            break;
        case 9: //max player
            m_tiptext = (m_colinfovec[column].tip);
            break;
        case 10: //running time
            m_tiptext = (m_colinfovec[column].tip);
            break;
        default: m_tiptext = _T("");
            break;
    }
}


int BattleListCtrl::GetIndexFromData( const DataType& data ) const
{
    static long seekpos;
    seekpos = LSL::Util::Clamp( seekpos, 0l , (long)m_data.size() );
    int index = seekpos;

    for ( DataCIter f_idx = m_data.begin() + seekpos; f_idx != m_data.end() ; ++f_idx )
    {
        if ( *f_idx != 0 && data->Equals( *(*f_idx) ) )
        {
            seekpos = index;
            return seekpos;
        }
        index++;
    }
    //it's ok to init with seekpos, if it had changed this would not be reached
    int r_index = seekpos - 1;
    for ( DataRevCIter r_idx = m_data.rbegin() + ( m_data.size() - seekpos ); r_idx != m_data.rend() ; ++r_idx )
    {
        if ( *r_idx != 0 && data->Equals( *(*r_idx) ) )
        {
            seekpos = r_index;
            return seekpos;
        }
        r_index--;
    }

    return -1;
}

