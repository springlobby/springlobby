#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H

#include <wx/panel.h>

#include "battlelistfilter.h"
#include "battlelist.h"
#include "user.h"


class Ui;
class Battle;
class BattleListCtrl;
class BattleListFilter;
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
class wxCheckBox;
class wxToggleButton;

/** \brief The panel containing a BattleListCtrl and a BattleListFilter
 * \todo DOCME */
class BattleListTab : public wxPanel
{
  friend class BattleListFilter;
  public:
    BattleListTab( wxWindow* parent, Ui& ui );
     ~BattleListTab();

    void AddBattle( Battle& battle );
    void RemoveBattle( Battle& battle );
    void UpdateBattle( Battle& battle );

    void UserUpdate( User& user );

    void RemoveAllBattles();

    void UpdateList();

    void SelectBattle( Battle* battle );

    void OnHost( wxCommandEvent& event );
    void OnFilter( wxCommandEvent& event );
    void OnFilterActiv( wxCommandEvent& event );
    void SetFilterActiv(bool activ);
    void OnJoin( wxCommandEvent& event );
    void OnListJoin( wxListEvent& event );

    void DoJoin( Battle& battle );

    void OnSelect( wxListEvent& event );
    void OnUnitSyncReloaded();

    void UpdateHighlights();

  protected:
    BattleListFilter* m_filter;
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
    wxButton* m_host_btn;
    wxButton* m_join_btn;

    wxCheckBox* m_filter_activ;
#if wxUSE_TOGGLEBTN
		wxToggleButton* m_filter_show;
#else
		wxCheckBox* m_filter_show;
#endif

    Ui& m_ui;

    Battle* m_sel_battle;

    DECLARE_EVENT_TABLE();
};

enum
{
    BATTLE_JOIN = wxID_HIGHEST,
    BATTLE_HOST,
    BATTLE_LIST,
    BATTLE_LIST_FILTER_BUTTON,
    BATTLE_LIST_FILTER_ACTIV
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
