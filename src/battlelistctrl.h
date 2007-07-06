//
// Class: BattleListCtrl
//

#ifndef _BATTLELISTCTRL_H_
#define _BATTLELISTCTRL_H_

#include <wx/listctrl.h>
#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/utils.h>

#include "iconimagelist.h"
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
  
    int GetSelectedIndex();
  
    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );

  protected:
    // BattleListCtrl variables
    
    int m_selected;
    wxMenu* m_popup;
  
    DECLARE_EVENT_TABLE()
};

enum
{
    BLIST_LIST = wxID_HIGHEST,
    BLIST_DLMOD,
    BLIST_DLMAP
};


#endif  //_BATTLELISTCTRL_H_

