//
// Class: BattleListTab
//

#ifndef _BATTLELISTTAB_H_
#define _BATTLELISTTAB_H_

#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include "battlelistctrl.h"

class Ui;
  

class BattleListTab : public wxPanel
{
    public:
      
    BattleListTab( wxWindow* parent, Ui& ui );
     ~BattleListTab();
  
    void AddBattle( Battle& battle );
    void RemoveBattle( Battle& battle );
    void UpdateBattle( Battle& battle );
  
    void OnJoin( wxCommandEvent& event );
  protected:
  
    BattleListCtrl* m_battle_list;
    wxStaticText* m_filter_text;
    wxComboBox* m_filter_combo;
    wxButton* m_join_button;
  
    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_tools_sizer;
  
    Ui& m_ui;
  
    DECLARE_EVENT_TABLE()
};

enum
{
    BATTLE_JOIN = wxID_HIGHEST
};

#endif  //_BATTLELISTTAB_H_

