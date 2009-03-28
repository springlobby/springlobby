#ifndef SPRINGLOBBY_REPLAYFILTER_H_INCLUDED
#define SPRINGLOBBY_REPLAYFILTER_H_INCLUDED

#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

class ReplayTab;
class wxToggleButton;
class wxCheckBox;
class wxStaticText;
class wxTextCtrl;
class wxChoice;
class wxButton;
class wxRegEx;
class wxStaticText;
struct Replay;


/** \brief The panel contained in BattleListTab used to filter for diff info of battles
 * \todo DOCMEMORE */
class ReplayListFilter : public wxPanel
{
	public:
    ReplayListFilter( wxWindow* parent, wxWindowID id, ReplayTab* parentTab, const wxPoint& pos, const wxSize& size, long style );

    void OnPlayerButton   ( wxCommandEvent& event );
    void OnFilesizeButton ( wxCommandEvent& event );
    void OnDurationButton   ( wxCommandEvent& event );

    void OnActivate       ( wxCommandEvent& event );

    void SetActiv         ( bool state );

    void OnChange            ( wxCommandEvent& event );
    void OnChangeMap         ( wxCommandEvent& event );
    void OnChangeMod         ( wxCommandEvent& event );
    void OnChangeFilesize    ( wxCommandEvent& event );
    void OnChangeDuration    ( wxCommandEvent& event );

    void OnPlayerChange      ( wxCommandEvent& event );

    bool FilterReplay( const Replay& replay );
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

		ReplayTab* m_parent_tab;
#if wxUSE_TOGGLEBTN
		wxToggleButton* m_filter_show;
#else
		wxCheckBox* m_filter_show;
#endif
		wxStaticText* m_filter_text;

		wxCheckBox* m_filter_activ;

        //Player
		wxStaticText* m_filter_player_text;
		wxButton* m_filter_player_button;
		m_button_mode m_filter_player_mode;
		wxChoice* m_filter_player_choice;
		int m_filter_player_choice_value;

        //Filesize
		wxStaticText* m_filter_filesize_text;
		wxButton* m_filter_filesize_button;
		m_button_mode m_filter_filesize_mode;
		wxTextCtrl* m_filter_filesize_edit;

        //Duration
		wxStaticText* m_filter_duration_text;
		wxButton* m_filter_duration_button;
		m_button_mode m_filter_duration_mode;
		wxTextCtrl* m_filter_duration_edit;
		long m_duration_value;
		void SetDurationValue();

        //Map
		wxStaticText* m_filter_map_text;
		wxTextCtrl* m_filter_map_edit;
		wxCheckBox* m_filter_map_show;
        wxRegEx*    m_filter_map_expression;

        //Mod
		wxStaticText* m_filter_mod_text;
		wxTextCtrl* m_filter_mod_edit;
		wxCheckBox* m_filter_mod_show;
        wxRegEx*    m_filter_mod_expression;


    DECLARE_EVENT_TABLE();
};

enum
{
    REPLAY_FILTER_MAP_EDIT,
    REPLAY_FILTER_MOD_EDIT,
    REPLAY_FILTER_FILESIZE_EDIT,
    REPLAY_FILTER_DURATION_EDIT,
    REPLAY_FILTER_PLAYER_CHOICE,
    REPLAY_FILTER_MAP_SHOW,
    REPLAY_FILTER_MOD_SHOW,
    REPLAY_FILTER_PLAYER_BUTTON,
    REPLAY_FILTER_DURATION_BUTTON,
    REPLAY_FILTER_FILESIZE_BUTTON
};

#endif // SPRINGLOBBY_REPLAYFILTER_H_INCLUDED

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

