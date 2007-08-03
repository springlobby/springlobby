#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H

#include <wx/panel.h>

class Ui;
class Battle;
class BattleListCtrl;
class wxCommandEvent;
class wxListEvent;
class wxStaticText;
class wxComboBox;
class wxButton;
class wxBoxSizer;

class BattleListTab : public wxPanel
{
  public:
    BattleListTab( wxWindow* parent, Ui& ui );
     ~BattleListTab();

    void AddBattle( Battle& battle );
    void RemoveBattle( Battle& battle );
    void UpdateBattle( Battle& battle );

    void RemoveAllBattles();

    void UpdateList();

    void OnHost( wxCommandEvent& event );
    void OnJoin( wxCommandEvent& event );
    void OnListJoin( wxListEvent& event );

    void DoJoin( Battle& battle );

  protected:
    BattleListCtrl* m_battle_list;
    wxStaticText* m_filter_text;
    wxComboBox* m_filter_combo;
    wxButton* m_join_button;
    wxButton* m_host_button;

    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_tools_sizer;

    Ui& m_ui;

    DECLARE_EVENT_TABLE()
};

enum
{
    BATTLE_JOIN = wxID_HIGHEST,
    BATTLE_HOST
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
