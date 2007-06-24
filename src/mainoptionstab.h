//
// Class: MainOptionsTab
//

#ifndef _MAINOPTIONSTAB_H_
#define _MAINOPTIONSTAB_H_

#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/imaglist.h>
#include <wx/notebook.h>
#include <wx/listbook.h>
#include <wx/panel.h>
#include <wx/window.h>

#include "springoptionstab.h"

class Ui;


class MainOptionsTab : public wxPanel
{
  public:
    MainOptionsTab( wxWindow* parent, Ui& ui );
     ~MainOptionsTab();
  
    // MainOptionsTab interface
    /*
    void BattleUserUpdated( User& user );
    BattleRoomTab* GetBattleRoomTab() { return m_battle_tab; }
    */
  protected:
    // MainJoinBattleTab variables
  
    wxBoxSizer* m_main_sizer;
    
    wxImageList* m_imagelist;
  
    wxNotebook* m_tabs;
  
    SpringOptionsTab* m_spring_opts;

    wxButton* m_restore_btn;
    wxButton* m_defaults_btn;
    wxButton* m_apply_btn;

    wxBoxSizer* m_button_sizer;

    Ui& m_ui;
};

enum
{
    OPTIONS_TABS = wxID_HIGHEST
};

#endif  //_MAINOPTIONSTAB_H_

