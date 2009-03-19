#ifndef SPRINGLOBBY_REPLAYTAB_H_INCLUDED
#define SPRINGLOBBY_REPLAYTAB_H_INCLUDED

#include <wx/panel.h>

class Ui;
class MapCtrl;
class BattleroomListCtrl;
class wxCommandEvent;
class wxListEvent;
class wxStaticText;
class wxComboBox;
class wxButton;
class wxBoxSizer;
class wxStaticText;
class wxStaticLine;
class wxCheckBox;
class wxToggleButton;
class ReplayList_Iter;
struct Replay;
class ReplayList;
class ReplayListFilter;
class ReplayListCtrl;

class ReplayTab : public wxPanel
{
  friend class BattleListFilter;
  public:
    //! loads all replays into list and adds them to listctrl
    ReplayTab( wxWindow* parent, Ui& ui );
     ~ReplayTab();

    //! adds a single replay to listctrl
    void AddReplay( Replay& Replay );
    void RemoveReplay( Replay& Replay );
    void UpdateReplay( Replay& Replay );

    //! add all replays in m_replays to listctrl
    void AddAllReplays();
    void RemoveAllReplays();
    void ReloadList();

    void UpdateList();

    //! calls ui::watch which executes spring
    void OnWatch( wxCommandEvent& event );
    //! clears list and parses all replays anew
    void OnReload( wxCommandEvent& event );
    //! does nothing yet
    void OnDelete( wxCommandEvent& event );
        //! does nothing yet
    void OnFilter( wxCommandEvent& event );
        //! does nothing yet
    void OnFilterActiv( wxCommandEvent& event );

    //! sets m_sel_replay_id according to selected listitem
    void OnSelect( wxListEvent& event );
        //! does nothing yet
    void SetFilterActiv(bool activ);

    void Deselect();
    void Deselected();
    void OnDeselect( wxListEvent& event );

  protected:
    ReplayList* m_replays;
    ReplayListFilter* m_filter;
    ReplayListCtrl* m_replay_listctrl;
    MapCtrl* m_minimap;
    wxStaticText* m_map_lbl;
    wxStaticText* m_map_text;
    wxStaticText* m_mod_lbl;
    wxStaticText* m_mod_text;
    wxStaticText* m_players_lbl;
    wxStaticText* m_players_text;

    wxStaticLine* m_buttons_sep;
    wxButton* m_watch_btn;
    wxButton* m_delete_btn;
    wxButton* m_reload_btn;

    BattleroomListCtrl* m_players;

    wxCheckBox* m_filter_activ;
#if wxUSE_TOGGLEBTN
		wxToggleButton* m_filter_show;
#else
		wxCheckBox* m_filter_show;
#endif

    Ui& m_ui;

    DECLARE_EVENT_TABLE();
};

enum
{
    REPLAY_WATCH = wxID_HIGHEST,
    REPLAY_DELETE,
    REPLAY_RELOAD,
    REPLAY_LIST,
    REPLAY_LIST_FILTER_BUTTON,
    REPLAY_LIST_FILTER_ACTIV,
    REPLAY_TIMER,
    REPLAY_USER_LIST
};


#endif // SPRINGLOBBY_REPLAYTAB_H_INCLUDED

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

