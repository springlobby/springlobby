#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H

#include <wx/scrolwin.h>

#include <lslunitsync/optionswrapper.h>
#include "utils/globalevents.h"
#include "utils/uievents.h"
#include "autohostmanager.h"
#include <map>

class Ui;
class Battle;
struct BattleBot;
class BattleroomListCtrl;
class User;
class ChatPanel;
class wxCommandEvent;
class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxSplitterWindow;
class wxStaticLine;
class wxButton;
class wxCheckBox;
class wxListCtrl;
class MapCtrl;
class ColorButton;
class wxBitmapComboBox;
struct UnitSyncMap;
class wxToggleButton;
class wxChoice;
class wxListEvent;

typedef std::map<wxString, long> OptionListMap;

/** \brief container for BattleroomListCtrl, battle specific ChatPanel. Also displaying battle info summary
 * \todo DOCMEMORE */
class BattleRoomTab : public wxScrolledWindow, public GlobalEvent
{
	public:
		BattleRoomTab( wxWindow* parent, Battle* battle );
		~BattleRoomTab();

		BattleroomListCtrl& GetPlayersListCtrl();

		void UpdateUser( User& user );

		Battle* GetBattle();
		ChatPanel& GetChatPanel();

		void UpdateBattleInfo();
		void UpdateBattleInfo( const wxString& Tag );

		void OnStart( wxCommandEvent& event );
		void OnHostNew( wxCommandEvent& event );
		void OnLeave( wxCommandEvent& event );
		void OnBalance( wxCommandEvent& event );
		void OnFixTeams( wxCommandEvent& event );
		void OnFixColours( wxCommandEvent& event );
		void OnAddBot( wxCommandEvent& event );
		void OnImReady( wxCommandEvent& event );
		void OnLock( wxCommandEvent& event );
		void OnAutoHost( wxCommandEvent& event );
		void OnImSpec( wxCommandEvent& event );
		void OnAutounSpec( wxCommandEvent& event );
		void OnTeamSel( wxCommandEvent& event );
		void OnAllySel( wxCommandEvent& event );
		void OnColourSel( wxCommandEvent& event );
		void OnSideSel( wxCommandEvent& event );
		void OnPresetSel( wxCommandEvent& event );
		void OnAutoLock( wxCommandEvent& event );
		void OnLockBalance( wxCommandEvent& event );
		void OnShowManagePlayersMenu( wxCommandEvent& event );
		void OnLoadPreset( wxCommandEvent& event );
		void OnSavePreset( wxCommandEvent& event );
		void OnDeletePreset( wxCommandEvent& event );
		void OnSetModDefaultPreset( wxCommandEvent& event );
		void OnMapBrowse( wxCommandEvent& event );
		void OnMapSelect( wxCommandEvent& event );
		void OnOptionActivate( wxListEvent& event );

		void OnSpectUnsynced( wxCommandEvent& event );
		void OnSpectUnready( wxCommandEvent& event );
		void OnSpectUnreadyUnsynced( wxCommandEvent& event );

		void OnRingUnready( wxCommandEvent& event );
		void OnRingUnsynced( wxCommandEvent& event );
		void OnRingUnreadyUnsynced( wxCommandEvent& event );

		void OnAutoPaste( wxCommandEvent& event );
		void OnAutoControl( wxCommandEvent& event );
		void OnAutoStart( wxCommandEvent& event );
		void OnAutoSpec( wxCommandEvent& event );

        void OnAutohostBalance( wxCommandEvent& event );
        void OnAutohostRandomMap( wxCommandEvent& event );
        void OnAutohostNotify( wxCommandEvent& event );


		void OnBattleActionEvent( UiEvents::UiEventData data );

		void OnUserJoined( User& user );
		void OnUserLeft( User& user );

		void ReloadMaplist();
		void SetMap( int index );

		void UpdateHighlights();

		void UpdatePresetList();

		void SortPlayerList();

		void OnUnitsyncReloaded( wxCommandEvent& /*data*/ );

		void SetBattle( Battle* battle );

		void PrintAllySetup();

		void RegenerateOptionsList();

		void UpdateStatsLabels();
		void UpdateMapInfoSummary();
		void UpdateMyInfo();

	protected:
        AutohostManager autohostManager;

		long AddMMOptionsToList( long pos, LSL::OptionsWrapper::GameOption optFlag );

