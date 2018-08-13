/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __TAB_SIMPLE_h__
#define __TAB_SIMPLE_h__

#include "tab_abstract.h"
class wxComboBox;
class wxSlider;
class wxFlexGridSizer;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxStaticText;
class wxStaticBoxSizer;
class wxButton;
class settings_frame;

class tab_simple : public abstract_panel
{

public:
	tab_simple(settings_frame* _origin, wxWindow* parent, wxWindowID id = 1, const wxString& title = wxT("Project2"),
		   const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~tab_simple();


	void updateControls(int);
	void saveCbxChoices();
	void setTabs(abstract_panel*, abstract_panel*);
	void OnComboBoxChange(wxCommandEvent& event);

private:
	void getSetUpResolutionCBX();
	void OnClose(wxCloseEvent& event);
	void OnButtonClick(wxCommandEvent& event);
	void CreateGUIControls();
	void initOptSizer(wxFlexGridSizer*);
	void initInfoSizer(wxFlexGridSizer*);
	void initButSizer(wxSizer*);
	wxComboBox* renderQuality_CBX;
	wxComboBox* renderDetail_CBX;
	wxComboBox* videoMode_CBX;
	wxSlider* audioVolume_SLI;
	wxButton* goExpert_BUT;
	wxStaticText* infoText;

	//dirty
	abstract_panel* detailTab;
	abstract_panel* qualityTab;

	wxSizer* parentSizer;

	wxFlexGridSizer* leftSizer;
	wxFlexGridSizer* rightSizer;
	wxFlexGridSizer* Sizer_CBX;
	wxFlexGridSizer* Sizer_info;
	wxSizer* Sizer_BUT;

	wxStaticBoxSizer* boxA;
	wxStaticBoxSizer* boxB;
	settings_frame* origin;

	DECLARE_EVENT_TABLE()
};

#endif
