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
#include "battlelisttab.h"


class MainJoinBattleTab : public wxPanel
{
  public:
    MainJoinBattleTab( wxWindow* parent );
     ~MainJoinBattleTab();
  
    // MainJoinBattleTab interface
  
    BattleListTab& GetBattleListTab();
  
  protected:
    // MainJoinBattleTab variables
  
    wxBoxSizer* m_main_sizer;
    
    wxImageList* m_imagelist;
  
    wxNotebook* m_tabs;
    BattleListTab* m_list_tab;
};

enum
{
    BATTLE_TABS = wxID_HIGHEST
};

#endif  //_MAINJOINBATTLETAB_H_

