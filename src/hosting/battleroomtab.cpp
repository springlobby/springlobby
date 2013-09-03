/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: BattleRoomTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/choicdlg.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <wx/log.h>
#include <wx/bmpcbox.h>
#include <wx/textdlg.h>
#include <wx/image.h>
#include <wx/choice.h>
#include <wx/numdlg.h>

#include <stdexcept>

#include "battleroomtab.h"
#include "ui.h"
#include <lslunitsync/unitsync.h>
#include "user.h"
#include "battle.h"
#include "defines.h"
#include "utils/conversion.h"
#include "utils/debug.h"
#include "utils/uievents.h"
#include "defines.h"
#include "battleroomlistctrl.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"
#include "addbotdialog.h"
#include "server.h"
#include "iconimagelist.h"
#include "utils/customdialogs.h"
#include "autobalancedialog.h"
#include "settings.h"
#include "helper/colorbutton.h"
#include "mapselectdialog.h"
#include "mmoptionwindows.h"
#include "aui/auimanager.h"
#include "hostbattledialog_public.h"

BEGIN_EVENT_TABLE( BattleRoomTab, wxPanel )

	EVT_BUTTON              ( BROOM_START,                  BattleRoomTab::OnStart                  )
	EVT_BUTTON              ( BROOM_LEAVE,                  BattleRoomTab::OnLeave                  )
	EVT_BUTTON              ( BROOM_ADDBOT,                 BattleRoomTab::OnAddBot                 )
	EVT_BUTTON              ( BROOM_HOST_NEW,               BattleRoomTab::OnHostNew                )

	EVT_CHECKBOX            ( BROOM_IMREADY,                BattleRoomTab::OnImReady                )
	EVT_CHECKBOX            ( BROOM_SPEC,                   BattleRoomTab::OnImSpec                 )
	EVT_CHECKBOX            ( BROOM_UNSPEC,                 BattleRoomTab::OnAutounSpec             )
	EVT_COMBOBOX            ( BROOM_TEAMSEL,                BattleRoomTab::OnTeamSel                )
	EVT_COMBOBOX            ( BROOM_ALLYSEL,                BattleRoomTab::OnAllySel                )
	EVT_BUTTON              ( BROOM_COLOURSEL,              BattleRoomTab::OnColourSel              )
	EVT_COMBOBOX            ( BROOM_SIDESEL,                BattleRoomTab::OnSideSel                )

	EVT_COMBOBOX            ( BROOM_PRESETSEL,              BattleRoomTab::OnPresetSel              )
	EVT_BUTTON              ( BROOM_SAVEPRES,               BattleRoomTab::OnSavePreset             )
	EVT_BUTTON              ( BROOM_DELETEPRES,             BattleRoomTab::OnDeletePreset           )
	EVT_BUTTON              ( BROOM_SETDEFAULTPRES,         BattleRoomTab::OnSetModDefaultPreset    )

	EVT_BUTTON              ( BROOM_MAP_BROWSE,             BattleRoomTab::OnMapBrowse              )
	EVT_COMBOBOX            ( BROOM_MAP_SEL,                BattleRoomTab::OnMapSelect              )

	EVT_CHECKBOX            ( BROOM_LOCK,                   BattleRoomTab::OnLock                   )
	EVT_CHECKBOX            ( BROOM_AUTOLOCK,               BattleRoomTab::OnAutoLock               )

    EVT_BUTTON              ( BROOM_MANAGE_MENU,            BattleRoomTab::OnShowManagePlayersMenu  )

    EVT_MENU                ( BROOM_AUTOHOST,               BattleRoomTab::OnAutoHost               )
	EVT_MENU                ( BROOM_AUTOSPECT,              BattleRoomTab::OnAutoSpec               )
	EVT_MENU                ( BROOM_AUTOSTART,              BattleRoomTab::OnAutoStart              )
    EVT_MENU                ( BROOM_AUTOCONTROL,            BattleRoomTab::OnAutoControl            )

    EVT_MENU                ( BROOM_RING_UNREADY,           BattleRoomTab::OnRingUnready            )
	EVT_MENU                ( BROOM_RING_UNSYNC,            BattleRoomTab::OnRingUnsynced           )
	EVT_MENU                ( BROOM_RING_UNREADY_UNSYNC,    BattleRoomTab::OnRingUnreadyUnsynced    )

	EVT_MENU                ( BROOM_SPECT_UNREADY,          BattleRoomTab::OnSpectUnready           )
	EVT_MENU                ( BROOM_SPECT_UNSYNC,           BattleRoomTab::OnSpectUnsynced          )
	EVT_MENU                ( BROOM_SPECT_UNREADY_UNSYNC,   BattleRoomTab::OnSpectUnreadyUnsynced   )

    EVT_MENU                ( BROOM_LOCK_BALANCE,           BattleRoomTab::OnLockBalance            )
	EVT_MENU                ( BROOM_BALANCE,                BattleRoomTab::OnBalance                )
	EVT_MENU                ( BROOM_FIXID,                  BattleRoomTab::OnFixTeams               )
	EVT_MENU                ( BROOM_FIXCOLOURS,             BattleRoomTab::OnFixColours             )
	EVT_MENU                ( BROOM_AUTOPASTE,              BattleRoomTab::OnAutoPaste              )

	EVT_MENU                ( BROOM_AUTOHOST_BALANCE,       BattleRoomTab::OnAutohostBalance        )
	EVT_MENU                ( BROOM_AUTOHOST_RANDOMMAP,     BattleRoomTab::OnAutohostRandomMap      )
	EVT_MENU                ( BROOM_AUTOHOST_NOTIFY,        BattleRoomTab::OnAutohostNotify         )


	EVT_LIST_ITEM_ACTIVATED ( BROOM_OPTIONLIST,             BattleRoomTab::OnOptionActivate         )

END_EVENT_TABLE()

template < int N, int S = 1 >
class MyStrings : public wxArrayString
{
	public:
		MyStrings()
		{
			for ( int i = S; i <= N; ++i )
				Add( TowxString( i ) );
		}
};

const MyStrings<SPRING_MAX_TEAMS> team_choices;
const MyStrings<SPRING_MAX_ALLIES> ally_choices;

