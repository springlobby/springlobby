#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H

#include <wx/scrolwin.h>

#include "singleplayerbattle.h"


class Ui;
class MapCtrl;
class SinglePlayerBattle;

class wxWindow;
class wxButton;
class wxStaticLine;
class wxChoice;
class wxStaticText;
class wxCommandEvent;
class wxCheckBox;
class wxMouseEvent;
class ColorButton;

class SinglePlayerTab: public  wxScrolledWindow
{
  public:

    SinglePlayerTab( wxWindow* parent, Ui& ui, MainSinglePlayerTab& msptab );
    ~SinglePlayerTab();

    void UpdateMinimap();
    void ReloadMaplist();
    void ReloadModlist();
    void Update( const wxString& Tag );
    void UpdatePresetList();

    SinglePlayerBattle& GetBattle() { return m_battle; }

    bool ValidSetup();

    void OnMapSelect( wxCommandEvent& event );
    void OnModSelect( wxCommandEvent& event );
    void OnMapBrowse( wxCommandEvent& event );
    void OnAddBot( wxCommandEvent& event );
    void OnStart( wxCommandEvent& event );
    void OnRandomCheck( wxCommandEvent& event );
    void OnSpectatorCheck( wxCommandEvent& event );
    void OnColorButton( wxCommandEvent& event );
    void OnReset( wxCommandEvent& event );
    void OnMouseWheel( wxMouseEvent& event );

    void OnUnitSyncReloaded();
    void ResetUsername();

    void SetMap( unsigned int index );
    void SetMod( unsigned int index );

  protected:

    Ui& m_ui;
    SinglePlayerBattle m_battle;

    MapCtrl* m_minimap;
    wxChoice* m_map_pick;
    wxChoice* m_mod_pick;
    wxStaticText* m_map_lbl;
    wxStaticText* m_mod_lbl;
    wxButton* m_select_btn;
    wxButton* m_addbot_btn;
    wxStaticLine* m_buttons_sep;
    wxButton* m_reset_btn;
    wxCheckBox* m_random_check;
    wxCheckBox* m_spectator_check;
    wxButton* m_start_btn;
    ColorButton* m_color_btn;

    enum
    {
      SP_MAP_PICK = wxID_HIGHEST,
      SP_MOD_PICK,
      SP_BROWSE_MAP,
      SP_ADD_BOT,
      SP_RESET,
      SP_START,
      SP_RANDOM,
      SP_SPECTATE,
      SP_COLOUR
    };
   DECLARE_EVENT_TABLE()
};
#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H

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

