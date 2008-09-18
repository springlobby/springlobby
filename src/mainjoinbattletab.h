#ifndef SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H

#include <wx/panel.h>

class Ui;
class BattleListTab;
class Battle;
class User;
class BattleRoomTab;
class BattleMapTab;
class BattleOptionsTab;
class wxBoxSizer;
class wxImageList;
class wxNotebook;
class BattleroomMMOptionsTab;

class MainJoinBattleTab : public wxPanel
{
  public:
    MainJoinBattleTab( wxWindow* parent, Ui& ui );
     ~MainJoinBattleTab();

    BattleListTab& GetBattleListTab();

    void HostBattle( Battle& battle );
    void JoinBattle( Battle& battle );
    //void UpdateCurrentBattle();
    void UpdateCurrentBattle( bool MapChanged = false,  bool UpdateRestrictions = false );
    void UpdateCurrentBattle( const wxString& Tag );
    void LeaveCurrentBattle();
    Battle* GetCurrentBattle();
    ChatPanel* GetActiveChatPanel();

    void BattleUserUpdated( User& user );
    BattleRoomTab* GetBattleRoomTab() { return m_battle_tab; }
    BattleMapTab* GetBattleMapTab() { return m_map_tab; }

    void OnUnitSyncReloaded();

    void ReloadMMoptTab();

    void OnConnected();

  protected:
    wxBoxSizer* m_main_sizer;

    wxImageList* m_imagelist;

    wxNotebook* m_tabs;
    BattleListTab* m_list_tab;

    BattleRoomTab* m_battle_tab;
    BattleMapTab* m_map_tab;
    BattleOptionsTab* m_opts_tab;
    BattleroomMMOptionsTab* m_mm_opts_tab;
    Ui& m_ui;


};

enum
{
    BATTLE_TABS = wxID_HIGHEST
};

#endif // SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H
