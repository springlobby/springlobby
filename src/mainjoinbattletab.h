#ifndef SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H
#define SPRINGLOBBY_HEADERGUARD_MAINJOINBATTLETAB_H

#include <wx/scrolwin.h>

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
class BattleroomMMOptionsTab;

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
    BattleroomMMOptionsTab& GetMMOptionsTab();

    void ReloadPresetList();

    void OnUnitSyncReloaded();

    void OnConnected();

  protected:
    wxBoxSizer* m_main_sizer;

    wxImageList* m_imagelist;

    #ifdef HAVE_WX26
    wxNotebook* m_tabs;
    #else
    wxAuiNotebook* m_tabs;
    #endif
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
