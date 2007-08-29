#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

#include <wx/listctrl.h>
#include <wx/string.h>

class wxMenu;
class Battle;
class wxListEvent;
class wxCommandEvent;
class Ui;


class BattleListCtrl : public wxListCtrl
{
  public:
    BattleListCtrl( wxWindow* parent, Ui& ui );
    ~BattleListCtrl();

    void AddBattle( Battle& battle );
    void RemoveBattle( Battle& battle );
    void UpdateBattle( Battle& battle );
    void UpdateBattle( const int& index );
    bool BattleExists( Battle& battle );

    void UpdateList();

    int GetBattleIndex( Battle& battle );

    int GetSelectedIndex();

    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );

  protected:

    int m_selected;
    wxMenu* m_popup;
    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

enum
{
    BLIST_LIST = wxID_HIGHEST,
    BLIST_DLMOD,
    BLIST_DLMAP
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
