/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "tab_quality_video.h"

#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/defs.h>
#include <wx/event.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>

#include "ctrlconstants.h"
#include "utils/conversion.h"

void tab_quality_video::initVideoSizer(wxFlexGridSizer* sizer)
{


	// i < "sizeof"(VO_CBOX)
	for (int i = 0; i < ctrl_vo_Boxes_size; i++) {
		ctrl_vo_Boxes[i] = new wxCheckBox(this, VO_CBOX[i].id, (VO_CBOX[i].lbl));
		ctrl_vo_Boxes[i]->SetToolTip(VO_CBOX[i].tTip[0]);
		sizer->Add(ctrl_vo_Boxes[i], 0, wxTOP, (i == 0) ? 5 : 0);
	}

	ctrl_x_res = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_X, wxEmptyString, WX_DEF_P, wxSize(60, 20), 0);
	ctrl_y_res = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_Y, wxEmptyString, WX_DEF_P, wxSize(60, 20), 0);
	ctrl_x_res->SetToolTip(RC_TEXT[0].tTip[0]);
	ctrl_y_res->SetToolTip(RC_TEXT[1].tTip[0]);


	wxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

	subSizer->Add(ctrl_x_res, 0, wxALIGN_LEFT, 10);
	subSizer->Add(5, 0, 0);
	subSizer->Add(new wxStaticText(this, -1, wxT("x")), 0, wxTOP | wxBOTTOM, 0);
	subSizer->Add(5, 0, 0);
	subSizer->Add(ctrl_y_res, 0, wxALIGN_LEFT, 10);

	wxSizer* subSizer2 = new wxBoxSizer(wxVERTICAL);
	subSizer2->Add(new wxStaticText(this, -1, _("Screen Resolution")), 1, wxTOP | wxEXPAND, 10);
	subSizer2->Add(subSizer);
	sizer->Add(subSizer2);
	sizer->Add(0, 5, 0);
}

void tab_quality_video::updateControls(int what_to_update)
{
	if (what_to_update == UPDATE_ALL) {
		//the rest
		for (int i = 5; i < 7; i++) {
			ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
		}

		for (int i = 0; i < ctrl_vo_Boxes_size; i++) {
			ctrl_vo_Boxes[i]->SetValue(intSettings[VO_CBOX[i].key]);
		}
	}

	if (what_to_update == UPDATE_VIDEO_MODE || what_to_update == UPDATE_ALL) {
		ctrl_x_res->SetValue(wxString::Format(_T("%d"), intSettings[RC_TEXT[0].key]));
		ctrl_y_res->SetValue(wxString::Format(_T("%d"), intSettings[RC_TEXT[1].key]));
	}
	if (what_to_update == UPDATE_QA_BOXES || what_to_update == UPDATE_ALL) {
		//option 7-9 are not on presets
		//skip 0 == shadows
		for (int i = 1; i < 9; i++) {
			ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
		}
		for (int i = 10; i < ctrl_qa_Boxes_size; i++) {
			ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
		}

		ctrl_shadows_CBox->SetValue(SHADOW_CHOICES[intSettings[QA_CBOX[0].key] + 1]);

		m_enable_w4 = false;
		int waterOptIndex = 0;
		const int waterSetting = intSettings[WR_COMBOX[0].key];
		switch (waterSetting) {
			case 0:
			case 1:
				waterOptIndex = waterSetting;
				break;
			case 2:
				waterOptIndex = 3;
				break;
			case 3:
				waterOptIndex = 2;
				break;
			case 4:
				waterOptIndex = 4;
				m_enable_w4 = true;
				break;
		}
		ctrl_waterQ_CBox->SetValue(WR_COMBOX_CHOICES[waterOptIndex]);

		int useFSAA = intSettings[VO_SLI_EXT[0].key];
		int FSAALev = intSettings[VO_SLI[0].key];
		ctrl_fsaa_slider->SetValue((useFSAA == 1) ? FSAALev : 0);

		switch (intSettings[VO_RBUT[0].key]) {
			case 16: {
				ctrl_z_radio1->SetValue(1);
			} break;
			case 24: {
				ctrl_z_radio2->SetValue(1);
			} break;
		}
	}
	if (what_to_update == UPDATE_W4_CONTROLS || what_to_update == UPDATE_ALL) {
		for (unsigned int i = 0; i < m_w4_controls.size(); ++i) {
			wxControl* tmp = m_w4_controls[i];
			tmp->Enable(m_enable_w4);
			switch (i) {
				case 0:
				case 1:
				case 2:
				case 3:
					((wxCheckBox*)tmp)->SetValue(intSettings[W4_CONTROLS[i].key]);
					break;
				case 4: {
					int val = intSettings[W4_CONTROLS[i].key];
					for (unsigned int j = 0; j < sizeof(W4_TEXSIZE_CHOICES) / sizeof(W4_TEXSIZE_CHOICES[0]); ++j) {
						if (W4_TEXSIZE_CHOICES[j] == TowxString(val))
							((wxComboBox*)tmp)->SetValue(W4_TEXSIZE_CHOICES[j]);
					}
				} break;
				case 5:
					((wxComboBox*)tmp)->SetValue(W4_REFRACTION_CHOICES[intSettings[W4_CONTROLS[i].key]]);
					break;
				case 6:
					//doesn't work if catsed from wxControl like the others
					double tmpval = double(floatSettings[W4_CONTROLS[i].key]);
					m_aniso_spin->SetValue(tmpval);
					break;
			}
		}
	}
}

