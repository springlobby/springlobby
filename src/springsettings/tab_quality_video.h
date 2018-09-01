/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __TAB_QUALITY_VIDEO_h__
#define __TAB_QUALITY_VIDEO_h__

#include <vector>
#include "tab_abstract.h"
class wxComboBox;
class wxCheckBox;
class wxBoxSizer;
class wxTextCtrl;
class wxFlexGridSizer;
class wxRadioButton;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxSlider;
class wxStaticBoxSizer;
class wxSpinCtrlDouble;
class wxSpinDoubleEvent;

class tab_quality_video : public abstract_panel
{

public:
	tab_quality_video(wxWindow* parent, wxWindowID id, const wxString& title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~tab_quality_video();

	void initVideoSizer(wxFlexGridSizer*);
	void initQualitySizer(wxFlexGridSizer*);
	void initAASizer(wxFlexGridSizer*);
	void initZBufferSizer(wxFlexGridSizer*);
	void initW4Sizer(wxSizer*);
	void updateControls(int);
	virtual void OnComboBoxChange(wxCommandEvent& event);
	void OnSpinCtrlDoubleChange(wxSpinDoubleEvent& event);

private:
	void OnClose(wxCloseEvent& event);
	void CreateGUIControls();

	wxComboBox* ctrl_waterQ_CBox;
	wxComboBox* ctrl_shadows_CBox;
	wxCheckBox** ctrl_vo_Boxes;
	static const int ctrl_vo_Boxes_size = 3;
	wxTextCtrl* ctrl_x_res;
	wxTextCtrl* ctrl_y_res;
	wxCheckBox** ctrl_qa_Boxes;
	static const int ctrl_qa_Boxes_size = 12;
	wxSlider* ctrl_fsaa_slider;

	wxRadioButton* ctrl_z_radio1;
	wxRadioButton* ctrl_z_radio2;

	wxSizer* parentSizer;
	wxSizer* leftSizer;
	wxSizer* middleSizer;
	wxSizer* rightSizer;
	wxFlexGridSizer* SizerA;
	wxFlexGridSizer* SizerB;
	wxFlexGridSizer* SizerC;
	wxFlexGridSizer* SizerD;
	wxBoxSizer* SizerE;
	wxStaticBoxSizer* boxA;
	wxStaticBoxSizer* boxB;
	wxStaticBoxSizer* boxC;
	wxStaticBoxSizer* boxD;
	wxStaticBoxSizer* boxE;

	bool m_enable_w4;

	std::vector<wxControl*> m_w4_controls;

	wxSpinCtrlDouble* m_aniso_spin;

	DECLARE_EVENT_TABLE()
};

#endif
