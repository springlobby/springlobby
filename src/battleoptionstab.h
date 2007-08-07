#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

#include <wx/panel.h>

#include "iunitsync.h"

class Ui;
class Battle;
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
class wxListCtrl;
class wxChoice;
class wxRadioBox;
class wxCheckListBox;
class wxSlider;

class BattleOptionsTab : public wxPanel
{
  public:
    BattleOptionsTab( wxWindow* parent, Ui& ui, Battle& battle );
     ~BattleOptionsTab();

  protected:
    Ui& m_ui;
    Battle& m_battle;

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
		wxListCtrl* m_allowed_list;
		wxButton* m_restrict_btn;
		wxButton* m_allow_btn;
		wxStaticText* m_restricted_lbl;
		wxListCtrl* m_restrict_list;
		wxButton* m_load_btn;
		wxButton* m_save_btn;
		wxButton* m_clear_btn;

    DECLARE_EVENT_TABLE()
};

enum
{
  BOPTS_MAP_SEL = wxID_HIGHEST,
  BOPTS_START_TYPE
};

#endif // SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
