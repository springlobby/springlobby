//
// Class: BattleRoomTab
//

#ifndef _BATTLEROOMTAB_H_
#define _BATTLEROOMTAB_H_

#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/checkbox.h>

#include "battle.h"
#include "battleroomlistctrl.h"
#include "chatpanel.h"

class Ui;

const wxString team_choices[] = { _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), _T("10"), _T("11"), _T("12"), _T("13"), _T("14"), _T("15"), _T("16") };
const wxString colour_choices[] = { 
  _T("black"), _T("dark gray"), _T("dark blue"), _T("bright blue"), _T("dark green"),
  _T("bright green"), _T("dark cyan"), _T("bright cyan"), _T("dark red"), _T("bright red"),
  _T("dark magenta"), _T("bright magenta"), _T("dark yellow"), _T("bright yellow"),
  _T("light gray"), _T("inky blue") 
};

const int colour_values[][3] = { {0,0,0}, {128, 128, 128}, {0, 0, 128}, {0, 0, 255},
  {0, 128, 0}, {0, 255, 0}, {0, 128, 128}, {0, 255, 255}, {128, 0, 0}, {255, 0, 0},
  {128, 0, 128}, {255, 0, 255}, {128, 128, 0}, {255, 255, 0}, {192, 192, 192}, {0, 220, 250}
};


class BattleRoomTab : public wxPanel
{
  public:
    BattleRoomTab( wxWindow* parent, Ui& ui, Battle& battle );
     ~BattleRoomTab();
  
    // BattleRoomTab interface
   BattleroomListCtrl& GetPlayersListCtrl() { assert( m_players != NULL); return *m_players; }
   
   void UpdateUser( User& user );

   Battle& GetBattle() { return m_battle; }
   ChatPanel& GetChatPanel() { return *m_chat; }
   
   void OnLeave( wxCommandEvent& event );
   void OnImReady( wxCommandEvent& event );
   void OnImSpec( wxCommandEvent& event );
   void OnTeamSel( wxCommandEvent& event );
   void OnAllySel( wxCommandEvent& event );
   void OnColourSel( wxCommandEvent& event );
   void OnSideSel( wxCommandEvent& event );
   
   void OnUserJoined( User& user );
   void OnUserLeft( User& user );
   
  protected:
    // BattleRoomTab variables
  
    Ui& m_ui;
    Battle& m_battle;
  
    wxBoxSizer* m_players_sizer;
    wxBoxSizer* m_player_sett_sizer;
    wxBoxSizer* m_info_sizer;
    wxBoxSizer* m_top_sizer;
    wxBoxSizer* m_buttons_sizer;
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
  
    wxPanel* m_player_panel;
  
    BattleroomListCtrl* m_players;
    ChatPanel* m_chat;
    wxSplitterWindow* m_splitter;
  
    wxStaticLine* m_command_line;
  
    wxButton* m_leave_btn;
    wxButton* m_start_btn;
  
    wxCheckBox* m_ready_chk;
    wxCheckBox* m_spec_chk;
  
    DECLARE_EVENT_TABLE()
};

enum
{
    BROOM_LEAVE = wxID_HIGHEST,
    BROOM_IMREADY,
    BROOM_SPEC,
    BROOM_TEAMSEL,
    BROOM_ALLYSEL,
    BROOM_COLOURSEL,
    BROOM_SIDESEL
};

#endif  //_BATTLEROOMTAB_H_

