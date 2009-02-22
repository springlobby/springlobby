#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H

#include <wx/scrolwin.h>

#include "mmoptionswrapper.h"
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

typedef std::map<wxString,long> OptionListMap;

/** \brief container for BattleroomListCtrl, battle specific ChatPanel. Also displaying battle info summary
 * \todo DOCMEMORE */
class BattleRoomTab : public wxScrolledWindow
{
  public:
    BattleRoomTab( wxWindow* parent, Ui& ui, Battle& battle );
     ~BattleRoomTab();

    BattleroomListCtrl& GetPlayersListCtrl();

    void UpdateUser( User& user );

    Battle& GetBattle();
    ChatPanel& GetChatPanel();

    bool IsHosted();

    void UpdateBattleInfo();
    void UpdateBattleInfo( const wxString& Tag );

    void OnStart( wxCommandEvent& event );
    void OnLeave( wxCommandEvent& event );
    void OnBalance( wxCommandEvent& event );
    void OnFixTeams( wxCommandEvent& event );
    void OnFixColours( wxCommandEvent& event );
    void OnAddBot( wxCommandEvent& event );
    void OnImReady( wxCommandEvent& event );
    void OnLock( wxCommandEvent& event );
    void OnAutoHost( wxCommandEvent& event );
    void OnImSpec( wxCommandEvent& event );
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

    void OnUserJoined( User& user );
    void OnUserLeft( User& user );

    void OnUnitSyncReloaded();
		void ReloadMaplist();
		void SetMap( int index );

    void UpdateHighlights();

    void UpdatePresetList();

    void SortPlayerList();

  protected:

    long AddMMOptionsToList( long pos, OptionsWrapper::GameOption optFlag );

    Ui& m_ui;
    Battle& m_battle;
    UnitSyncMap m_map;

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
    wxStaticText* m_wind_lbl;
    wxStaticText* m_tidal_lbl;
    wxStaticText* m_size_lbl;

    MapCtrl * m_minimap;

    wxPanel* m_player_panel;

		wxChoice* m_map_combo;

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

    wxMenu* m_manage_users_mnu;
    wxMenuItem* m_lock_balance_mnu;

    wxCheckBox* m_ready_chk;
    wxCheckBox* m_spec_chk;
    #if wxUSE_TOGGLEBTN
    wxToggleButton* m_lock_chk;
    wxToggleButton* m_autohost_chk;
    wxToggleButton* m_autolock_chk;
    #else
    wxCheckBox* m_lock_chk;
    wxCheckBox* m_autohost_chk;
    wxCheckBox* m_autolock_chk;
    #endif

    wxListCtrl* m_opts_list;

    enum {
        BROOM_LEAVE = wxID_HIGHEST,
        BROOM_IMREADY,
        BROOM_LOCK,
        BROOM_LOCK_BALANCE,
        BROOM_MANAGE_MENU,
        BROOM_SPEC,
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
				BROOM_OPTIONLIST
    };

    DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H
