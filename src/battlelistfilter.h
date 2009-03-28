#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H

#include <wx/panel.h>

#include "battlelisttab.h"
#include "battle.h"

///////////////////////////////////////////////////////////////////////////

class BattleListTab;
class wxToggleButton;
class wxCheckBox;
class wxStaticText;
class wxTextCtrl;
class wxChoice;
class wxButton;
class wxRegEx;
class wxStaticText;


/** \brief The panel contained in BattleListTab used to filter for diff info of battles
 * \todo DOCMEMORE */
class BattleListFilter : public wxPanel
{
	public:
    BattleListFilter( wxWindow* parent, wxWindowID id, BattleListTab* parentBattleListTab, const wxPoint& pos, const wxSize& size, long style );

    void OnRankButton     ( wxCommandEvent& event );
    void OnPlayerButton   ( wxCommandEvent& event );
    void OnMaxPlayerButton( wxCommandEvent& event );
    void OnSpectatorButton( wxCommandEvent& event );
    void OnActivate       ( wxCommandEvent& event );

    void SetActiv         ( bool state );

    void OnChange            ( wxCommandEvent& event );
    void OnChangeMap         ( wxCommandEvent& event );
    void OnChangeMod         ( wxCommandEvent& event );
    void OnChangeDescription ( wxCommandEvent& event );
    void OnChangeHost        ( wxCommandEvent& event );

    void OnRankChange        ( wxCommandEvent& event );
    void OnPlayerChange      ( wxCommandEvent& event );
    void OnMaxPlayerChange   ( wxCommandEvent& event );
    void OnSpectatorChange   ( wxCommandEvent& event );

    bool FilterBattle(IBattle& battle);
    bool GetActiv() const;

    void SetFilterHighlighted( bool state );

    void SaveFilterValues();

	protected:
        enum m_button_mode {m_equal,m_bigger,m_smaller};

    wxString _GetButtonSign(m_button_mode value);
		m_button_mode _GetNextMode(m_button_mode value);
		m_button_mode _GetButtonMode(wxString sign);
		bool _IntCompare(int a,int b,m_button_mode mode);

    bool m_activ;

		BattleListTab* m_parent_battlelisttab;
#if wxUSE_TOGGLEBTN
		wxToggleButton* m_filter_show;
#else
		wxCheckBox* m_filter_show;
#endif
		wxStaticText* m_filter_text;

		wxCheckBox* m_filter_activ;

        //Host
		wxStaticText* m_filter_host_text;
		wxTextCtrl*   m_filter_host_edit;
        wxRegEx*      m_filter_host_expression;

        //Status
		wxStaticText* m_filter_status_text;
		wxStaticText* m_filter_status_text1;
		wxCheckBox* m_filter_status_locked;
		wxCheckBox* m_filter_status_pass;
		wxCheckBox* m_filter_status_start;
		wxCheckBox* m_filter_status_full;
		wxCheckBox* m_filter_status_open;

        //Rank
		wxStaticText* m_filter_rank_text;
		m_button_mode m_filter_rank_mode;
		wxButton* m_filter_rank_button;
		wxChoice* m_filter_rank_choice;
		int m_filter_rank_choice_value;

        //Description
		wxStaticText* m_filter_description_text;
		wxTextCtrl* m_filter_description_edit;
        wxRegEx*      m_filter_description_expression;

        //Player
		wxStaticText* m_filter_player_text;
		wxButton* m_filter_player_button;
		m_button_mode m_filter_player_mode;
		wxChoice* m_filter_player_choice;
		int m_filter_player_choice_value;

        //Map
		wxStaticText* m_filter_map_text;
		wxTextCtrl* m_filter_map_edit;
		wxCheckBox* m_filter_map_show;
        wxRegEx*    m_filter_map_expression;

        //Max Player
		wxStaticText* m_filter_maxplayer_text;
		wxButton* m_filter_maxplayer_button;
		m_button_mode m_filter_maxplayer_mode;
		wxChoice* m_filter_maxplayer_choice;
		int m_filter_maxplayer_choice_value;

        //Mod
		wxStaticText* m_filter_mod_text;
		wxTextCtrl* m_filter_mod_edit;
		wxCheckBox* m_filter_mod_show;
        wxRegEx*    m_filter_mod_expression;

        //Spectator
		wxStaticText* m_filter_spectator_text;
		wxButton* m_filter_spectator_button;
		m_button_mode m_filter_spectator_mode;
		wxChoice* m_filter_spectator_choice;
		int m_filter_spectator_choice_value;

        wxCheckBox* m_filter_highlighted;

    DECLARE_EVENT_TABLE();
};

enum
{
    BATTLE_FILTER_HOST_EDIT,
    BATTLE_FILTER_DESCRIPTION_EDIT,
    BATTLE_FILTER_MAP_EDIT,
    BATTLE_FILTER_MOD_EDIT,
    BATTLE_FILTER_LOCKED,
    BATTLE_FILTER_OPEN,
    BATTLE_FILTER_PASSWORDED,
    BATTLE_FILTER_FULL,
    BATTLE_FILTER_STARTED,
    BATTLE_FILTER_RANK_CHOICE,
    BATTLE_FILTER_RANK_BUTTON,
    BATTLE_FILTER_PLAYER_CHOICE,
    BATTLE_FILTER_MAXPLAYER_CHOICE,
    BATTLE_FILTER_SPECTATOR_CHOICE,
    BATTLE_FILTER_MAP_SHOW,
    BATTLE_FILTER_MOD_SHOW,
    BATTLE_FILTER_PLAYER_BUTTON,
    BATTLE_FILTER_MAXPLAYER_BUTTON,
    BATTLE_FILTER_SPECTATOR_BUTTON,
    BATTLE_FILTER_HIGHLIGHTED
};

#endif //SPRINGLOBBY_HEADERGUARD_BATTLELISTFILTER_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

