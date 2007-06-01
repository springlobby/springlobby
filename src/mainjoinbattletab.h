//
// Class: MainJoinBattleTab
//

#ifndef _MAINJOINBATTLETAB_H_
#define _MAINJOINBATTLETAB_H_

#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include "battlelistctrl.h"


class MainJoinBattleTab : public wxPanel
{
  public:
    MainJoinBattleTab( wxWindow* parent );
     ~MainJoinBattleTab();
  
    // MainJoinBattleTab interface
  
    void AddBattle( Battle& battle );
    void RemoveBattle( Battle& battle );
    void UpdateBattle( Battle& battle );
  
  protected:
    // MainJoinBattleTab variables
  
    BattleListCtrl* m_battle_list;
    wxStaticText* m_filter_text;
    wxComboBox* m_filter_combo;
    wxButton* m_join_button;
  
    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_tools_sizer;
  
};


#endif  //_MAINJOINBATTLETAB_H_

