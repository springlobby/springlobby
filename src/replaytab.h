#ifndef SPRINGLOBBY_REPLAYTAB_H_INCLUDED
#define SPRINGLOBBY_REPLAYTAB_H_INCLUDED

#include <wx/panel.h>

class Ui;
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
class ReplayList_Iter;
class Replay;
class ReplayList;
class ReplayListFilter;
class ReplayListCtrl;

class ReplayTab : public wxPanel
{
  friend class BattleListFilter;
  public:
    ReplayTab( wxWindow* parent, Ui& ui );
     ~ReplayTab();

    void AddReplay( Replay& Replay );
    void RemoveReplay( Replay& Replay );
    void UpdateReplay( Replay& Replay );

    void RemoveAllReplays();

    void UpdateList();

    void SelectReplay( Replay* Replay );

    void OnWatch( wxCommandEvent& event );
    void OnDelete( wxCommandEvent& event );
    void OnFilter( wxCommandEvent& event );
    void OnFilterActiv( wxCommandEvent& event );
    void SetFilterActiv(bool activ);

    void OnSelect( wxListEvent& event );
    void OnUnitSyncReloaded();
    void AddAllReplays();

  protected:
    ReplayList_Iter* m_replays_iter;
    ReplayList* m_replays;
    ReplayListFilter* m_filter;
    ReplayListCtrl* m_replay_listctrl;
    MapCtrl* m_minimap;
    wxStaticText* m_map_lbl;
    wxStaticText* m_map_text;
    wxStaticText* m_mod_lbl;
    wxStaticText* m_mod_text;
    wxStaticText* m_players_lbl;
    wxStaticText* m_players_text;

    wxStaticLine* m_buttons_sep;
    wxButton* m_watch_btn;
    wxButton* m_delete_btn;

    wxCheckBox* m_filter_activ;
#if wxUSE_TOGGLEBTN
		wxToggleButton* m_filter_show;
#else
		wxCheckBox* m_filter_show;
#endif

    Ui& m_ui;

    int m_sel_replay_id;

    DECLARE_EVENT_TABLE();
};

enum
{
    REPLAY_WATCH = wxID_HIGHEST,
    REPLAY_DELETE,
    REPLAY_LIST,
    REPLAY_LIST_FILTER_BUTTON,
    REPLAY_LIST_FILTER_ACTIV
};


#endif // SPRINGLOBBY_REPLAYTAB_H_INCLUDED
