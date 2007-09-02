#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

#include <wx/panel.h>

#include "iunitsync.h"

class Ui;
class IBattle;
class User;
class ChatPanel;
class wxCommandEvent;
class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxSplitterWindow;
class wxStaticLine;
class wxButton;
class wxCheckBox;
class MapCtrl;
class wxListBox;
class wxChoice;
class wxRadioBox;
class wxCheckListBox;
class wxSlider;

class BattleOptionsTab : public wxPanel
{
  public:
    BattleOptionsTab( wxWindow* parent, Ui& ui, IBattle& battle, bool singleplayer );
    ~BattleOptionsTab();

    void UpdateBattle();
    void ReloadRestrictions();

    int GetAllowedUnitIndex( const wxString& name );
    int GetRestrictedUnitIndex( const wxString& name );
    bool IsRestricted( const wxString& name );
    void Restrict( const wxString& name );
    void Allow( const wxString& name );
    void Restrict( int index );
    void Allow( int index );

    void OnEndSelect( wxCommandEvent& event );
    void OnOptsCheck( wxCommandEvent& event );
    void OnSlideChanged( wxScrollEvent& event );

    void OnRestrict( wxCommandEvent& event );
    void OnAllow( wxCommandEvent& event );
    void OnLoadRestrictions( wxCommandEvent& event );
    void OnSaveRestrictions( wxCommandEvent& event );
    void OnClearRestrictions( wxCommandEvent& event );

  protected:
    Ui& m_ui;
    IBattle& m_battle;

    wxRadioBox* m_end_radios;
    wxStaticText* m_metal_lbl;
    wxSlider* m_metal_slider;
    wxStaticLine* m_staticline2;
    wxStaticText* m_energy_lbl;
    wxSlider* m_energy_slider;
    wxStaticLine* m_staticline21;
    wxStaticText* m_units_lbl;
    wxSlider* m_units_slider;
    wxCheckListBox* m_options_checks;
    wxStaticText* m_aloowed_lbl;
    wxListBox* m_allowed_list;
    wxButton* m_restrict_btn;
    wxButton* m_allow_btn;
    wxStaticText* m_restricted_lbl;
    wxListBox* m_restrict_list;
    wxButton* m_load_btn;
    wxButton* m_save_btn;
    wxButton* m_clear_btn;

    int m_last_metal;
    int m_last_energy;
    int m_last_units;

    bool m_sp;

    DECLARE_EVENT_TABLE()
};

enum
{
  BOPTS_END = wxID_HIGHEST,
  BOPTS_OPTS,
  BOPTS_SLIDE,

  BOPTS_RESTRICT,
  BOPTS_ALLOW,
  BOPTS_LOADRES,
  BOPTS_SAVERES,
  BOPTS_CLEARRES
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
