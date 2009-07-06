#ifndef SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H

#include <wx/scrolwin.h>
#include "battleroommmoptionstab.h"

class Ui;
class wxNotebook;
class wxAuiNotebook;
class wxImageList;
class wxBoxSizer;
class SinglePlayerTab;
class BattleOptionsTab;

class MainSinglePlayerTab : public wxScrolledWindow
{
  public:
    MainSinglePlayerTab( wxWindow* parent );
    ~MainSinglePlayerTab();

    void UpdateMinimap();
    void OnUnitSyncReloaded();
    void Update( const wxString& Tag );

    void ReloadRestrictions();
    void ReloadMapOptContrls();
    void ReloadModOptContrls();
    void ReloadPresetList();

    SinglePlayerTab& GetSinglePlayerTab();
    BattleOptionsTab& GetOptionsTab();
    BattleroomMMOptionsTab<SinglePlayerBattle>& GetMMOptionsTab();

  protected:

    wxBoxSizer* m_main_sizer;
    wxImageList* m_imagelist;
    wxAuiNotebook* m_tabs;

    SinglePlayerTab* m_sp_tab;
    BattleOptionsTab* m_opts_tab;
    BattleroomMMOptionsTab<SinglePlayerBattle>* m_mm_opts_tab;

};


#endif // SPRINGLOBBY_HEADERGUARD_MAINSINGLEPLAYERTAB_H

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

