#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H

#include <wx/tglbtn.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

#include "battlelisttab.h"
#include "battle.h"

///////////////////////////////////////////////////////////////////////////

class BattleListTab;

///////////////////////////////////////////////////////////////////////////////
/// Class BattleListFilter
///////////////////////////////////////////////////////////////////////////////
class BattleListFilter : public wxDialog
{
	public:
    BattleListFilter( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style , BattleListTab* parentBattleListTab);

    void OnPlayerButton   ( wxCommandEvent& event );
    void OnMaxPlayerButton( wxCommandEvent& event );
    void OnSpectatorButton( wxCommandEvent& event );
    void OnActivate       ( wxCommandEvent& event );
    void OnClose          ( wxCloseEvent&   event );
    void SetActiv         ();

    void OnChange   ( wxCommandEvent& event );

    void OnStatusChange      ( wxCommandEvent& event );
    void OnRankChange        ( wxCommandEvent& event );
    void OnPlayerChange      ( wxCommandEvent& event );
    void OnMaxPlayerChange   ( wxCommandEvent& event );
    void OnSpectatorChange   ( wxCommandEvent& event );

    bool FilterBattle(Battle& battle);
    bool GetActiv() const;

	protected:
	  enum m_button_mode {m_equal,m_bigger,m_smaller};
	  enum m_status      {m_all = -1,m_started = 0,m_open = 1,m_openpw = 2,m_closed = 3,m_closedpw = 4,m_unknown = 5};

    char _GetButtonSign(m_button_mode value);
		m_button_mode _GetNextMode(m_button_mode value);
		bool _IntCompare(int a,int b,m_button_mode mode);

    bool m_activ;

		BattleListTab* m_parent_battlelisttab;

		wxToggleButton* m_filter_show;
		wxStaticText* m_filter_text;

		wxCheckBox* m_filter_activ;
		wxStaticText* m_filter_host_text;
		wxTextCtrl* m_filter_host_edit;

		wxStaticText* m_filter_status_text;
		wxChoice* m_filter_status_choice;
		m_status m_filter_status_choice_value;
		wxStaticText* m_filter_rank_text;
		wxChoice* m_filter_rank_choice;
		int m_filter_rank_choice_value;
		wxStaticText* m_filter_description_text;
		wxTextCtrl* m_filter_description_edit;

		wxStaticText* m_filter_player_text;
		wxButton* m_filter_player_button;
		m_button_mode m_filter_player_mode;
		wxChoice* m_filter_player_choice;
		int m_filter_player_choice_value;
		wxStaticText* m_filter_map_text;
		wxTextCtrl* m_filter_map_edit;
		wxCheckBox* m_filter_map_show;

		wxStaticText* m_filter_maxplayer_text;
		wxButton* m_filter_maxplayer_button;
		m_button_mode m_filter_maxplayer_mode;
		wxChoice* m_filter_maxplayer_choice;
		int m_filter_maxplayer_choice_value;
		wxStaticText* m_filter_mod_text;
		wxTextCtrl* m_filter_mod_edit;
		wxCheckBox* m_filter_mod_show;

		wxStaticText* m_filter_spectator_text;
		wxButton* m_filter_spectator_button;
		m_button_mode m_filter_spectator_mode;
		wxChoice* m_filter_spectator_choice;
		int m_filter_spectator_choice_value;


    DECLARE_EVENT_TABLE();
};

enum
{
    BATTLE_FILTER_HOST_EDIT,
    BATTLE_FILTER_DESCRIPTION_EDIT,
    BATTLE_FILTER_MAP_EDIT,
    BATTLE_FILTER_MOD_EDIT,
    BATTLE_FILTER_STATUS_CHOICE,
    BATTLE_FILTER_RANK_CHOICE,
    BATTLE_FILTER_PLAYER_CHOICE,
    BATTLE_FILTER_MAXPLAYER_CHOICE,
    BATTLE_FILTER_SPECTATOR_CHOICE,
    BATTLE_FILTER_MAP_SHOW,
    BATTLE_FILTER_MOD_SHOW,
    BATTLE_FILTER_PLAYER_BUTTON,
    BATTLE_FILTER_MAXPLAYER_BUTTON,
    BATTLE_FILTER_SPECTATOR_BUTTON,
    BATTLE_FILTER_ACTIV,
    BATTLE_FILTER_ACTIV_TB
};

#endif //SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H
