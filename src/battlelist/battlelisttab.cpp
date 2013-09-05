/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/textdlg.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/log.h>
#include <stdexcept>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif
#include <set>

#include "aui/auimanager.h"
#include "battlelisttab.h"
#include "battlelistctrl.h"
#include "ui.h"
#include "chatpanel.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "utils/controls.h"
#include "uiutils.h"
#include "hosting/hostbattledialog_public.h"
#include "hosting/mainjoinbattletab.h"
#include "server.h"
#include "settings.h"
#include "mapctrl.h"
#include "nicklistctrl.h"
#include "mainwindow.h"
#include "battlelistfilter.h"
#include "iconimagelist.h"
#include "useractions.h"
#include "utils/customdialogs.h"

const unsigned int BATTLELIST_COLUMNCOUNT = 10;

BEGIN_EVENT_TABLE( BattleListTab, wxPanel )

	EVT_BUTTON              ( BattleListTab::BATTLE_JOIN ,              BattleListTab::OnJoin           )
	EVT_BUTTON              ( BattleListTab::BATTLE_HOST ,              BattleListTab::OnHost           )
	EVT_LIST_ITEM_ACTIVATED ( BattleListTab::BATTLE_JOIN ,              BattleListTab::OnListJoin       )
	EVT_LIST_ITEM_SELECTED  ( BattleListCtrl::BLIST_LIST  ,             BattleListTab::OnSelect         )
	EVT_CHECKBOX            ( BattleListTab::BATTLE_LIST_FILTER_ACTIV,  BattleListTab::OnFilterActiv    )
	#if  wxUSE_TOGGLEBTN
	EVT_TOGGLEBUTTON        ( BattleListTab::BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter         )
	EVT_TOGGLEBUTTON        ( BattleListTab::BATTLE_LIST_INFO_BUTTON,   BattleListTab::OnInfoShow       )
	#else
	EVT_CHECKBOX            ( BattleListTab::BATTLE_LIST_FILTER_BUTTON, BattleListTab::OnFilter         )
	EVT_CHECKBOX            ( BattleListTab::BATTLE_LIST_INFO_BUTTON,   BattleListTab::OnOnInfoShow     )
	#endif
	EVT_SIZE( BattleListTab::OnResize )


END_EVENT_TABLE()


