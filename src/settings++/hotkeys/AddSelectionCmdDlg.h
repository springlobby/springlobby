///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __AddSelectionPanelDlg__
#define __AddSelectionPanelDlg__

#include "AddSelectionCmdDlgBase.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AddSelectionCmdDlg
///////////////////////////////////////////////////////////////////////////////
class AddSelectionCmdDlg : public AddSelectionCmdDlgBase 
{
public:
	AddSelectionCmdDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Add Selection Command"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
	
	wxString getCommandString() const;

protected:
	wxString getSourceString() const;
	wxString getConditionsAndSelectString() const;
	wxString validateInputFields();

	virtual void OnRadioBtnSrcMapClick( wxCommandEvent& event );
	virtual void OnRadioBtnSrcViewClick( wxCommandEvent& event );
	virtual void OnRadioBtnSrcSelClick( wxCommandEvent& event );
	virtual void OnRadioBtnSrcDistClick( wxCommandEvent& event );
	virtual void OnRadioBtnSelAllClick( wxCommandEvent& event );
	virtual void OnRadioBtnSelCountClick( wxCommandEvent& event );
	virtual void OnRadioBtnSelOneClick( wxCommandEvent& event );
	virtual void OnRadioBtnSelPercClick( wxCommandEvent& event );
	virtual void OnButtonAddClick( wxCommandEvent& event );
};

#endif //__AddSelectionPanelBase__
