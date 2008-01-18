#ifndef BATTLEROOMMMOPTIONSTAB_H_
#define BATTLEROOMMMOPTIONSTAB_H_

#include <wx/scrolwin.h>
#include <map>
#include "mmoptionswrapper.h"

const int BOOL_START_ID = 3000;
const int FLOAT_START_ID = 4000;
const int LIST_START_ID = 5000;
const int STRING_START_ID = 6000;

class wxBoxSizer;
class wxStaticBoxSizer;
//class wxFlexGridSizer;
class mmOptionsWrapper;
class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class IBattle;
class wxSpinCtrlDbl;
class wxTextCtrl;
class wxSpinEvent;
class wxStaticText;

typedef std::map<wxString,wxCheckBox*> chkBoxMap;
typedef std::map<wxString,wxComboBox*> comboBoxMap;
typedef std::map<wxString,wxSpinCtrlDbl*> spinCtrlMap;
typedef std::map<wxString,wxTextCtrl*> textCtrlMap;
typedef std::map<wxString,wxStaticText*> staticTextMap;

class BattleroomMMOptionsTab : public wxScrolledWindow
{
	public:
		BattleroomMMOptionsTab( IBattle& battle, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~BattleroomMMOptionsTab();
		void setupMapOptionsSizer();
		void UpdateOptControls(wxString controlName);
		void OnRefreshControls(GameOption flag);
	protected:
		 IBattle& m_battle;

		wxBoxSizer* m_main_sizer;
		wxStaticBoxSizer* m_mod_options_sizer;
		wxBoxSizer* m_mod_layout;
		wxStaticBoxSizer* m_map_options_sizer;
		wxBoxSizer* m_map_layout;
		mmOptionsWrapper* m_mapmodoptions;

		chkBoxMap m_chkbox_map;
		comboBoxMap m_combox_map;
		spinCtrlMap m_spinctrl_map;
		textCtrlMap m_textctrl_map;
		staticTextMap m_statictext_map;

		void setupOptionsSizer(wxBoxSizer* optFlagSizer,GameOption optFlag);
		void OnComBoxChange(wxCommandEvent&);
		void OnChkBoxChange(wxCommandEvent&);
		void OnTextCtrlChange(wxCommandEvent& event);
		void OnSpinCtrlChange(wxSpinEvent& event);


		DECLARE_EVENT_TABLE();

};

#endif /*BATTLEROOMMMOPTIONSTAB_H_*/

