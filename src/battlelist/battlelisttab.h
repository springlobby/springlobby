#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H

#include <wx/scrolwin.h>
#include "utils/globalevents.h"
#include "utils/mixins.hh"

class User;
class Ui;
class IBattle;
class Battle;
class BattleListCtrl;
class BattleListFilter;
class MapCtrl;
class NickListCtrl;
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
class wxFlexGridSizer;
/** \brief The panel containing a BattleListCtrl and a BattleListFilter
 * \todo DOCME */
class BattleListTab : public wxScrolledWindow, public GlobalEvent, public SL::NonCopyable
{
  friend class BattleListFilter;
  public:
    BattleListTab( wxWindow* parent );
     ~BattleListTab();

		void OnConnected();

    void AddBattle( IBattle& battle );
    void RemoveBattle( IBattle& battle );
    void UpdateBattle( IBattle& battle );

    void UserUpdate( User& user );

    void RemoveAllBattles();

    void UpdateList();

    void SelectBattle( IBattle* battle );

    void OnHost( wxCommandEvent& event );
    void OnFilter( wxCommandEvent& event );
    void OnInfoShow( wxCommandEvent& event );
    void OnFilterActiv( wxCommandEvent& event );
    void SetFilterActiv(bool activ);
    void OnJoin( wxCommandEvent& event );
    void OnListJoin( wxListEvent& event );
    void ShowExtendedInfos( bool show );

    void DoJoin( Battle& battle );

    void OnSelect( wxListEvent& event );
    void OnUnitsyncReloaded( wxCommandEvent& data );

    void UpdateHighlights();

    void SortBattleList();

  protected:
    BattleListFilter* m_filter;
    BattleListCtrl* m_battle_list;
    MapCtrl* m_minimap;
    wxStaticText* m_map_lbl;
    wxStaticText* m_map_text;
    wxStaticText* m_mod_lbl;
    wxStaticText* m_mod_text;
    wxStaticText* m_players_lbl;
    wxStaticText* m_players_text;
    wxStaticText* m_spec_lbl;
    wxStaticText* m_spec_text;
    wxStaticText* m_battle_num;
    NickListCtrl* m_players;
    wxStaticLine* m_buttons_sep;
    wxButton* m_host_btn;
    wxButton* m_join_btn;
    wxBoxSizer* m_battlelist_sizer;
    wxFlexGridSizer* m_data_sizer;
    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_info_sizer;

    wxCheckBox* m_filter_activ;

#if wxUSE_TOGGLEBTN
		wxToggleButton* m_filter_show;
		wxToggleButton* m_info_show;
#else
		wxCheckBox* m_filter_show;
		wxCheckBox* m_info_show;
#endif

    IBattle* m_sel_battle;

    void SetNumDisplayed();

    enum {
        BATTLE_JOIN = wxID_HIGHEST,
        BATTLE_HOST,
        BATTLE_LIST,
        BATTLE_LIST_FILTER_BUTTON,
        BATTLE_LIST_INFO_BUTTON,
        BATTLE_LIST_FILTER_ACTIV
    };

	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H

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

