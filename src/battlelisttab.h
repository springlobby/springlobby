#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H

#include <wx/panel.h>

class Ui;
class Battle;
class BattleListCtrl;
class MapCtrl;
class NickListCtrl;
class wxCommandEvent;
class wxListEvent;
class wxStaticText;
class wxComboBox;
class wxButton;
class wxBoxSizer;
class wxStaticText;
class wxStaticLine;


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

    void SelectBattle( Battle* battle );

    void OnHost( wxCommandEvent& event );
    void OnJoin( wxCommandEvent& event );
    void OnListJoin( wxListEvent& event );

    void DoJoin( Battle& battle );

    void OnSelect( wxListEvent& event );

  protected:
    BattleListCtrl* m_battle_list;
    MapCtrl* m_minimap;
    wxStaticText* m_map_lbl;
    wxStaticText* m_map_text;
    wxStaticText* m_mod_lbl;
    wxStaticText* m_mod_text;
    wxStaticText* m_players_lbl;
    wxStaticText* m_players_text;
    wxStaticText* m_spec_lbl;
    wxStaticText* m_spec_text;
    NickListCtrl* m_players;
    wxStaticLine* m_buttons_sep;
    wxStaticText* m_filter_lbl;
    wxComboBox* m_filter_combo;
    wxButton* m_host_btn;
    wxButton* m_join_btn;

    Ui& m_ui;

    Battle* m_sel_battle;

    DECLARE_EVENT_TABLE();
};

enum
{
    BATTLE_JOIN = wxID_HIGHEST,
    BATTLE_HOST,
    BATTLE_LIST
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
