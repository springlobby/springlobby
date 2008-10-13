#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

#include <wx/scrolwin.h>

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

/** \brief manipulate "standard" engine options, unit restriction list
 * \todo DOCMEMORE */
class BattleOptionsTab : public wxScrolledWindow
{
  public:
    BattleOptionsTab( wxWindow* parent, Ui& ui, IBattle& battle, bool singleplayer );
    ~BattleOptionsTab();

    void UpdateBattle( const wxString& Tag );
    void ReloadRestrictions();

    int GetAllowedUnitIndex( const wxString& name );
    int GetRestrictedUnitIndex( const wxString& name );
    bool IsRestricted( const wxString& name );
    void Restrict( const wxString& name );
    void Allow( const wxString& name );
    void Restrict( int index );
    void Allow( int index );

    void UpdatePresetList();

    void OnEndSelect( wxCommandEvent& event );
    void OnOptsCheck( wxCommandEvent& event );
    void OnSlideChanged( wxScrollEvent& event );

    void OnRestrict( wxCommandEvent& event );
    void OnAllow( wxCommandEvent& event );
    void OnClearRestrictions( wxCommandEvent& event );

    void OnLoadPreset( wxCommandEvent& event );
    void OnSavePreset( wxCommandEvent& event );
    void OnDeletePreset( wxCommandEvent& event );
    void OnSetModDefaultPreset( wxCommandEvent& event );

  protected:

    void UpdateBattle();

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
    wxButton* m_clear_btn;
    wxButton* m_load_btn;
    wxButton* m_save_btn;
    wxButton* m_delete_btn;
    wxButton* m_default_btn;
    wxComboBox* m_options_preset_sel;

    int m_last_metal;
    int m_last_energy;
    int m_last_units;

    bool m_sp;

    enum {
      BOPTS_END = wxID_HIGHEST,
      BOPTS_OPTS,
      BOPTS_SLIDE,

      BOPTS_RESTRICT,
      BOPTS_ALLOW,
      BOPTS_CLEARRES,

      BOPTS_LOADPRES,
      BOPTS_SAVEPRES,
      BOPTS_DELETEPRES,
      BOPTS_SETDEFAULTPRES,
      BOPTS_CHOSEPRES
    };

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
