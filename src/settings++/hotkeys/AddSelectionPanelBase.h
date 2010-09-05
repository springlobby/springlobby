///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __AddSelectionPanelBase__
#define __AddSelectionPanelBase__

#include <wx/string.h>
#include <wx/radiobut.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AddSelectionCmdDlg
///////////////////////////////////////////////////////////////////////////////
class AddSelectionCmdDlg : public wxDialog 
{
	private:
	
	protected:
		wxRadioButton* m_radioBtn14;
		wxRadioButton* m_radioBtn15;
		wxRadioButton* m_radioBtn16;
		wxRadioButton* m_radioBtn18;
		wxTextCtrl* m_textCtrl5;
		wxRadioButton* m_radioBtn8;
		wxRadioButton* m_radioBtn12;
		wxTextCtrl* m_textCtrl3;
		wxRadioButton* m_radioBtn11;
		wxRadioButton* m_radioBtn13;
		wxTextCtrl* m_textCtrl4;
		wxCheckBox* m_checkBox2;
		wxStaticText* m_staticText9;
		wxStaticText* m_staticText11;
		
		wxStaticText* m_staticText12;
		wxStaticText* m_staticText13;
		
		
		wxCheckBox* m_checkBoxBuilderInc;
		wxCheckBox* m_checkBoxBuilderEx;
		wxStaticText* m_staticText14;
		wxCheckBox* m_checkBoxBuildingInc;
		wxCheckBox* m_checkBoxBuildingEx;
		wxStaticText* m_staticText15;
		
		wxCheckBox* m_checkBoxComInc;
		wxCheckBox* m_checkBoxComEx;
		wxStaticText* m_staticText17;
		wxCheckBox* m_checkBoxTypeInc;
		wxCheckBox* m_checkBoxTypeEx;
		wxStaticText* m_staticText18;
		
		wxCheckBox* m_checkBoxAirInc;
		wxCheckBox* m_checkBoxAirEx;
		wxStaticText* m_staticText19;
		wxCheckBox* m_checkBoxRelHealthInc;
		wxCheckBox* m_checkBoxRelHealthEx;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_textCtrlRelHealth;
		wxCheckBox* m_checkBoxWeaponsInc;
		wxCheckBox* m_checkBoxWeaponsEx;
		wxStaticText* m_staticText21;
		wxCheckBox* m_checkBoxWeapRangeInc;
		wxCheckBox* m_checkBoxWeapRangeEx;
		wxStaticText* m_staticText22;
		wxTextCtrl* m_textCtrlWeapRange;
		wxCheckBox* m_checkBoxIdleInc;
		wxCheckBox* m_checkBoxIdleEx;
		wxStaticText* m_staticText23;
		wxCheckBox* m_checkBoxAbsHealthInc;
		wxCheckBox* m_checkBoxAbsHealthEx;
		wxStaticText* m_staticText24;
		wxTextCtrl* m_textCtrl51;
		wxCheckBox* m_checkBoxRadarInc;
		wxCheckBox* m_checkBoxRadarEx;
		wxStaticText* m_staticText25;
		wxCheckBox* m_checkBoxNameInc;
		wxCheckBox* m_checkBoxNameEx;
		wxStaticText* m_staticText26;
		wxTextCtrl* m_textCtrlName;
		wxCheckBox* m_checkBoxTransInc;
		wxCheckBox* m_checkBoxTransEx;
		wxStaticText* m_staticText27;
		wxCheckBox* m_checkBoxCategoryInc;
		wxCheckBox* m_checkBoxCategoryEx;
		wxStaticText* m_staticText29;
		wxTextCtrl* m_textCtrlCategory;
		wxButton* m_buttonOk;
		wxButton* m_buttonCancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void ButtonAddClicked( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		AddSelectionCmdDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Add Selection Command"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~AddSelectionCmdDlg();
	
};

#endif //__AddSelectionPanelBase__