void tab_quality_video::initQualitySizer(wxFlexGridSizer* sizer)
{
	sizer->Add(new wxStaticText(this, -1, _("If an option needs special hardware to work\n\
												it will be mentioned in the tooltip.")),
		   1, wxTOP | wxEXPAND, 10);

	// i < 8 with High resolution LOS textures
	// i < 7 without
	//skip first == shadows
	for (int i = 1; i < ctrl_qa_Boxes_size - 3; i++) {
		ctrl_qa_Boxes[i] = new wxCheckBox(this, QA_CBOX[i].id, (QA_CBOX[i].lbl));
		//ctrl_qa_Boxes[i]->SetValue(configHandler.GetSpringConfigInt(QA_CBOX[i].key,fromString(QA_CBOX[i].def)));
		ctrl_qa_Boxes[i]->SetToolTip(QA_CBOX[i].tTip[0]);
		sizer->Add(ctrl_qa_Boxes[i], 0, wxTOP, (i == 0) ? 5 : 0);
	}

	wxSizer* subSizer_shadows = new wxBoxSizer(wxVERTICAL);
	subSizer_shadows->Add(new wxStaticText(this, -1, _("Shadows (slow)")), 0, wxTOP | wxEXPAND, 10);
	ctrl_shadows_CBox = new wxComboBox(this, ID_SHADOW_CHOICES, SHADOW_CHOICES[0], wxDefaultPosition, wxSize(220, 21),
					   3, SHADOW_CHOICES, wxCB_DROPDOWN | wxCB_READONLY);
	ctrl_shadows_CBox->SetToolTip(QA_CBOX[0].tTip[0]);
	subSizer_shadows->Add(ctrl_shadows_CBox, 0, wxBOTTOM, 5);
	sizer->Add(subSizer_shadows, 0, wxBOTTOM, 5);

	wxSizer* subSizer = new wxBoxSizer(wxVERTICAL);
	subSizer->Add(new wxStaticText(this, -1, _("Water Quality")), 0, wxTOP | wxEXPAND, 10);
	ctrl_waterQ_CBox = new wxComboBox(this, ID_WINDOWP_WR_COMBOX, WR_COMBOX_CHOICES[0], wxDefaultPosition, wxSize(220, 21),
					  5, WR_COMBOX_CHOICES, wxCB_DROPDOWN | wxCB_READONLY);
	ctrl_waterQ_CBox->SetToolTip(WR_COMBOX[0].tTip[0]);
	subSizer->Add(ctrl_waterQ_CBox, 0, wxBOTTOM, 5);
	sizer->Add(subSizer, 0, wxBOTTOM, 5);
}

