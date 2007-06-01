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
    void UpdateBattle( Battle& battle );
    void UpdateBattle( const int& index );
    bool BattleExists( Battle& battle );
  
    int GetBattleIndex( Battle& battle );
  
    
    static wxString RefineMapname( wxString mapname );
    static wxString RefineModname( wxString modname );
  
  protected:
    // BattleListCtrl variables
  
};


#endif  //_BATTLELISTCTRL_H_