BattleListTab::BattleListTab( wxWindow* parent )
    : wxScrolledWindow( parent, -1 ),
    m_sel_battle( 0 )
{
	GetAui().manager->AddPane( this, wxLEFT, _T( "battlelisttab" ) );

	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* m_filter_sizer;
	m_filter_sizer = new wxBoxSizer( wxVERTICAL );


	m_battlelist_sizer = new wxBoxSizer( wxVERTICAL );

	m_battle_list = new BattleListCtrl( this );
	m_battle_list->SetHighLightAction ( UserActions::ActHighlight );
	m_battlelist_sizer->Add( m_battle_list, 1, wxALL | wxEXPAND, 5 );

	m_main_sizer->Add( m_battlelist_sizer, 1, wxEXPAND, 5 );;

	m_info_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_minimap = new MapCtrl( this, 100, 0, true, true, false, false );
	m_info_sizer->Add( m_minimap, 0, wxALL, 5 );

	m_data_sizer = new wxFlexGridSizer( 4, 2, 0, 0 );

	m_map_lbl = new wxStaticText( this, wxID_ANY, _( "Map:" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_map_lbl, 0, wxALL, 5 );

	m_map_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_map_text, 0, wxALL, 5 );

	m_mod_lbl = new wxStaticText( this, wxID_ANY, _( "Game:" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_mod_lbl, 0, wxALL, 5 );

	m_mod_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_mod_text, 0, wxALL, 5 );

	m_players_lbl = new wxStaticText( this, wxID_ANY, _( "Players:" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_players_lbl, 0, wxALL, 5 );

	m_players_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_players_text, 0, wxALL, 5 );

	m_spec_lbl = new wxStaticText( this, wxID_ANY, _( "Spectators:" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_spec_lbl, 0, wxALL, 5 );

	m_spec_text = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_data_sizer->Add( m_spec_text, 0, wxALL, 5 );

	m_info_sizer->Add( m_data_sizer, 1, wxEXPAND, 5 );

	m_players = new NickListCtrl( this, false, 0, true, _T("battlelist_nicklist") );
	int width = m_players->GetClientSize().GetWidth() - 60;
	m_players->SetColumnWidth( 3, width );
	m_info_sizer->Add( m_players, 1, wxALL | wxEXPAND, 5 );

	m_main_sizer->Add( m_info_sizer, 0, wxEXPAND, 5 );


	m_filter = new BattleListFilter( this , wxID_ANY, this , wxDefaultPosition, wxSize( -1, -1 ), wxEXPAND );
	m_filter_sizer->Add( m_filter, 0, wxEXPAND, 5 );

	m_main_sizer->Add( m_filter_sizer, 0, wxEXPAND, 5 );

	m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_main_sizer->Add( m_buttons_sep, 0, wxALL | wxEXPAND, 5 );

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

#if  wxUSE_TOGGLEBTN
	m_filter_show = new wxToggleButton( this, BATTLE_LIST_FILTER_BUTTON , _( " Filter " ), wxDefaultPosition , wxSize( -1, 28 ), 0 );
#else
	m_filter_show = new wxCheckBox( this, BATTLE_LIST_FILTER_BUTTON , _( " Filter " ), wxDefaultPosition , wxSize( -1, 28 ), 0 );
#endif
	m_buttons_sizer->Add( m_filter_show, 0, 0, 5 );

	m_filter_activ = new wxCheckBox( this, BATTLE_LIST_FILTER_ACTIV , _( "Activated" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_filter_activ, 0, wxALL, 5 );

#if  wxUSE_TOGGLEBTN
	m_info_show = new wxToggleButton( this, BATTLE_LIST_INFO_BUTTON , _( " Battle infos " ), wxDefaultPosition , wxSize( -1, 28 ), 0 );
#else
	m_info_show = new wxCheckBox( this, BATTLE_LIST_INFO_BUTTON, _( " Battle infos " ), wxDefaultPosition , wxSize( -1, 28 ), 0 );
#endif
	m_buttons_sizer->Add( m_info_show , 0, 0, 5 );

	m_battle_num = new wxStaticText( this, wxID_ANY, _( "0 battles displayed" ), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttons_sizer->Add( m_battle_num, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 4 );
	m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

	m_host_btn = new wxButton( this, BATTLE_HOST, _( "Host new..." ), wxDefaultPosition, wxSize( -1, 28 ), 0 );
	m_buttons_sizer->Add( m_host_btn, 0, wxBOTTOM | wxLEFT | wxRIGHT, 5 );

	m_join_btn = new wxButton( this, BATTLE_JOIN, _( "Join" ), wxDefaultPosition, wxSize( -1, 28 ), 0 );
	m_buttons_sizer->Add( m_join_btn, 0, wxBOTTOM | wxRIGHT, 5 );

	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

	m_filter->Hide();

    SetScrollRate( SCROLL_RATE, SCROLL_RATE );
	SetSizer( m_main_sizer );
	Layout();

	SelectBattle( 0 );
	ConnectGlobalEvent(this, GlobalEvent::OnUnitsyncReloaded, wxObjectEventFunction(&BattleListTab::OnUnitsyncReloaded));
}


BattleListTab::~BattleListTab()
{
	if ( m_filter != 0 )
		m_filter->SaveFilterValues();
}

void BattleListTab::OnConnected()
{
	bool filter = sett().GetBattleFilterActivState();
	SetFilterActiv( filter );
}

void BattleListTab::SetNumDisplayed()
{
	int num = m_battle_list->GetItemCount();
	m_battle_num->SetLabel( wxFormat( _( "%d battles displayed" ) ) % num );
}

void BattleListTab::SelectBattle( IBattle* battle )
{
	m_sel_battle = battle;
	m_minimap->SetBattle( m_sel_battle );
	m_players->ClearUsers();
	if ( m_sel_battle != 0 )
	{
		m_map_text->SetLabel( m_sel_battle->GetHostMapName() );
		m_mod_text->SetLabel( m_sel_battle->GetHostModName() );
		m_players_text->SetLabel( wxFormat( _T( "%d / %d" ) ) % (int( m_sel_battle->GetNumUsers() ) - int( m_sel_battle->GetSpectators() )) % int( m_sel_battle->GetMaxPlayers() ) );
		m_spec_text->SetLabel( wxFormat( _T( "%d" ) ) % m_sel_battle->GetSpectators() );
		for ( unsigned int i = 0; i < m_sel_battle->GetNumUsers(); i++ )
		{
			User& usr = m_sel_battle->GetUser( i );
			if ( usr.BattleStatus().IsBot() ) continue;
			m_players->AddUser( usr );
		}
	} else
	{
		m_map_text->SetLabel( wxEmptyString );
		m_mod_text->SetLabel( wxEmptyString );
		m_players_text->SetLabel(  _T( "0 / 0" ) );
		m_spec_text->SetLabel( _T( "0" ) );
	}
}

void BattleListTab::AddBattle( IBattle& battle ) {
	if ( battle.GetGUIListActiv() || ( m_filter->GetActiv() && !m_filter->FilterBattle( battle ) ) ) {
		return;
	}
	m_battle_list->AddBattle( battle );
	battle.SetGUIListActiv( true );
	m_battle_list->MarkDirtySort();
	SetNumDisplayed();
}

void BattleListTab::RemoveBattle( IBattle& battle )
{
	if ( &battle == m_sel_battle ) {
		m_battle_list->ResetSelection();
		SelectBattle( 0 );
	}

	m_battle_list->RemoveBattle( battle );

	battle.SetGUIListActiv( false );
	SetNumDisplayed();
}

void BattleListTab::UserUpdate( User& user )
{
	if ( m_sel_battle && user.GetBattle() == m_sel_battle ) {
		m_players->UserUpdated( user );
	}
}


void BattleListTab::UpdateBattle( IBattle& battle )
{
	if ( !battle.GetGUIListActiv() ) {
		AddBattle( battle );
		return;
	}
	if ( m_filter->GetActiv() && !m_filter->FilterBattle( battle ) ) {
		RemoveBattle( battle );
		return;
	}
	m_battle_list->UpdateBattle( battle );
	if ( &battle == m_sel_battle )
		SelectBattle( m_sel_battle );
}


void BattleListTab::RemoveAllBattles()
{
	SelectBattle( 0 );
	serverSelector().GetServer().battles_iter->IteratorBegin();
	while ( ! serverSelector().GetServer().battles_iter->EOL() )
	{
		Battle* temp_battle = serverSelector().GetServer().battles_iter->GetBattle();
		if ( temp_battle != 0 )
			temp_battle->SetGUIListActiv( false );
	}
	m_battle_list->Clear();
	SetNumDisplayed();
}


void BattleListTab::UpdateList() {
	serverSelector().GetServer().battles_iter->IteratorBegin();
	while ( ! serverSelector().GetServer().battles_iter->EOL() ) {
		Battle* b = serverSelector().GetServer().battles_iter->GetBattle();
		if ( b != 0 )
			UpdateBattle( *b );
	}
	m_battle_list->RefreshVisibleItems();
}


void BattleListTab::SetFilterActiv( bool activ )
{
	m_filter->SetActiv( activ );
	m_filter_activ->SetValue( activ );
	sett().SetBattleFilterActivState( activ );
	m_battle_list->MarkDirtySort();
}

void BattleListTab::OnHost( wxCommandEvent& /*unused*/ )
{
	if ( !ui().IsConnected() )
	{
		wxLogWarning( _T( "Trying to host while offline" ) );
		customMessageBoxNoModal( SL_MAIN_ICON, _( "You cannot host a game while being offline. Please connect to a lobby server." ), _( "Not Online." ), wxOK );
		ui().ShowConnectWindow();
		return;
	}

	Battle* battle = ui().mw().GetJoinTab().GetCurrentBattle();
	if ( battle != 0 )
	{
		if ( ui().Ask( _( "Already in a battle" ), _( "You are already in a battle.\n\nDo you want to leave current battle to start a new?" ) ) ) {
			battle->Leave();
			ui().mw().GetJoinTab().LeaveCurrentBattle();
		}
		else
		{
			return;
		}
	}

	SL::RunHostBattleDialog( this );
}


void BattleListTab::OnFilter( wxCommandEvent& /*unused*/ )
{
	if ( m_filter_show->GetValue() )
	{
		m_filter->Show(  );
		this->Layout();
	}
	else
	{
		m_filter->Hide(  );
		this->Layout();
	}
}


void BattleListTab::OnFilterActiv( wxCommandEvent& /*unused*/ )
{
	bool active = m_filter_activ->GetValue();
	if ( !ui().IsConnected() )
	{
		m_filter_activ->SetValue( !active );
		return;
	}
	m_filter->SetActiv( active );
	sett().SetBattleFilterActivState( active );
	SetNumDisplayed();
}

void BattleListTab::OnJoin( wxCommandEvent& /*unused*/ )
{
	try
	{
		ASSERT_LOGIC( m_battle_list != 0, _T( "m_battle_list = 0" ) );
	} catch ( ... ) {
		return;
	}

	if ( m_battle_list->GetSelectedIndex() < 0 ) return;

	int id = m_battle_list->GetSelectedData()->GetBattleId();
	DoJoin( serverSelector().GetServer().battles_iter->GetBattle( id ) );
}


void BattleListTab::OnListJoin( wxListEvent& event )
{
	try
	{
		ASSERT_LOGIC( m_battle_list != 0, _T( "m_battle_list = 0" ) );
	} catch ( ... ) {
		return;
	}
	if ( event.GetIndex() < 0 ) return;

	int id = m_battle_list->GetSelectedData()->GetBattleId();
	DoJoin( serverSelector().GetServer().battles_iter->GetBattle( id ) );
}


void BattleListTab::DoJoin( Battle& battle )
{

	if ( !ui().IsSpringCompatible(battle.GetBattleOptions().engineName, battle.GetBattleOptions().engineVersion) )
	{
        wxLogWarning( _T( "trying to join battles with incompatible spring version" ) );
//		customMessageBox( SL_MAIN_ICON, _( "Joining battles is disabled due to the incompatible spring version you're using." ), _( "Spring error" ), wxICON_EXCLAMATION | wxOK );
		return;
	}

	Battle* curbattle = ui().mw().GetJoinTab().GetCurrentBattle();

    //handle matchmakers
    const std::set<int> cpus = sett().KnownMatchmakerCPU();
    if ( cpus.find(battle.GetFounder().GetCpu()) != cpus.end() )
    {
        const wxString msg(_( "You are about to join a matchmaking battleroom.\nThe host might move you to another battle at any time. In case of problems please contact the host or a lobby moderator.\nDo you want to continue?" ));
        if ( !ui().Ask( _( "Matchmaker detected" ), msg ) )
            return;
    }

	if ( curbattle != 0 && curbattle->GetID() == battle.GetID() )
	{
        ui().mw().GetJoinTab().FocusBattleRoomTab();
        return;
	}

	if ( curbattle != 0 )
	{
		if ( ui().Ask( _( "Already in another battle" ), _( "You are already in a battle.\n\nDo you want to leave your current battle and join this one?" ) ) ) {
			curbattle->Leave();
			ui().mw().GetJoinTab().LeaveCurrentBattle();
		}
		else
		{
			return;
		}
	}

    const wxString downloadProc = _( "Should I try to download it for you?\nYou'll be notified once it's complete" );
	if ( !battle.ModExists() )
	{
        if ( customMessageBox( SL_MAIN_ICON, _( "You need to download the game before you can join this game.\n\n" ) + downloadProc,
                               _( "Game not available" ), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
			wxString modhash = battle.GetHostModHash();
			wxString modname = battle.GetHostModName();
			ui().Download ( _T("game"), modname, modhash);
		}
        else
            return;
	}

	if ( !battle.MapExists() )
	{
        if ( customMessageBox( SL_MAIN_ICON, _( "You need to download the map to be able to play in this game.\n\n" ) + downloadProc,
                               _( "Map not available" ), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
			wxString maphash = battle.GetHostMapHash();
			wxString mapname = battle.GetHostMapName();
			ui().Download ( _T("map"), mapname, maphash);
		}
        else
            return;
	}

	if ( battle.IsPassworded() )
	{
		wxPasswordEntryDialog pw( this, _( "Battle password" ), _( "Enter password (spaces will be stripped)" ) );
		pw.SetFocus();
		if ( pw.ShowModal() == wxID_OK )
		{
			wxString password = pw.GetValue();
			password.Replace(_T(" "), _T(""));
			battle.Join( password );
		}
	}
	else
	{
		battle.Join();
	}
}


void BattleListTab::OnSelect( wxListEvent& event )
{
	wxLogDebugFunc( _T( "" ) );
	if ( event.GetIndex() == -1 )
	{
		SelectBattle( 0 );
	} else {
		IBattle* b = ( m_battle_list->GetDataFromIndex( event.GetIndex() ) ) ;
		SelectBattle( b );//
	}
}


void BattleListTab::OnUnitsyncReloaded( wxCommandEvent& /*data*/ )
{
	assert(wxThread::IsMain());
	if ( ! serverSelector().GetServerStatus() )
		return;

	UpdateList();
}

void BattleListTab::UpdateHighlights()
{
	m_battle_list->RefreshVisibleItems();
}


void BattleListTab::SortBattleList()
{
	m_battle_list->SortList();
}

void BattleListTab::ShowExtendedInfos( bool show )
{
	const bool recursive = true;
	m_main_sizer->Show( m_info_sizer, show, recursive );
	m_main_sizer->Show( m_buttons_sep, show, recursive  );
	m_info_show->SetValue( show );
	Layout();
}

void BattleListTab::OnInfoShow( wxCommandEvent& /*unused*/ )
{
	ShowExtendedInfos( m_info_show->GetValue() );
}

void BattleListTab::OnResize( wxSizeEvent& event )
{
	SetSize( event.GetSize() );
	Layout();
	// window too small, hide additional infos
	ShowExtendedInfos( ( GetClientSize().GetHeight() > 400 ) );
}

