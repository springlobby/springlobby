#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H

#include <wx/panel.h>

#include "iunitsync.h"

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


class BattleRoomTab : public wxPanel
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

   void OnStart( wxCommandEvent& event );
   void OnLeave( wxCommandEvent& event );
   void OnAddBot( wxCommandEvent& event );
   void OnImReady( wxCommandEvent& event );
   void OnLock( wxCommandEvent& event );
   void OnImSpec( wxCommandEvent& event );
   void OnTeamSel( wxCommandEvent& event );
   void OnAllySel( wxCommandEvent& event );
   void OnColourSel( wxCommandEvent& event );
   void OnSideSel( wxCommandEvent& event );

   void OnUserJoined( User& user );
   void OnUserLeft( User& user );

   void OnBotAdded( BattleBot& bot );
   void OnBotRemoved( BattleBot& bot );
   void OnBotUpdated( BattleBot& bot );

  protected:
    Ui& m_ui;
    Battle& m_battle;
    UnitSyncMap m_map;

    wxBoxSizer* m_players_sizer;
    wxBoxSizer* m_player_sett_sizer;
    wxBoxSizer* m_info_sizer;
    wxBoxSizer* m_top_sizer;
    wxBoxSizer* m_buttons_sizer;
    wxBoxSizer* m_info1_sizer;
    wxBoxSizer* m_main_sizer;

    wxComboBox* m_team_sel;
    wxComboBox* m_ally_sel;
    wxComboBox* m_color_sel;
    wxComboBox* m_side_sel;

    wxStaticText* m_team_lbl;
    wxStaticText* m_ally_lbl;
    wxStaticText* m_color_lbl;
    wxStaticText* m_side_lbl;
    wxStaticText* m_map_lbl;
    wxStaticText* m_wind_lbl;
    wxStaticText* m_tidal_lbl;
    wxStaticText* m_size_lbl;

    MapCtrl * m_minimap;

    wxPanel* m_player_panel;

    BattleroomListCtrl* m_players;
    ChatPanel* m_chat;
    wxSplitterWindow* m_splitter;

    wxStaticLine* m_command_line;

    wxButton* m_leave_btn;
    wxButton* m_start_btn;
    wxButton* m_addbot_btn;

    wxCheckBox* m_ready_chk;
    wxCheckBox* m_spec_chk;
    wxCheckBox* m_lock_chk;

    wxListCtrl* m_opts_list;
    DECLARE_EVENT_TABLE();
};

enum
{
    BROOM_LEAVE = wxID_HIGHEST,
    BROOM_IMREADY,
    BROOM_LOCK,
    BROOM_SPEC,
    BROOM_TEAMSEL,
    BROOM_ALLYSEL,
    BROOM_COLOURSEL,
    BROOM_SIDESEL,
    BROOM_START,
    BROOM_ADDBOT
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEROOMTAB_H
