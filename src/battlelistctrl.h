//
// Class: BattleListCtrl
//

#ifndef _BATTLELISTCTRL_H_
#define _BATTLELISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include "battle.h"

#define ICON_GAME_UNKNOWN -1
#define ICON_OPEN_GAME 0
#define ICON_OPEN_PW_GAME 1
#define ICON_CLOSED_GAME 2
#define ICON_CLOSED_PW_GAME 3
#define ICON_STARTED_GAME 4


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
  
    static int GetStatusIcon( Battle& battle );
  
  protected:
    // BattleListCtrl variables
  
};


#endif  //_BATTLELISTCTRL_H_

