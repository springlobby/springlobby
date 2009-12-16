#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H

#include <wx/scrolwin.h>
#include "utils/isink.h"

class Ui;
class Battle;
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
class MapCtrl;
class wxListCtrl;
class wxChoice;
class wxRadioBox;
class wxMouseEvent;
class MapSelectDialog;

/** \brief select map, draw startboxes (in sp define startpos)
 * \todo DOCMEMORE */
class BattleMapTab : public wxScrolledWindow, public UnitsyncReloadedSink< BattleMapTab >
{
  public:
    BattleMapTab( wxWindow* parent, Battle* battle );
     ~BattleMapTab();

     void UpdateUser( User& user );

     void Update();
     void Update( const wxString& Tag );
     void ReloadMaplist();

     void OnMapSelect( wxCommandEvent& event );
     void OnMapBrowse( wxCommandEvent& event );
     void OnStartTypeSelect( wxCommandEvent& event );
     void OnMouseWheel( wxMouseEvent& event );

    void OnUnitsyncReloaded( GlobalEvents::GlobalEventData /*data*/ );

    void SetMap( int index );
    void SetBattle( Battle* battle );
    Battle* GetBattle() { return m_battle; }

  protected:
    Battle* m_battle;
    //UnitSyncMap m_map;

    MapCtrl* m_minimap;
    wxChoice* m_map_combo;
    wxButton* m_browse_btn;
    wxRadioBox* m_start_radios;
    wxListCtrl* m_map_opts_list;
    MapSelectDialog* m_map_dlg;

    enum {
      BMAP_MAP_SEL = wxID_HIGHEST,
      BMAP_MAP_BROWSE,
      BMAP_START_TYPE
    };

    DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H

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