void tab_quality_video::initAASizer(wxFlexGridSizer* sizer)
{
	for (int i = 10; i < ctrl_qa_Boxes_size; i++) {
		ctrl_qa_Boxes[i] = new wxCheckBox(this, QA_CBOX[i].id, (QA_CBOX[i].lbl));
		//ctrl_qa_Boxes[i]->SetValue(configHandler.GetSpringConfigInt(QA_CBOX[i].key,fromString(QA_CBOX[i].def)));
		ctrl_qa_Boxes[i]->SetToolTip(QA_CBOX[i].tTip[0]);
		sizer->Add(ctrl_qa_Boxes[i], 0, wxTOP, (i == 10) ? 5 : 0);
	}
	wxSizer* subsizer = new wxBoxSizer(wxVERTICAL);

	ctrl_fsaa_slider = new wxSlider(this, VO_SLI[0].id, 0, 0, 16, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	ctrl_fsaa_slider->SetToolTip(VO_SLI[0].tTip[0]);
	subsizer->Add(new wxStaticText(this, -1, (VO_SLI[0].lbl)), 0, wxTOP | wxEXPAND, 10);
	subsizer->Add(ctrl_fsaa_slider, 0, wxALIGN_LEFT | wxBOTTOM | wxEXPAND, 5);
	sizer->Add(subsizer);
}

void tab_quality_video::initZBufferSizer(wxFlexGridSizer* sizer)
{
	//z-buffer

	sizer->Add(new wxStaticText(this, -1, _("Resolution in bit")), 0, wxTOP, 15);

	ctrl_z_radio1 = new wxRadioButton(this, VO_RBUT[0].id, (VO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	ctrl_z_radio2 = new wxRadioButton(this, VO_RBUT[1].id, (VO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	ctrl_z_radio1->SetToolTip(VO_RBUT[0].tTip[0]);
	ctrl_z_radio2->SetToolTip(VO_RBUT[1].tTip[0]);

	sizer->Add(ctrl_z_radio1, 0, wxTOP, 0);
	sizer->Add(ctrl_z_radio2, 0, wxBOTTOM, 10);
}

void tab_quality_video::initW4Sizer(wxSizer* sizer)
{
	for (int i = 0; i < 4; ++i) {
		wxCheckBox* blurChk = new wxCheckBox(this, W4_CONTROLS[i].id, W4_CONTROLS[i].lbl);
		m_w4_controls.push_back((wxControl*)blurChk);
		blurChk->SetToolTip(W4_CONTROLS[i].tTip[0]);
		sizer->Add(blurChk, 0, wxEXPAND | wxALL, 4);
	}

	sizer->Add(new wxStaticText(this, -1, (W4_CONTROLS[4].lbl)), 0, wxTOP | wxEXPAND, 5);
	wxComboBox* texsizeCom = new wxComboBox(this, W4_CONTROLS[4].id, W4_TEXSIZE_CHOICES[0], wxDefaultPosition, wxSize(220, 21),
						4, W4_TEXSIZE_CHOICES, wxCB_DROPDOWN | wxCB_READONLY);
	texsizeCom->SetToolTip(W4_CONTROLS[4].tTip[0]);
	m_w4_controls.push_back((wxControl*)texsizeCom);
	sizer->Add(texsizeCom, 0, wxEXPAND | wxALL, 4);

	sizer->Add(new wxStaticText(this, -1, (W4_CONTROLS[5].lbl)), 0, wxTOP | wxEXPAND, 5);
	wxComboBox* refractionCom = new wxComboBox(this, W4_CONTROLS[5].id, W4_REFRACTION_CHOICES[0], wxDefaultPosition, wxSize(220, 21),
						   3, W4_REFRACTION_CHOICES, wxCB_DROPDOWN | wxCB_READONLY);
	refractionCom->SetToolTip(W4_CONTROLS[5].tTip[0]);
	m_w4_controls.push_back((wxControl*)refractionCom);
	sizer->Add(refractionCom, 0, wxEXPAND | wxALL, 4);

	sizer->Add(new wxStaticText(this, -1, (W4_CONTROLS[6].lbl)), 0, wxTOP | wxEXPAND, 5);
	m_aniso_spin = new wxSpinCtrlDouble(this, W4_CONTROLS[6].id, wxEmptyString,
					    wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.f, 6.f,
					    0.f, 0.25f, wxEmptyString);
	m_aniso_spin->SetToolTip(W4_CONTROLS[6].tTip[0]);
	m_w4_controls.push_back((wxControl*)m_aniso_spin);
	sizer->Add(m_aniso_spin, 0, wxEXPAND | wxALL, 4);
}


tab_quality_video::tab_quality_video(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
				     const wxSize& size, long style)
    : abstract_panel(parent, id, title, pos, size, style)
{
	ctrl_qa_Boxes = new wxCheckBox* [ctrl_qa_Boxes_size];
	ctrl_vo_Boxes = new wxCheckBox* [ctrl_vo_Boxes_size];
	parentSizer = new wxFlexGridSizer(3, 0, 0);
	leftSizer = new wxFlexGridSizer(1, 15, 0);
	middleSizer = new wxFlexGridSizer(1, 15, 0);
	rightSizer = new wxFlexGridSizer(1, 15, 0); //for info
	SizerA = new wxFlexGridSizer(1, 15, 10);
	SizerB = new wxFlexGridSizer(1, 15, 10);
	SizerC = new wxFlexGridSizer(1, 15, 10);
	SizerD = new wxFlexGridSizer(1, 5, 10);
	SizerE = new wxBoxSizer(wxVERTICAL);
	boxA = new wxStaticBoxSizer(wxVERTICAL, this, _("Render Quality Options"));
	boxB = new wxStaticBoxSizer(wxVERTICAL, this, _("Video Mode Options"));
	boxC = new wxStaticBoxSizer(wxVERTICAL, this, _("Anti-Aliasing Options"));
	boxD = new wxStaticBoxSizer(wxVERTICAL, this, _("Z-/Depth-Buffer"));
	boxE = new wxStaticBoxSizer(wxVERTICAL, this, _("Bump-mapped Water"));
	SizerA->AddGrowableCol(0);
	SizerB->AddGrowableCol(0);
	SizerC->AddGrowableCol(0);
	SizerD->AddGrowableCol(0);

	initVideoSizer(SizerB);
	initZBufferSizer(SizerD);
	initQualitySizer(SizerA);
	initAASizer(SizerC);
	initW4Sizer(SizerE);

	SizerA->Fit(this);
	SizerA->SetSizeHints(this);
	SizerB->Fit(this);
	SizerB->SetSizeHints(this);
	SizerC->Fit(this);
	SizerC->SetSizeHints(this);
	SizerD->Fit(this);
	SizerD->SetSizeHints(this);
	SizerE->Fit(this);
	SizerE->SetSizeHints(this);

	boxA->Add(SizerA, 1, wxEXPAND);
	boxB->Add(SizerB, 1, wxEXPAND);
	boxC->Add(SizerC, 1, wxEXPAND);
	boxD->Add(SizerD, 1, wxEXPAND);
	boxE->Add(SizerE, 1, wxEXPAND);
	leftSizer->Add(boxB, 1, wxEXPAND | wxALL, 5);
	rightSizer->Add(boxC, 1, wxEXPAND | wxALL, 5);
	rightSizer->Add(boxE, 1, wxEXPAND | wxALL, 5);
	middleSizer->Add(boxA, 1, wxEXPAND | wxALL, 5);
	leftSizer->Add(boxD, 1, wxEXPAND | wxALL, 5);
	parentSizer->Add(leftSizer, 0, wxALL | wxEXPAND, 10);
	parentSizer->Add(middleSizer, 0, wxALL | wxEXPAND, 10);
	parentSizer->Add(rightSizer, 0, wxALL | wxEXPAND, 10);

	updateControls(UPDATE_ALL);

	SetSizer(parentSizer, true); // true --> delete old sizer if present
}

tab_quality_video::~tab_quality_video(void)
{
}

void tab_quality_video::OnComboBoxChange(wxCommandEvent& event)
{
	if (event.GetId() == ID_WINDOWP_WR_COMBOX) {
		wxComboBox* comboBox = (wxComboBox*)event.GetEventObject();
		const wxString choice = comboBox->GetValue();
		m_enable_w4 = (choice == WR_COMBOX_CHOICES[4]);
		updateControls(UPDATE_W4_CONTROLS);
	}

	abstract_panel::OnComboBoxChange(event);
}

//! this just forwards the direct event receivier from the control to base class
void tab_quality_video::OnSpinCtrlDoubleChange(wxSpinDoubleEvent& event)
{
	abstract_panel::OnSpinCtrlDoubleChange(event);
}

BEGIN_EVENT_TABLE(tab_quality_video, abstract_panel)
EVT_SLIDER(wxID_ANY, tab_quality_video::OnSliderMove)
EVT_TEXT(ID_RES_CHOICES_LBOX_X, tab_quality_video::OnTextUpdate)
EVT_TEXT(ID_RES_CHOICES_LBOX_Y, tab_quality_video::OnTextUpdate)
EVT_CHECKBOX(wxID_ANY, tab_quality_video::OnCheckBoxTick)
EVT_SPINCTRL(wxID_ANY, tab_quality_video::OnSpinControlChange)
EVT_RADIOBUTTON(wxID_ANY, tab_quality_video::OnRadioButtonToggle)
//EVT_IDLE(                       tab_quality_video::update)
EVT_COMBOBOX(wxID_ANY, tab_quality_video::OnComboBoxChange)
END_EVENT_TABLE()
