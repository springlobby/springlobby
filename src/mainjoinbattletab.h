#ifndef SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H

#include <wx/scrolwin.h>
#include "battleroommmoptionstab.h"

class Ui;
class BattleListTab;
class Battle;
class User;
class BattleRoomTab;
class BattleMapTab;
class BattleOptionsTab;
class wxBoxSizer;
class wxImageList;
class wxAuiNotebook;
class wxNotebook;

class MainJoinBattleTab : public wxScrolledWindow
{
  public:
    MainJoinBattleTab( wxWindow* parent, Ui& ui );
     ~MainJoinBattleTab();

    BattleListTab& GetBattleListTab();

    void HostBattle( Battle& battle );
    void JoinBattle( Battle& battle );
    //void UpdateCurrentBattle();
    void UpdateCurrentBattle();
    void UpdateCurrentBattle( const wxString& Tag );
    void LeaveCurrentBattle();
    Battle* GetCurrentBattle();
    ChatPanel* GetActiveChatPanel();

    void BattleUserUpdated( User& user );
    BattleRoomTab& GetBattleRoomTab();
    BattleMapTab& GetBattleMapTab();
    BattleOptionsTab& GetOptionsTab();
    BattleroomMMOptionsTab<Battle>& GetMMOptionsTab();

    void ReloadPresetList();

    void OnConnected();

  protected:
    wxBoxSizer* m_main_sizer;

    wxImageList* m_imagelist;

    wxAuiNotebook* m_tabs;

    BattleListTab* m_list_tab;

    BattleRoomTab* m_battle_tab;
    BattleMapTab* m_map_tab;
    BattleOptionsTab* m_opts_tab;
    BattleroomMMOptionsTab<Battle>* m_mm_opts_tab;
    Ui& m_ui;

    enum {
        BATTLE_TABS = wxID_HIGHEST
    };

};



#endif // SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H

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