		void SplitSizerHorizontally( const bool horizontal );

		Battle* m_battle;
//		UnitSyncMap m_map; //not needed

		long m_mod_opts_index;
		long m_map_opts_index;

		OptionListMap m_opt_list_map;

		wxBoxSizer* m_players_sizer;
		wxBoxSizer* m_player_sett_sizer;
		wxBoxSizer* m_info_sizer;
		wxBoxSizer* m_top_sizer;
		wxBoxSizer* m_buttons_sizer;
		wxBoxSizer* m_info1_sizer;
		wxBoxSizer* m_main_sizer;

		wxComboBox* m_team_sel;
		wxComboBox* m_ally_sel;
		ColorButton* m_color_sel;
		wxBitmapComboBox* m_side_sel;
		wxComboBox* m_options_preset_sel;

		wxStaticText* m_team_lbl;
		wxStaticText* m_ally_lbl;
		wxStaticText* m_side_lbl;
		wxStaticText* m_color_lbl;
//		wxStaticText* m_wind_lbl;
//		wxStaticText* m_tidal_lbl;
//		wxStaticText* m_size_lbl;
		wxStaticText* m_ally_setup_lbl;
		wxStaticText* m_ok_count_lbl;

		MapCtrl * m_minimap;

		wxScrolledWindow* m_player_panel;

		wxComboBox* m_map_combo;

		BattleroomListCtrl* m_players;
		ChatPanel* m_chat;
		wxSplitterWindow* m_splitter;

		wxStaticLine* m_command_line;

		wxButton* m_leave_btn;
		wxButton* m_start_btn;
		wxButton* m_addbot_btn;
		wxButton* m_manage_players_btn;
		wxButton* m_save_btn;
		wxButton* m_delete_btn;
		wxButton* m_default_btn;
		wxButton* m_browse_map_btn;
		wxButton* m_host_new_btn;

		wxMenu* m_manage_users_mnu;
		wxMenuItem* m_lock_balance_mnu;
		wxMenuItem* m_autohost_mnu;
		wxMenuItem* m_autostart_mnu;
		wxMenuItem* m_autospec_mnu;
		wxMenuItem* m_autocontrol_mnu;
		wxMenuItem* m_autopaste_mnu;

		wxCheckBox* m_ready_chk;
		wxCheckBox* m_spec_chk;
		wxCheckBox* m_auto_unspec_chk;
		wxCheckBox* m_lock_chk;
		wxCheckBox* m_autolock_chk;

		wxListCtrl* m_opts_list;

		EventReceiverFunc<BattleRoomTab, UiEvents::UiEventData, &BattleRoomTab::OnBattleActionEvent> m_BattleActionSink;

		enum {
			BROOM_LEAVE = wxID_HIGHEST,
			BROOM_IMREADY,
			BROOM_LOCK,
			BROOM_LOCK_BALANCE,
			BROOM_MANAGE_MENU,
			BROOM_SPEC,
			BROOM_UNSPEC,
			BROOM_TEAMSEL,
			BROOM_ALLYSEL,
			BROOM_COLOURSEL,
			BROOM_SIDESEL,
			BROOM_START,
			BROOM_ADDBOT,
			BROOM_BALANCE,
			BROOM_FIXID,
			BROOM_FIXCOLOURS,
			BROOM_PRESETSEL,
			BROOM_AUTOHOST,
			BROOM_AUTOLOCK,
			BROOM_SAVEPRES,
			BROOM_DELETEPRES,
			BROOM_SETDEFAULTPRES,
			BROOM_MAP_BROWSE,
			BROOM_MAP_SEL,
			BROOM_OPTIONLIST,
			BROOM_RING_UNREADY,
			BROOM_RING_UNSYNC,
			BROOM_RING_UNREADY_UNSYNC,
			BROOM_SPECT_UNREADY,
			BROOM_SPECT_UNSYNC,
			BROOM_SPECT_UNREADY_UNSYNC,
			BROOM_AUTOSPECT,
			BROOM_AUTOSTART,
			BROOM_AUTOCONTROL,
			BROOM_AUTOPASTE,
			BROOM_HOST_NEW,

			BROOM_AUTOHOST_BALANCE,
			BROOM_AUTOHOST_RANDOMMAP,
			BROOM_AUTOHOST_NOTIFY,
		};

		DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

