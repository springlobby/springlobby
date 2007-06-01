//
// Class: BattleListCtrl
//

#ifndef _BATTLELISTCTRL_H_
#define _BATTLELISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include "battle.h"

class BattleListCtrl : public wxListCtrl
{
  public:
    BattleListCtrl( wxWindow* parent );
    ~BattleListCtrl();
  
    // BattleListCtrl interface
  
    void AddBattle( Battle& battle );
    void RemoveBattle( Battle& battle );
    bool BattleExists( Battle& battle );
  
    static int GetStatusIcon( Battle& battle );
  
  protected:
    // BattleListCtrl variables
  
};


#endif  //_BATTLELISTCTRL_H_

