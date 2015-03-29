/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __AddSelectionPanelDlg__
#define __AddSelectionPanelDlg__

#include "HotkeyForms.h"


class AddSelectionCmdDlg : public AddSelectionCmdDlgBase
{
public:
	AddSelectionCmdDlg(wxWindow* parent);

	wxString getCommandString() const;

protected:
	wxString getSourceString() const;
	wxString getConditionsAndSelectString() const;
	wxString validateInputFields();
	static void controlCheckBoxEnable(wxCheckBox* pCheckbClicked, wxCheckBox* pCheckbSibling, wxTextCtrl* pTextCtrl);

	virtual void OnCheckBoxCondBuilderIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondBuilderExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondBuildingIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondBuildingExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondComIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondComExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondTypeSelIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondTypeSelExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondAirIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondAirExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondWeaponsIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondWeaponsExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondIdleIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondIdleExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondRadarIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondRadarExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondTransIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondTransExClick(wxCommandEvent& event);

	virtual void OnCheckBoxCondHotkeyIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondHotkeyExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondWaitingIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondWaitingExClick(wxCommandEvent& event);

	virtual void OnCheckBoxCondRelHealthIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondRelHealthExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondWeapRangeIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondWeapRangeExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondAbsHealthIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondAbsHealthExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondNameIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondNameExClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondCategoryIncClick(wxCommandEvent& event);
	virtual void OnCheckBoxCondCategoryExClick(wxCommandEvent& event);
	virtual void OnRadioBtnSrcMapClick(wxCommandEvent& event);
	virtual void OnRadioBtnSrcViewClick(wxCommandEvent& event);
	virtual void OnRadioBtnSrcSelClick(wxCommandEvent& event);
	virtual void OnRadioBtnSrcDistClick(wxCommandEvent& event);
	virtual void OnRadioBtnSelAllClick(wxCommandEvent& event);
	virtual void OnRadioBtnSelCountClick(wxCommandEvent& event);
	virtual void OnRadioBtnSelOneClick(wxCommandEvent& event);
	virtual void OnRadioBtnSelPercClick(wxCommandEvent& event);
	virtual void OnButtonAddClick(wxCommandEvent& event);
};

#endif //__AddSelectionPanelBase__