BattleRoomTab::BattleRoomTab( wxWindow* parent, Battle* battle )
    : wxScrolledWindow( parent, -1 ),
	m_battle( battle ),
	m_BattleActionSink( this, &UiEvents::GetUiEventSender( UiEvents::OnBattleActionEvent ))
{
	GetAui().manager->AddPane( this, wxLEFT, _T( "battleroomtab" ) );

	// Create all widgets
	m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxSize( 100, 60 ) );

	m_player_panel = new wxScrolledWindow( m_splitter , -1 );
	m_player_panel->SetScrollRate( SCROLL_RATE, SCROLL_RATE );
	m_team_sel = new wxComboBox( m_player_panel, BROOM_TEAMSEL, _T( "1" ), wxDefaultPosition, wxSize( 50, CONTROL_HEIGHT ), team_choices );
	m_team_sel->SetToolTip( TE( _( "Players with the same team number share control of their units." ) ) );
	m_ally_sel = new wxComboBox( m_player_panel, BROOM_ALLYSEL, _T( "1" ), wxDefaultPosition, wxSize( 50, CONTROL_HEIGHT ), ally_choices );
	m_ally_sel->SetToolTip( TE( _( "Players with the same ally number work together to achieve victory." ) ) );
	m_color_sel = new ColorButton( m_player_panel, BROOM_COLOURSEL, wxColour (0,0,0), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_color_sel->SetToolTip( TE( _( "Select a color to identify your units in-game" ) ) );
	m_side_sel = new wxBitmapComboBox( m_player_panel, BROOM_SIDESEL, _T( "" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_side_sel->SetToolTip( TE( _( "Select your faction" ) ) );
	m_spec_chk = new wxCheckBox( m_player_panel, BROOM_SPEC, _( "Spectator" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_spec_chk->SetToolTip( TE( _( "Spectate (watch) the battle instead of playing" ) ) );
	m_auto_unspec_chk = new wxCheckBox( m_player_panel, BROOM_UNSPEC, _( "Auto un-spectate" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_auto_unspec_chk->SetToolTip( TE( _( "automatically unspec when there's a free slot" ) ) );
	m_ready_chk = new wxCheckBox( m_player_panel, BROOM_IMREADY, _( "I'm ready" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_ready_chk->SetToolTip( TE( _( "Click this if you are content with the battle settings." ) ) );

	m_team_lbl = new wxStaticText( m_player_panel, -1, _( "Team" ) );
	m_ally_lbl = new wxStaticText( m_player_panel, -1, _( "Ally" ) );
	m_color_lbl = new wxStaticText( m_player_panel, -1, _( "Color" ) );
	m_side_lbl = new wxStaticText( m_player_panel, -1, _( "Side" ) );

	m_ally_setup_lbl = new wxStaticText( m_player_panel, -1, _( "Setup: " ) );
	m_ok_count_lbl = new wxStaticText( m_player_panel, -1, _( "Unready: 0" ) );

	//XXX not needed ?
//	m_size_lbl = new wxStaticText( this, -1, _T( "" ) );
//	m_wind_lbl = new wxStaticText( this, -1, _T( "" ) );
//	m_tidal_lbl = new wxStaticText( this, -1, _T( "" ) );

	m_map_combo = new wxComboBox( this, BROOM_MAP_SEL, _T( "" ), wxDefaultPosition, wxDefaultSize );

	m_minimap = new MapCtrl( this, 162, m_battle, true, true, true, false );
	m_minimap->SetToolTip( TE( _( "A preview of the selected map.  You can see the starting positions, or (if set) starting boxes." ) ) );

	m_browse_map_btn = new wxButton( this, BROOM_MAP_BROWSE, _( "Map" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_browse_map_btn->SetSize( m_browse_map_btn->GetSize().GetWidth() * 2 , m_browse_map_btn->GetSize().GetHeight() ) ; // has 0 effect

	m_players = new BattleroomListCtrl( m_player_panel, m_battle, false, true );
	m_chat = new ChatPanel( m_splitter, m_battle );

	m_command_line = new wxStaticLine( this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );

	m_host_new_btn = new wxButton( this, BROOM_HOST_NEW, _( "Host new" ), wxDefaultPosition, wxDefaultSize );
	m_host_new_btn->SetToolTip( TE( _( "Host a new battle" ) ) );
	m_leave_btn = new wxButton( this, BROOM_LEAVE, _( "Leave" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_leave_btn->SetToolTip( TE( _( "Leave the battle and return to the battle list" ) ) );
	m_start_btn = new wxButton( this, BROOM_START, _( "Start" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_start_btn->SetToolTip( TE( _( "Start the battle" ) ) );

	m_manage_players_btn = new wxButton( this, BROOM_MANAGE_MENU, _( "Player Management" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_manage_players_btn->SetToolTip( TE( _( "Various functions to make team games simplers to setup" ) ) );

	m_addbot_btn = new wxButton( this, BROOM_ADDBOT, _( "Add Bot..." ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_addbot_btn->SetToolTip( TE( _( "Add a computer-controlled player to the game" ) ) );
	m_autolock_chk = new wxCheckBox( this, BROOM_AUTOLOCK, _( "Autolock on start" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_autolock_chk->SetToolTip( TE( _( "Automatically locks the battle when the game starts and unlock when it's finished." ) ) );
	m_autolock_chk->SetValue( sett().GetLastAutolockStatus() );

	m_lock_chk = new wxCheckBox( this, BROOM_LOCK, _( "Locked" ), wxDefaultPosition, wxSize( -1, CONTROL_HEIGHT ) );
	m_lock_chk->SetToolTip( TE( _( "Prevent additional players from joining the battle" ) ) );

	m_manage_users_mnu = new wxMenu();

	m_autohost_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_AUTOHOST, _( "Autohost" ), _( "Toggle autohost mode.  This allows players to control your battle using commands like '!balance' and '!start'." ), wxITEM_CHECK );
	m_manage_users_mnu->Append( m_autohost_mnu );
	m_autohost_mnu->Check( false );


	m_autopaste_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_AUTOPASTE, _( "AutoPaste Description" ), _( "Automatically paste battle's descriptoin when a new user joins" ), wxITEM_CHECK );
	m_manage_users_mnu->Append( m_autopaste_mnu );
	m_autopaste_mnu->Check( sett().GetBattleLastAutoAnnounceDescription() );
	m_autospec_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_AUTOSPECT, _( "AutoSpect" ), _( "Automatically spectate players that don't ready up or become synced within x seconds." ), wxITEM_CHECK );
	m_manage_users_mnu->Append( m_autospec_mnu );
	m_autospec_mnu->Check( sett().GetBattleLastAutoSpectTime() > 0 );
	m_autocontrol_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_AUTOCONTROL, _( "AutoControlBalance" ), _( "Automatically balance teams and allies and fix colors when all players are ready and synced" ), wxITEM_CHECK );
	m_manage_users_mnu->Append( m_autocontrol_mnu );
	m_autocontrol_mnu->Check( sett().GetBattleLastAutoControlState() );
	m_autostart_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_AUTOSTART, _( "AutoStart" ), _( "Automatically start the battle when all players are ready and synced" ), wxITEM_CHECK );
	m_manage_users_mnu->Append( m_autostart_mnu );
	m_autostart_mnu->Check( sett().GetBattleLastAutoStartState() );

	m_lock_balance_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_LOCK_BALANCE, _( "Lock Balance" ), _( "When activated, prevents anyone but the host to change team and ally" ), wxITEM_CHECK );
	m_manage_users_mnu->Append( m_lock_balance_mnu );
	m_lock_balance_mnu->Check( false );

	wxMenu* ring_menu = new wxMenu;
	wxMenuItem* ring_unready = new wxMenuItem( ring_menu, BROOM_RING_UNREADY, _( "Ring unready" ), _( "Rings all players that don't have ready status and aren't spectators" ) );
	ring_menu->Append( ring_unready );
	wxMenuItem* ring_unsynced = new wxMenuItem( ring_menu, BROOM_RING_UNSYNC, _( "Ring unsynced" ), _( "Rings all players that don't have sync status and aren't spectators" ) );
	ring_menu->Append( ring_unsynced );
	wxMenuItem* ring_unready_unsynced = new wxMenuItem( ring_menu, BROOM_RING_UNREADY_UNSYNC, _( "Ring unready and unsynced" ), _( "Rings all players that don't have sync status or don't have ready status and aren't spectators" ) );
	ring_menu->Append( ring_unready_unsynced );
	m_manage_users_mnu->Append( wxID_ANY, _( "Ring ..." ), ring_menu );

	wxMenu* spect_menu = new wxMenu;
	wxMenuItem* spect_unready = new wxMenuItem( spect_menu, BROOM_SPECT_UNREADY, _( "Spect unready" ), _( "Force to spectate all players that don't have ready status" ) );
	spect_menu->Append( spect_unready );
	wxMenuItem* spect_unsynced = new wxMenuItem( spect_menu, BROOM_SPECT_UNSYNC, _( "Spect unsynced" ), _( "Force to spectate all players that don't have sync status" ) );
	spect_menu->Append( spect_unsynced );
	wxMenuItem* spect_unready_unsynced = new wxMenuItem( spect_menu, BROOM_SPECT_UNREADY_UNSYNC, _( "Force to spectate unready and unsynced" ), _( "Rings all players that don't have sync status or don't have ready status" ) );
	spect_menu->Append( spect_unready_unsynced );
	m_manage_users_mnu->Append( wxID_ANY, _( "Force spectate ..." ), spect_menu );

	wxMenuItem* m_balance_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_BALANCE, _( "Balance alliances" ), _( "Automatically balance players into two or more alliances" ) );
	m_manage_users_mnu->Append( m_balance_mnu );

	wxMenuItem* m_fix_colours_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_FIXCOLOURS, _( "Fix colours" ), _( "Make player colors unique" ) );
	m_manage_users_mnu->Append( m_fix_colours_mnu );

	wxMenuItem* m_fix_team_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_FIXID, _( "Balance teams" ), _( "Automatically balance players into control teams, by default none shares control" ) );
	m_manage_users_mnu->Append( m_fix_team_mnu );

	wxStaticBoxSizer* m_preset_sizer;
	m_preset_sizer = new wxStaticBoxSizer( new wxStaticBox( this, 0, _( "Manage Presets" ) ), wxVERTICAL );

	wxBoxSizer* m_preset_btns_sizer;
	m_preset_btns_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_options_preset_sel = new wxComboBox( this, BROOM_PRESETSEL, _T(""), wxDefaultPosition, wxDefaultSize,  sett().GetPresetList(), wxCB_READONLY );
	m_options_preset_sel->SetToolTip( TE( _( "Load battle preset" ) ) );

	m_preset_sizer->Add( m_options_preset_sel, 0, wxEXPAND | wxALL );

	m_save_btn = new wxButton( this, BROOM_SAVEPRES, _( "Save" ), wxDefaultPosition, wxDefaultSize );
	m_save_btn->SetToolTip( TE( _( "Save a set of options." ) ) );

	m_preset_btns_sizer->Add( m_save_btn, 0, wxEXPAND );

	m_delete_btn = new wxButton( this, BROOM_DELETEPRES, _( "Delete" ), wxDefaultPosition, wxDefaultSize );
	m_delete_btn->SetToolTip( TE( _( "Delete a set of options." ) ) );

	m_preset_btns_sizer->Add( m_delete_btn, 0, wxEXPAND );

	m_default_btn = new wxButton( this, BROOM_SETDEFAULTPRES, _( "Set default" ), wxDefaultPosition, wxDefaultSize );
	m_default_btn->SetToolTip( TE( _( "Use the current set of options as game's default." ) ) );

	m_preset_btns_sizer->Add( m_default_btn, 0, wxEXPAND );

	m_preset_sizer->Add( m_preset_btns_sizer, 0, wxEXPAND );


	m_opts_list = new wxListCtrl( this, BROOM_OPTIONLIST, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT );
	//m_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_opts_list->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );
	m_opts_list->SetToolTip( TE( _( "Activate an element to quickly change it" ) ) );
	wxListItem col;

	col.SetText( _( "Option" ) );
	m_opts_list->InsertColumn( 0, col );
	col.SetText( _( "Value" ) );
	m_opts_list->InsertColumn( 1, col );

	// Create Sizers
	m_players_sizer = new wxBoxSizer( wxVERTICAL );
	m_player_sett_sizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* m_map_select_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_info_sizer = new wxBoxSizer( wxVERTICAL );
	m_top_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
	//m_info1_sizer = new wxBoxSizer( wxHORIZONTAL );
	m_main_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* m_side_sel_sizer = new wxBoxSizer( wxHORIZONTAL );
	#ifndef HAVE_WX29
		int side_sel_width = m_side_sel->GetWidestItemWidth();
		m_side_sel_sizer->SetMinSize( side_sel_width, CONTROL_HEIGHT );
	#endif
	m_side_sel_sizer->Add( m_side_sel, 1, wxEXPAND );

	// Put widgets in place
	m_player_sett_sizer->Add( m_team_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 );
	m_player_sett_sizer->Add( m_team_sel, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->Add( m_ally_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 );
	m_player_sett_sizer->Add( m_ally_sel, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->Add( m_color_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 );
	m_player_sett_sizer->Add( m_color_sel, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->Add( m_side_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2 );
	m_player_sett_sizer->Add( m_side_sel_sizer, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->Add( m_spec_chk, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->Add( m_auto_unspec_chk, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->Add( m_ready_chk, 0, wxEXPAND | wxALL, 2 );
	m_player_sett_sizer->AddStretchSpacer();
	m_player_sett_sizer->Add( m_ally_setup_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 2 );
	m_player_sett_sizer->Add( m_ok_count_lbl, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT | wxALL, 2 );

	m_players_sizer->Add( m_players, 1, wxEXPAND );
	m_players_sizer->Add( m_player_sett_sizer, 0, wxEXPAND );

	m_player_panel->SetSizer( m_players_sizer );

	SplitSizerHorizontally( sett().GetSplitBRoomHorizontally() );

	//m_info1_sizer->Add( m_wind_lbl, 1, wxEXPAND );
	//m_info1_sizer->Add( m_size_lbl, 1, wxEXPAND );

	m_info_sizer->Add( m_minimap, 0, wxEXPAND );
	m_map_select_sizer->Add( m_map_combo, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL );
	m_map_select_sizer->Add( m_browse_map_btn, 0, wxALIGN_RIGHT );
	m_info_sizer->Add( m_map_select_sizer, 0, wxALL );
	//m_info_sizer->Add( m_info1_sizer, 0, wxEXPAND );
	//m_info_sizer->Add( m_tidal_lbl, 0, wxEXPAND );
	m_info_sizer->Add( m_opts_list, 1, wxEXPAND | wxTOP, 4 );
	m_info_sizer->Add( m_preset_sizer, 0, wxEXPAND, 4 );


	m_top_sizer->Add( m_splitter, 1, wxEXPAND | wxALL, 2 );
	m_top_sizer->Add( m_info_sizer, 0, wxEXPAND | wxALL, 2 );

	m_buttons_sizer->AddStretchSpacer();
	m_buttons_sizer->Add( m_host_new_btn, 0, wxEXPAND | wxALL, 2 );
	m_buttons_sizer->AddStretchSpacer();
	m_buttons_sizer->Add( m_leave_btn, 0, wxEXPAND | wxALL, 2 );
	m_buttons_sizer->Add( m_addbot_btn, 0, wxEXPAND | wxALL, 2 );
	m_buttons_sizer->Add( m_autolock_chk, 0, wxEXPAND | wxALL, 2 );
	m_buttons_sizer->Add( m_lock_chk, 0, wxEXPAND | wxALL, 2 );
	m_buttons_sizer->Add( m_manage_players_btn, 0, wxEXPAND | wxALL, 2 );
	m_buttons_sizer->Add( m_start_btn, 0, wxEXPAND | wxALL, 2 );

	m_main_sizer->Add( m_top_sizer, 1, wxEXPAND );
	m_main_sizer->Add( m_command_line, 0, wxEXPAND );
	m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );

	m_splitter->SetMinimumPaneSize( 240 );

	SetBattle( battle );

	SetScrollRate( SCROLL_RATE, SCROLL_RATE );
	SetSizer( m_main_sizer );
	Layout();
	ConnectGlobalEvent(this, GlobalEvent::OnUnitsyncReloaded, wxObjectEventFunction(&BattleRoomTab::OnUnitsyncReloaded));
	unsigned int widthfraction = m_opts_list->GetClientSize().GetWidth() / 3;
	m_opts_list->SetColumnWidth( 0, widthfraction * 1.95 );
	m_opts_list->SetColumnWidth( 1, widthfraction * 0.95 );

}


BattleRoomTab::~BattleRoomTab()
{
	if ( GetAui().manager )
        GetAui().manager->DetachPane( this );
}

void BattleRoomTab::SplitSizerHorizontally( const bool horizontal )
{
	if ( m_splitter->IsSplit() )
		m_splitter->Unsplit();
	if ( horizontal )
		m_splitter->SplitHorizontally( m_player_panel, m_chat );
	else
		m_splitter->SplitVertically( m_player_panel, m_chat );
}

wxString _GetStartPosStr( IBattle::StartType t )
{
	switch ( t )
	{
		case IBattle::ST_Fixed:
			return _( "Fixed" );
		case IBattle::ST_Random:
			return _( "Random" );
		case IBattle::ST_Choose:
			return _( "Boxes" );
		case IBattle::ST_Pick:
			return _( "Pick" );
		default:
			return _T( "?" );
	};
}


wxString _GetGameTypeStr( IBattle::GameType t )
{
	switch ( t )
	{
		case IBattle::GT_ComContinue:
			return _( "Continue" );
		case IBattle::GT_ComEnds:
			return _( "End" );
		case IBattle::GT_Lineage:
			return _( "Lineage" );
		default:
			return _T( "?" );
	};
}


void BattleRoomTab::UpdateBattleInfo()
{
	if ( !m_battle ) return;
	m_lock_chk->SetValue( m_battle->IsLocked() );
	m_minimap->UpdateMinimap();
	UpdateMapInfoSummary();
}

void BattleRoomTab::PrintAllySetup()
{
	wxString setupstring;
	if ( m_battle )
	{
		setupstring += _T("<") + TowxString( m_battle->GetSpectators() ) + _T("> ");
		setupstring += _T(" [") + TowxString( m_battle->GetNumActivePlayers() ) + _T("]: ");
		wxString alliancesstring;
		int previousalliancesize = 0;
		bool ffamode = true;
		std::map<int, int> allysizes = m_battle->GetAllySizes();
		if ( allysizes.size() < 3 ) ffamode = false;
		for ( std::map<int, int>::const_iterator itor = allysizes.begin(); itor != allysizes.end(); ++itor )
		{
			if ( itor != allysizes.begin() )
			{
				alliancesstring += _T("v");
				if (previousalliancesize != itor->second) ffamode = false;
			}
			else previousalliancesize = itor->second;
			alliancesstring += TowxString( itor->second );
			//this takes space needlessy
			//alliancesstring += wxFormat( _T("(%d)") , itor->first );
		}
		if ( !ffamode ) setupstring += alliancesstring;
		else setupstring += wxFormat(_("%d way FFA") ) % previousalliancesize;
	}
	m_ally_setup_lbl->SetLabel( wxFormat( _( "Setup: %s" ) ) % setupstring );
	Layout();
}

void BattleRoomTab::UpdateBattleInfo( const wxString& Tag )
{
  if ( !m_battle ) return;

	long index = m_opt_list_map[ Tag ];
	LSL::OptionsWrapper::GameOption type = ( LSL::OptionsWrapper::GameOption )s2l( Tag.BeforeFirst( '_' ) );
	wxString key = Tag.AfterFirst( '_' );
	wxString value;
	if ( ( type == LSL::OptionsWrapper::MapOption ) || ( type == LSL::OptionsWrapper::ModOption ) || ( type == LSL::OptionsWrapper::EngineOption ) )
	{
		LSL::Enum::OptionType DataType = m_battle->CustomBattleOptions().GetSingleOptionType( STD_STRING(key) );
		value = TowxString(m_battle->CustomBattleOptions().getSingleValue(STD_STRING(key), ( LSL::OptionsWrapper::GameOption )type ));
		if ( TowxString(m_battle->CustomBattleOptions().getDefaultValue( STD_STRING(key), type )) == value) m_opts_list->SetItemFont( index, wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );
		else m_opts_list->SetItemFont( index, wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
		if ( DataType == LSL::Enum::opt_bool )
		{
			value =  bool2yn( s2l( value ) ); // convert from 0/1 to literal Yes/No
		}
		else if ( DataType == LSL::Enum::opt_list )
		{
			value = TowxString(m_battle->CustomBattleOptions().GetNameListOptValue(STD_STRING(key), type )); // get the key full name not short key
		}
		m_opts_list->SetItem( index, 1, value );
	}
	else// if ( type == OptionsWrapper::PrivateOptions )
	{
		if ( key == _T( "mapname" ) ) // the map has been changed
		{
		    UpdateMapInfoSummary();

			wxString mapname =m_battle->GetHostMapName();
			int index_ = m_map_combo->FindString( mapname );
			if ( index_ != wxNOT_FOUND )
                m_map_combo->SetSelection( index_ );
			else
                m_map_combo->SetValue( mapname );

			//delete any eventual map option from the list and add options of the new map
			for ( int i = m_map_opts_index; i < m_opts_list->GetItemCount(); )
			{
                m_opts_list->DeleteItem( i );
			}
			AddMMOptionsToList( m_map_opts_index, LSL::OptionsWrapper::MapOption );

			m_minimap->UpdateMinimap();

		}
		else if ( key == _T( "restrictions" ) )
		{
			m_opts_list->SetItem( index, 1, bool2yn( m_battle->RestrictedUnits().size() > 0 ) );
		}
	}
}

BattleroomListCtrl& BattleRoomTab::GetPlayersListCtrl()
{
	ASSERT_LOGIC( m_players != 0, _T( "m_players = 0" ) );
	return *m_players;
}

void BattleRoomTab::UpdateMapInfoSummary()
{
	try   // updates map info summary
	{
		ASSERT_EXCEPTION( m_battle->MapExists(), _( "Map does not exist." ) );
		LSL::UnitsyncMap map = m_battle->LoadMap();
		m_opts_list->SetItem( m_opt_list_map[ _( "Size" ) ] , 1, wxFormat( _T( "%.0fx%.0f" ) ) % (map.info.width / 512.0) % (map.info.height / 512.0 ) );
		m_opts_list->SetItem( m_opt_list_map[ _( "Windspeed" ) ], 1, wxFormat( _T( "%d-%d" ) ) % map.info.minWind % map.info.maxWind );
		m_opts_list->SetItem( m_opt_list_map[ _( "Tidal strength" ) ], 1, wxFormat( _T( "%d" ) ) % map.info.tidalStrength );
		//    m_opts_list->SetItem( 0, 1,  );
	}
	catch ( ... )
	{
		m_opts_list->SetItem( m_opt_list_map[ _( "Size" ) ], 1, _T( "?x?" ) );
		m_opts_list->SetItem( m_opt_list_map[ _( "Windspeed" ) ], 1, _T( "?-?" ) );
		m_opts_list->SetItem( m_opt_list_map[ _( "Tidal strength" ) ], 1, _T( "?" ) );
	}
}

void BattleRoomTab::UpdateStatsLabels()
{
	m_ok_count_lbl->SetLabel( wxFormat( _( "Unready: %d" ) ) % ( m_battle->GetNumActivePlayers() - m_battle->GetNumOkPlayers() ) );
	PrintAllySetup();
}

void BattleRoomTab::UpdateMyInfo() {
    if ( !m_battle ) return;
    m_players->UpdateUser(m_battle->GetMe());
    m_players->RefreshVisibleItems();
}

void BattleRoomTab::UpdateUser( User& user )
{
	if ( !m_battle ) return;
	m_players->UpdateUser( user );

	m_minimap->UpdateMinimap();

	UpdateStatsLabels();

	if ( &user != &m_battle->GetMe() ) return;

	UserBattleStatus& bs = user.BattleStatus();
	m_team_sel->SetSelection( bs.team );
	m_ally_sel->SetSelection( bs.ally );
	m_side_sel->SetSelection( bs.side );
	m_spec_chk->SetValue( bs.spectator );
	m_auto_unspec_chk->SetValue( m_battle->GetAutoUnspec() );
	m_ready_chk->SetValue( bs.ready );
	// Enable or disable widgets' sensitivity as appropriate.
	if ( bs.spectator )
	{
		m_side_sel->Disable();
		m_ally_sel->Disable();
		m_team_sel->Disable();
		if ( m_battle->GetBattleType() != BT_Replay )
		{
		    m_auto_unspec_chk->Enable();
		    m_ready_chk->Disable();
		}
		else
		{
		    m_ready_chk->Enable();
		    m_auto_unspec_chk->Disable();
		}
	}
	else
	{
		m_side_sel->Enable();
		m_ally_sel->Enable();
		m_team_sel->Enable();
		m_ready_chk->Enable();
		m_auto_unspec_chk->Disable();
	}

	icons().SetColourIcon( user.BattleStatus().colour );
	m_color_sel->SetColor( user.BattleStatus().colour );
}


Battle* BattleRoomTab::GetBattle()
{
	return m_battle;
}


ChatPanel& BattleRoomTab::GetChatPanel()
{
	wxLogDebugFunc( _T( "" ) );
	ASSERT_LOGIC( m_chat != 0, _T( "m_chat = 0" ) );
	return *m_chat;
}


void BattleRoomTab::OnStart( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	if ( m_battle->IsFounderMe() )
	{
		m_battle->GetMe().BattleStatus().ready = true;
		if ( !m_battle->IsEveryoneReady() )
		{
			int answer = customMessageBox( SL_MAIN_ICON, _( "Some Players are not ready yet\nDo you want to force start?" ), _( "Not ready" ), wxYES_NO );
			if ( answer == wxNO ) return;
		}

		m_battle->SaveMapDefaults(); // save map presets

		m_battle->StartHostedBattle();
	}
	else
	{
		if ( m_battle->GetFounder().Status().in_game )
		{
			if ( !ui().IsSpringRunning() ) m_battle->StartSpring();
			else customMessageBoxNoModal( SL_MAIN_ICON, _("Spring is already running."), _("Error") );
		}
		else customMessageBoxNoModal( SL_MAIN_ICON, _("Host is not ingame."), _("Error") );
	}
}


void BattleRoomTab::OnLeave( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->Leave();
}



void BattleRoomTab::OnBalance( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	AutoBalanceDialog::BalanceOptions defaultval;
	defaultval.type = ( IBattle::BalanceType )sett().GetBalanceMethod();
	defaultval.respectclans = sett().GetBalanceClans();
	defaultval.strongclans = sett().GetBalanceStrongClans();
	defaultval.groupingsize = sett().GetBalanceGrouping();
	AutoBalanceDialog dlg( this, defaultval, m_battle->GetMaxPlayers() );
	if ( dlg.ShowModal() == wxID_OK )
	{
		AutoBalanceDialog::BalanceOptions balance = dlg.GetResult();
		sett().SetBalanceMethod( balance.type );
		sett().SetBalanceClans( balance.respectclans );
		sett().SetBalanceStrongClans( balance.strongclans );
		sett().SetBalanceGrouping( balance.groupingsize );
		m_battle->Autobalance( balance.type, balance.respectclans, balance.strongclans, balance.groupingsize );
	}
}


void BattleRoomTab::OnFixTeams( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	AutoBalanceDialog::BalanceOptions defaultval;
	defaultval.type = ( IBattle::BalanceType )sett().GetFixIDMethod();
	defaultval.respectclans = sett().GetFixIDClans();
	defaultval.strongclans = sett().GetFixIDStrongClans();
	defaultval.groupingsize = sett().GetFixIDGrouping();
	AutoBalanceDialog dlg( this, defaultval, m_battle->GetMaxPlayers() );
	if ( dlg.ShowModal() == wxID_OK )
	{
		AutoBalanceDialog::BalanceOptions balance = dlg.GetResult();
		sett().SetFixIDMethod( balance.type );
		sett().SetFixIDClans( balance.respectclans );
		sett().SetFixIDStrongClans( balance.strongclans );
		sett().SetFixIDGrouping( balance.groupingsize );
		m_battle->FixTeamIDs( balance.type, balance.respectclans, balance.strongclans, balance.groupingsize );
	}
}


void BattleRoomTab::OnFixColours( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxLogMessage( _T( "" ) );
	if ( !m_battle->IsFounderMe() ) // Works with autohosts, and human hosts knows what it mean.
	{
		m_battle->Say( _T( "!fixcolors" ) );
		return;
	}
	//m_battle->Say(_T("fixing colours"));
	m_battle->FixColours();
}


void BattleRoomTab::OnAddBot( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	//customMessageBox(SL_MAIN_ICON,_T("Max players reached"),_T("Cannot add bot, maximum number of players already reached.") );
	AddBotDialog dlg( this, *m_battle );
	if ( dlg.ShowModal() == wxID_OK )
	{
		UserBattleStatus bs;
		bs.team = m_battle->GetFreeTeam( false );
		bs.ally = m_battle->GetFreeAlly( false );
		bs.sync = SYNC_SYNCED;
		bs.spectator = false;
		bs.side = 0;
		bs.ready = true;
		bs.handicap = 0;
		bs.colour = m_battle->GetNewColour();
		bs.aishortname = dlg.GetAIShortName();
		bs.aiversion = dlg.GetAIVersion();
		bs.aitype = dlg.GetAIType();
		bs.owner = m_battle->GetMe().GetNick();
		serverSelector().GetServer().AddBot( m_battle->GetBattleId(), dlg.GetNick(), bs );
	}
}


void BattleRoomTab::OnImReady( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->SetImReady( m_ready_chk->GetValue() );
}


void BattleRoomTab::OnLock( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->SetIsLocked( m_lock_chk->GetValue() );
	m_battle->SendHostInfo( IBattle::HI_Locked );
}


void BattleRoomTab::OnAutoHost( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->GetAutoHost().SetEnabled( m_autohost_mnu->IsChecked() );
}


void BattleRoomTab::OnAutoPaste( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxString description = wxGetTextFromUser( _( "Enter a battle description" ), _( "Set description" ), m_battle->GetDescription(), ( wxWindow* ) & ui().mw() );
	m_autopaste_mnu->Check( !description.IsEmpty() );
	if ( !description.IsEmpty() ) m_battle->SetDescription( description );
	sett().SetBattleLastAutoAnnounceDescription( m_autopaste_mnu->IsChecked() );
}

void BattleRoomTab::OnAutoControl( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	sett().SetBattleLastAutoControlState( m_autocontrol_mnu->IsChecked() );
}

void BattleRoomTab::OnAutoStart( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	sett().SetBattleLastAutoStartState( m_autostart_mnu->IsChecked() );
}

void BattleRoomTab::OnAutoSpec( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	int trigger = wxGetNumberFromUser( _( "Enter timeout before autospeccing a player in minutes" ), _( "Set Timeout" ), _T( "" ), sett().GetBattleLastAutoSpectTime() / 60, 1, 60, ( wxWindow* ) & ui().mw(), wxDefaultPosition );
	if ( trigger < 0 ) trigger = 0;
	trigger = trigger * 60;
	m_autospec_mnu->Check( trigger > 0 );
	sett().SetBattleLastAutoSpectTime( trigger );
}

void BattleRoomTab::OnAutounSpec( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->SetAutoUnspec( m_auto_unspec_chk->GetValue() );
}

void BattleRoomTab::OnImSpec( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->ForceSpectator( m_battle->GetMe(), m_spec_chk->GetValue() );
}


void BattleRoomTab::OnTeamSel( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	unsigned long team;
	m_team_sel->GetValue().ToULong( &team );
	m_battle->ForceTeam( m_battle->GetMe(), team - 1  );
}


void BattleRoomTab::OnAllySel( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	unsigned long ally;
	m_ally_sel->GetValue().ToULong( &ally );
	m_battle->ForceAlly( m_battle->GetMe(), ally - 1  );
}


void BattleRoomTab::OnColourSel( wxCommandEvent& /*unused*/ )
{
		if ( !m_battle ) return;
    User& u = m_battle->GetMe();
    wxColour CurrentColour = u.BattleStatus().colour;
    CurrentColour = GetColourFromUser(this, CurrentColour);
    if ( !CurrentColour.IsOk() ) return;
    sett().SetBattleLastColour( CurrentColour );
    m_battle->ForceColour( u, CurrentColour );
}


void BattleRoomTab::OnSideSel( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->ForceSide( m_battle->GetMe(), m_side_sel->GetSelection() );
	sett().SetBattleLastSideSel( m_battle->GetHostModName(), m_side_sel->GetSelection() );
}


void BattleRoomTab::OnPresetSel( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxString presetname = m_options_preset_sel->GetValue();
	if ( presetname.IsEmpty() ) return;
	m_battle->LoadOptionsPreset( presetname );
	m_battle->SendHostInfo( IBattle::HI_Send_All_opts );
}

void BattleRoomTab::OnAutoLock( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->SetAutoLockOnStart( m_autolock_chk->GetValue() );
	sett().SetLastAutolockStatus( m_autolock_chk->GetValue() );
}


void BattleRoomTab::OnLockBalance( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	bool locked = m_lock_balance_mnu->IsChecked();
	m_battle->SetLockExternalBalanceChanges( locked );
}

void BattleRoomTab::OnSpectUnsynced( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->ForceUnsyncedToSpectate();
}

void BattleRoomTab::OnSpectUnready( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->ForceUnReadyToSpectate();
}
void BattleRoomTab::OnSpectUnreadyUnsynced( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->ForceUnsyncedAndUnreadyToSpectate();
}

void BattleRoomTab::OnRingUnready( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->RingNotReadyPlayers();
}

void BattleRoomTab::OnRingUnsynced( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->RingNotSyncedPlayers();
}

void BattleRoomTab::OnRingUnreadyUnsynced( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	m_battle->RingNotSyncedAndNotReadyPlayers();
}


void BattleRoomTab::OnShowManagePlayersMenu( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	PopupMenu( m_manage_users_mnu );
}

void BattleRoomTab::OnUserJoined( User& user )
{
	if ( !m_battle ) return;
	if ( !user.BattleStatus().IsBot() ) m_chat->Joined( user );
	m_players->AddUser( user );

	UpdateUser(user);

	if ( &user == &m_battle->GetMe() )
	{
		m_players->SetSelectedIndex ( m_players->GetIndexFromData( &user ) );
	}
	UpdateStatsLabels();

	UiEvents::GetStatusEventSender( UiEvents::addStatusMessage ).SendEvent(
			UiEvents::StatusData( wxFormat(_("%s joined your active battle") ) % user.GetNick(), 1 ) );
}


void BattleRoomTab::OnUserLeft( User& user )
{
	if ( !m_battle ) return;
	if ( !user.BattleStatus().IsBot() ) m_chat->Parted( user, wxEmptyString );
	m_players->RemoveUser( user );

	UpdateStatsLabels();
}


void BattleRoomTab::OnUnitsyncReloaded( wxCommandEvent& /*data*/ )
{
	if ( !m_battle ) return;
	//m_minimap->UpdateMinimap();//should happen automagically now
	ReloadMaplist();
	UpdateBattleInfo();
	RegenerateOptionsList();
	m_battle->SendMyBattleStatus(); // This should reset sync status.
}

long BattleRoomTab::AddMMOptionsToList( long pos, LSL::OptionsWrapper::GameOption optFlag )
{
    if ( !m_battle ) return -1;
    for (auto it : m_battle->CustomBattleOptions().getOptions(optFlag))
	{
        const wxString tag = wxFormat( _T( "%d_%s" ) ) % optFlag % it.first;
		//m_opt_list_map[ tag ] = pos;
		UpdateBattleInfo( tag );
		pos++;
	}
	return pos;
}

void BattleRoomTab::UpdateHighlights()
{
	if ( !m_battle ) return;
	m_players->RefreshVisibleItems();
}


void BattleRoomTab::UpdatePresetList()
{
	if ( !m_battle ) return;
	m_options_preset_sel->Clear();
	m_options_preset_sel->Append( sett().GetPresetList() );
	m_options_preset_sel->SetStringSelection(  m_battle->GetCurrentPreset() );
}


void BattleRoomTab::OnSavePreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxString presetname;
	if ( !ui().AskText( _( "Enter preset name" ), _( "Enter a name to save the current set of options\nIf a preset with the same name already exist, it will be overwritten" ), presetname ) )
		return;
	if ( presetname.IsEmpty() )
	{
		customMessageBoxNoModal( SL_MAIN_ICON , _( "Cannot save an options set without a name." ), _( "error" ), wxICON_EXCLAMATION | wxOK );
		return;
	}
	m_battle->SaveOptionsPreset( presetname );
}


void BattleRoomTab::OnDeletePreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxArrayString choices = m_battle->GetPresetList();
	int result = wxGetSingleChoiceIndex( _( "Pick an existing option set from the list" ), _( "Delete preset" ), choices );
	if ( result < 0 ) return;
	m_battle->DeletePreset( choices[result] );
}

void BattleRoomTab::OnSetModDefaultPreset( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxArrayString choices = m_battle->GetPresetList();
	int result = wxGetSingleChoiceIndex( _( "Pick an existing option set from the list" ), _( "Set game default preset" ), choices );
	if ( result < 0 ) return;
	sett().SetModDefaultPresetName( m_battle->GetHostModName(), choices[result] );
}


void BattleRoomTab::OnMapBrowse( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	wxLogDebugFunc( _T( "" ) );

	const wxString mapname = mapSelectDialog();
	if ( !mapname.empty() )
	{
		if ( !m_battle->IsFounderMe() )
		{
            autohostManager.GetAutohostHandler().SetMap(mapname);
			//m_battle->DoAction( _T( "suggests " ) + mapname );
			return;
		}

		const int idx = m_map_combo->FindString( mapname, true /*case sensitive*/ );
		if ( idx != wxNOT_FOUND ) {
			SetMap( idx );
		}
	}
}

void BattleRoomTab::ReloadMaplist()
{
	if ( !m_battle ) return;
	m_map_combo->Clear();

    const wxArrayString maplist = LSL::Util::vectorToArrayString(LSL::usync().GetMapList());
// maplist.Sort(CompareStringIgnoreCase);

	size_t nummaps = maplist.Count();
	for ( size_t i = 0; i < nummaps; i++ ) m_map_combo->Insert( maplist[i], i );
	m_map_combo->SetValue( m_battle->GetHostMapName() );
}

void BattleRoomTab::SetMap( int index )
{
	if ( !m_battle ) return;
	try
	{
		LSL::UnitsyncMap map = LSL::usync().GetMapEx( index );
		m_battle->SetLocalMap( map );
		m_battle->SendHostInfo( IBattle::HI_Map );
	} catch ( ... ) {}
}

void BattleRoomTab::OnMapSelect( wxCommandEvent& /*unused*/ )
{
	if ( !m_battle ) return;
	if ( !m_battle->IsFounderMe() )
	{
		try
		{
            m_battle->DoAction( _T( "suggests " ) + TowxString(LSL::usync().GetMap( m_map_combo->GetCurrentSelection() ).name));
		}
		catch ( ... )
		{
		}
		return;
	}
	SetMap( m_map_combo->GetCurrentSelection() );
}

void BattleRoomTab::OnOptionActivate( wxListEvent& event )
{
	if ( !m_battle ) return;
	if ( !m_battle->IsFounderMe() ) return;
	long index = event.GetIndex();
	if ( index == 0 ) return;
	wxString tag;
	for ( OptionListMap::const_iterator itor = m_opt_list_map.begin(); itor != m_opt_list_map.end(); ++itor )
	{
		if ( itor->second == index )
		{
			tag = itor->first;
			break;
		}
	}
	LSL::OptionsWrapper& optWrap = m_battle->CustomBattleOptions();
	LSL::OptionsWrapper::GameOption optFlag = ( LSL::OptionsWrapper::GameOption )s2l( tag.BeforeFirst( '_' ) );
	const std::string key = STD_STRING(tag.AfterFirst( '_' ));
	LSL::Enum::OptionType type = optWrap.GetSingleOptionType( key );
	if ( !optWrap.keyExists( key, optFlag, false, type ) ) return;
	SingleOptionDialog dlg( *m_battle, tag );
	dlg.ShowModal();
}

void BattleRoomTab::SortPlayerList()
{
	if ( !m_battle ) return;
	m_players->SortList();
}

void BattleRoomTab::SetBattle( Battle* battle )
{
	m_battle = battle;

	m_team_sel->Enable(m_battle);
	m_ally_sel->Enable(m_battle);
	m_color_sel->Enable(m_battle);
	m_side_sel->Enable(m_battle);
	m_options_preset_sel->Enable(m_battle);

	m_minimap->Enable(m_battle);

	m_player_panel->Enable(m_battle);

	m_map_combo->Enable(m_battle);

	m_players->Enable(m_battle);

	m_leave_btn->Enable(m_battle);
	m_start_btn->Enable(m_battle);
	m_addbot_btn->Enable(m_battle);
	m_manage_players_btn->Enable(m_battle);
	m_save_btn->Enable(m_battle);
	m_delete_btn->Enable(m_battle);
	m_default_btn->Enable(m_battle);
	m_browse_map_btn->Enable(m_battle);

	m_ready_chk->Enable(m_battle);
	m_spec_chk->Enable(m_battle);
	m_lock_chk->Enable(m_battle);
	m_autolock_chk->Enable(m_battle);

	m_opts_list->Enable(m_battle);

	m_minimap->SetBattle( m_battle );
	m_players->SetBattle( m_battle );
	m_chat->SetBattle( m_battle );
	m_players->Clear();
	m_side_sel->Clear();

	if ( m_battle )
	{
		m_options_preset_sel->SetStringSelection( sett().GetModDefaultPresetName( m_battle->GetHostModName() ) );

		m_color_sel->SetColor( m_battle->GetMe().BattleStatus().colour );
		try
		{
            const wxArrayString sides = LSL::Util::vectorToArrayString(
                        LSL::usync().GetSides(STD_STRING(m_battle->GetHostModName())));
			for ( unsigned int i = 0; i < sides.GetCount(); i++ )
			{
				m_side_sel->Append( sides[i], icons().GetBitmap( icons().GetSideIcon( m_battle->GetHostModName(), i ) ) );
			}
		}
		catch ( ... ) {}
		for ( UserList::user_map_t::size_type i = 0; i < m_battle->GetNumUsers(); i++ )
		{
			m_players->AddUser( m_battle->GetUser( i ) );
			#ifdef __WXMAC__
			UpdateUser( m_battle->GetUser( i ) );
			#endif
		}

		if ( !m_battle->IsFounderMe() )
		{
			m_options_preset_sel->Disable();
			m_save_btn->Disable();
			m_delete_btn->Disable();
			m_default_btn->Disable();
			m_manage_players_btn->Disable();
			m_lock_chk->Disable();
			m_autolock_chk->Disable();
		}

		m_host_new_btn->Show( false );

		RegenerateOptionsList();

		ReloadMaplist();

		UpdateBattleInfo( wxFormat( _T( "%d_mapname" ) ) % LSL::OptionsWrapper::PrivateOptions );
		UpdateBattleInfo();
		UpdateStatsLabels();
	}
	else
	{
		m_host_new_btn->Show( true );
	}
}

void BattleRoomTab::RegenerateOptionsList()
{
	long pos = 0;
	m_opts_list->DeleteAllItems();
	m_opts_list->InsertItem( pos, _( "Size" ) );
	m_opt_list_map[ _( "Size" ) ] = pos;
	pos++;
	m_opts_list->InsertItem( pos , _( "Windspeed" ) );
	m_opt_list_map[ _( "Windspeed" ) ] = pos;
	pos++;
	m_opts_list->InsertItem( pos, _( "Tidal strength" ) );
	m_opt_list_map[ _( "Tidal strength" ) ] = pos;
	pos++;
	m_opts_list->InsertItem( pos, wxEmptyString );
	pos++;
	pos = AddMMOptionsToList( pos, LSL::OptionsWrapper::EngineOption );
	// AddMMOptionsToList already increments pos by itself
	m_opts_list->InsertItem( pos, wxEmptyString );
	pos++;
	m_mod_opts_index = pos;
	pos = AddMMOptionsToList( m_mod_opts_index, LSL::OptionsWrapper::ModOption );
	// AddMMOptionsToList already increments pos by itself
	m_opts_list->InsertItem( pos, wxEmptyString );
	pos++;
	m_map_opts_index = pos;
	pos = AddMMOptionsToList( m_map_opts_index, LSL::OptionsWrapper::MapOption );
}

void BattleRoomTab::OnBattleActionEvent( UiEvents::UiEventData data )
{
	wxString nick = data.Count() > 0 ? data[0] : wxString(wxEmptyString);
	wxString msg = data.Count() > 1 ? data[1] : wxString(wxEmptyString);
	GetChatPanel().DidAction( nick, msg );
}

void BattleRoomTab::OnHostNew( wxCommandEvent& /*event*/ )
{
	if ( !ui().IsConnected() )
	{
		wxLogWarning( _T( "Trying to host while offline" ) );
		customMessageBoxNoModal( SL_MAIN_ICON, _( "You cannot host a game while being offline. Please connect to a lobby server." ), _( "Not Online." ), wxOK );
		ui().ShowConnectWindow();
		return;
	}
/*	if ( !ui().IsSpringCompatible() )
	{
		wxLogWarning( _T( "Hosting is disabled due to the incompatible version " ) );
		customMessageBoxNoModal( SL_MAIN_ICON, _( "Hosting is disabled due to the incompatible version you're using" ), _( "Spring error" ), wxICON_EXCLAMATION | wxOK );
		return;
	}
*/
	SL::RunHostBattleDialog( this );
}

void BattleRoomTab::OnAutohostBalance( wxCommandEvent& /*event*/ )
{
	autohostManager.GetAutohostHandler().Balance();
}

void BattleRoomTab::OnAutohostRandomMap( wxCommandEvent& /*event*/ )
{
	autohostManager.GetAutohostHandler().SetRandomMap();
}


void BattleRoomTab::OnAutohostNotify( wxCommandEvent& /*event*/ )
{
	autohostManager.GetAutohostHandler().Notify();
}

//void BattleRoomTab::MaximizeSizer()
//{
//	wxSize s = GetClientSize();
//	m_main_sizer->RecalcSizes();
//	//m_main_sizer->SetDimension( )
//	m_main_sizer->Layout();
//	Layout();
//	Refresh();
//}

