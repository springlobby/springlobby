//
// Class: MainJoinBattleTab
//

#ifndef _MAINJOINBATTLETAB_H_
#define _MAINJOINBATTLETAB_H_

#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include "battlelisttab.h"
#include "battleroomtab.h"


class MainJoinBattleTab : public wxPanel
{
  public:
    MainJoinBattleTab( wxWindow* parent, Ui& ui );
     ~MainJoinBattleTab();
  
    // MainJoinBattleTab interface
  
    BattleListTab& GetBattleListTab();
   
    void JoinBattle( Battle& battle );
    void LeaveCurrentBattle();
    
    void BattleUserUpdated( User& user );
    BattleRoomTab* GetBattleRoomTab() { return m_battle_tab; }
    
  protected:
    // MainJoinBattleTab variables
  
    wxBoxSizer* m_main_sizer;
    
    wxImageList* m_imagelist;
  
    wxNotebook* m_tabs;
    BattleListTab* m_list_tab;
  
    BattleRoomTab* m_battle_tab;
  
    Ui& m_ui;
};

enum
{
    BATTLE_TABS = wxID_HIGHEST
};

#endif  //_MAINJOINBATTLETAB_H_

