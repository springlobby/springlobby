#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

#include <wx/scrolwin.h>

class Ui;
class IBattle;
class wxCommandEvent;
class wxBoxSizer;
class wxStaticText;
class wxSplitterWindow;
class wxButton;
class wxCheckBox;
class wxListBox;
class wxRadioBox;
class wxCheckListBox;

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


    void OnRestrict( wxCommandEvent& event );
    void OnAllow( wxCommandEvent& event );
    void OnClearRestrictions( wxCommandEvent& event );
    void OnOptsCheck( wxCommandEvent& event );

  protected:

    void UpdateBattle();

    Ui& m_ui;
    IBattle& m_battle;

    wxCheckListBox* m_options_checks;
    wxStaticText* m_aloowed_lbl;
    wxListBox* m_allowed_list;
    wxButton* m_restrict_btn;
    wxButton* m_allow_btn;
    wxStaticText* m_restricted_lbl;
    wxListBox* m_restrict_list;
    wxButton* m_clear_btn;

    bool m_sp;

    DECLARE_EVENT_TABLE()
};

enum
{
  BOPTS_OPTS = wxID_HIGHEST,

  BOPTS_RESTRICT,
  BOPTS_ALLOW,
  BOPTS_CLEARRES
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
