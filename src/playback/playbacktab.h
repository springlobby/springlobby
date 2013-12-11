#ifndef SPRINGLOBBY_PlaybackTab_H_INCLUDED
#define SPRINGLOBBY_PlaybackTab_H_INCLUDED

#include <wx/scrolwin.h>
#include <vector>
#include "utils/globalevents.h"

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

template <class PlaybackTabType>
class PlaybackListFilter;

template <class PlaybackType>
class PlaybackListCtrl;

template <class PlaybackType>
class PlaybackLoader;

template <class PlaybackTraitsImp>
class PlaybackTab : public GlobalEvent, public wxScrolledWindow
{
    protected:
        friend class BattleListFilter; //! WTF?

    public:
        typedef PlaybackTraitsImp
            PlaybackTraits;
        typedef typename PlaybackTraits::PlaybackType
            PlaybackType;
        typedef PlaybackTab<PlaybackTraits>
            ThisType;
        typedef typename PlaybackTraits::ListType
            ListType;
        typedef PlaybackListCtrl<PlaybackType>
            ListCtrlType;
        typedef PlaybackLoader<ThisType>
            LoaderType;

        static const bool IsReplayType = PlaybackTraits::IsReplayType;

  public:
    //! loads all replays into list and adds them to listctrl
    PlaybackTab( wxWindow* parent );
     ~PlaybackTab();

    //! adds a single replay to listctrl
    void AddPlayback( const PlaybackType& Replay );
    void RemovePlayback( const PlaybackType& Replay );
    void RemovePlayback( const int index );
    void UpdatePlayback( const PlaybackType& Replay );

    //! add all replays in m_replays to listctrl
    void AddAllPlaybacks( wxCommandEvent& evt );
    void RemoveAllPlaybacks();
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

    void OnSpringTerminated( wxCommandEvent& data );
	void OnUnitsyncReloaded( wxCommandEvent& data );

  protected:
    PlaybackListFilter<ThisType>* m_filter;
    ListCtrlType* m_replay_listctrl;
    LoaderType* m_replay_loader;
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

    void AskForceWatch( PlaybackType& rep  ) const;

	DECLARE_EVENT_TABLE()
};

enum
{
    PLAYBACK_WATCH = wxID_HIGHEST,
    PLAYBACK_DELETE,
    PLAYBACK_RELOAD,
    PLAYBACK_LIST,
    PLAYBACK_LIST_FILTER_BUTTON,
    PLAYBACK_LIST_FILTER_ACTIV,
    PLAYBACK_TIMER,
    PLAYBACK_USER_LIST
};

#include "playbacktab.cpp"
#endif // SPRINGLOBBY_PlaybackTab_H_INCLUDED

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

