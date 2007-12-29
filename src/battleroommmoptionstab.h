#ifndef BATTLEROOMMMOPTIONSTAB_H_
#define BATTLEROOMMMOPTIONSTAB_H_

#include <wx/panel.h>
#include <vector>

const int BOOL_START_ID = 3000;
const int FLOAT_START_ID = 4000;
const int LIST_START_ID = 5000;
const int STRING_START_ID = 6000;

class wxBoxSizer;
class wxStaticBoxSizer;
class wxFlexGridSizer;
class mmOptionsWrapper;
class wxCheckBox;
class wxComboBox;
class wxCommandEvent;

typedef std::vector<wxCheckBox*> chkBoxVec;
typedef std::vector<wxComboBox*> comboBoxVec;
//add similar for other controls

class BattleroomMMOptionsTab : public wxPanel 
{
	public:
		BattleroomMMOptionsTab( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~BattleroomMMOptionsTab();
		void setupMapOptionsSizer();
		void UpdateOptControls(/* wxstringpairvec* list **/);
	protected:
		wxBoxSizer* m_main_sizer;
		wxStaticBoxSizer* m_mod_options_sizer;
		wxFlexGridSizer* m_mod_layout;
		wxStaticBoxSizer* m_map_options_sizer;
		wxFlexGridSizer* m_map_layout;
		mmOptionsWrapper* m_mapmodoptions;
		
		chkBoxVec m_chkbox_vec;
		comboBoxVec m_combox_vec;
		//add similar for other controls
		
		void setupModOptionsSizer();
		void OnComBoxChange(wxCommandEvent&);
		void OnChkBoxChange(wxCommandEvent&);
	
		
		DECLARE_EVENT_TABLE();
	
};

#endif /*BATTLEROOMMMOPTIONSTAB_H_*/

