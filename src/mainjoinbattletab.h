#ifndef SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H

#include <wx/panel.h>

class Ui;
class BattleListTab;
class Battle;
class User;
class BattleRoomTab;
class BattleMapTab;
class wxBoxSizer;
class wxImageList;
class wxNotebook;

class MainJoinBattleTab : public wxPanel
{
  public:
    MainJoinBattleTab( wxWindow* parent, Ui& ui );
     ~MainJoinBattleTab();

    BattleListTab& GetBattleListTab();

    void JoinBattle( Battle& battle );
    void UpdateCurrentBattle();
    void LeaveCurrentBattle();
    Battle* GetCurrentBattle();

    void BattleUserUpdated( User& user );
    BattleRoomTab* GetBattleRoomTab() { return m_battle_tab; }

  protected:
    wxBoxSizer* m_main_sizer;

    wxImageList* m_imagelist;

    wxNotebook* m_tabs;
    BattleListTab* m_list_tab;

    BattleRoomTab* m_battle_tab;
    BattleMapTab* m_map_tab;

    Ui& m_ui;
};

enum
{
    BATTLE_TABS = wxID_HIGHEST
};

#endif // SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H
