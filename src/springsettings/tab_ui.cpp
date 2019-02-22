/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "tab_ui.h"

#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "ctrlconstants.h"
#include "log.h"

//TODO maybe use only one chkbox for minimap on left
void tab_ui::initScrollSpeedSizer(wxStaticBoxSizer* sizer)
{
	// i < "sizeof"(MO_SLI)
	sizer->Add(5, 10, 0);
	sizer->Add(new wxStaticText(this, -1, _("Setting a slider to 0 will exclude that\n\
											mode from being cycled through ingame.")),
		   0, wxBOTTOM, 15);
	for (int i = 0; i < ctrl_scroll_slider_size; i++) {
		//set to dummy value
		ctrl_scroll_slider[i] = new wxSlider(this, MO_SLI[i].id, 0, 0, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
		ctrl_scroll_slider[i]->SetToolTip(MO_SLI[i].tTip[0]);
		if (i > 0)
			sizer->Add(5, 23, 0);
		sizer->Add(new wxStaticText(this, wxID_ANY, (MO_SLI[i].lbl), wxDefaultPosition, wxDefaultSize, 10), 1, wxEXPAND);
		sizer->Add(ctrl_scroll_slider[i], 0, wxTOP, 0);
	}
	sizer->Add(5, 10, 0);
}

void tab_ui::initCameraSizer(wxStaticBoxSizer* sizer)
{
	ctrl_cam_radio0 = new wxRadioButton(this, MO_RBUT[0].id, (MO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	ctrl_cam_radio1 = new wxRadioButton(this, MO_RBUT[1].id, (MO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	ctrl_cam_radio2 = new wxRadioButton(this, MO_RBUT[2].id, (MO_RBUT[2].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	ctrl_cam_radio3 = new wxRadioButton(this, MO_RBUT[3].id, (MO_RBUT[3].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	ctrl_cam_radio4 = new wxRadioButton(this, MO_RBUT[4].id, (MO_RBUT[4].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	ctrl_cam_radio0->SetToolTip(MO_RBUT[0].tTip[0]);
	ctrl_cam_radio1->SetToolTip(MO_RBUT[1].tTip[0]);
	ctrl_cam_radio2->SetToolTip(MO_RBUT[2].tTip[0]);
	ctrl_cam_radio3->SetToolTip(MO_RBUT[3].tTip[0]);
	ctrl_cam_radio4->SetToolTip(MO_RBUT[4].tTip[0]);


	sizer->Add(ctrl_cam_radio0, 0, wxTOP, 10);
	sizer->Add(ctrl_cam_radio1, 0, wxTOP, 5);
	sizer->Add(ctrl_cam_radio2, 0, wxTOP, 5);
	sizer->Add(ctrl_cam_radio3, 0, wxTOP, 5);
	sizer->Add(ctrl_cam_radio4, 0, wxTOP | wxBOTTOM, 5);
}

void tab_ui::initUiOptSizer(wxStaticBoxSizer* sizer)
{
	wxBoxSizer* subSizer = new wxBoxSizer(wxVERTICAL);
	for (int i = 0; i < ctrl_ui_chkb_size; i++) {
		ctrl_ui_chkb[i] = new wxCheckBox(this, UI_CBOX[i].id, (UI_CBOX[i].lbl));
		subSizer->Add(ctrl_ui_chkb[i], 0, wxTOP, 5);
		ctrl_ui_chkb[i]->SetToolTip(UI_CBOX[i].tTip[0]);
	}
	sizer->Add(subSizer);
	sizer->Add(0, 5, 0);
}

void tab_ui::initZoomSizer(wxStaticBoxSizer* sizer)
{
	wxBoxSizer* subSizer = new wxBoxSizer(wxVERTICAL);
	ctrl_zoom_spin = new wxSpinCtrl(this, UI_ZOOM[0].id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -255, 255);
	ctrl_zoom_spin->SetToolTip(UI_ZOOM[0].tTip[0]);
	subSizer->Add(new wxStaticText(this, -1, UI_ZOOM[0].lbl), 0, wxTOP, 10);
	subSizer->Add(ctrl_zoom_spin, 0, wxALIGN_RIGHT | wxALL, 5);
	sizer->Add(subSizer, 5, wxALL);
	sizer->Add(0, 5, 0);
}

void tab_ui::updateControls(int /*what_to_update*/)
{
	for (int i = 0; i < ctrl_ui_chkb_size; i++) {
		ctrl_ui_chkb[i]->SetValue(intSettings[UI_CBOX[i].key]);
	}

	const int value = intSettings.at(MO_RBUT[0].key);
	switch (value) {
		case 0: {
			ctrl_cam_radio3->SetValue(1);
		} break; // CamMode 0: FPS
		case 1: {
			ctrl_cam_radio0->SetValue(1);
		} break; // CamMode 1: OH
		case 3: {
			ctrl_cam_radio1->SetValue(1);
		} break; // CamMode 2: ROH
		case 2: {
			ctrl_cam_radio2->SetValue(1);
		} break; // CamMode 3: TW
		case 4: {
			ctrl_cam_radio4->SetValue(1);
		} break; // CamMode 4: FC
		default:
			wxLogWarning(_T("Unhandled switch case: %d"), value);
			break;
	}

	for (int i = 0; i < ctrl_scroll_slider_size; i++) {
		ctrl_scroll_slider[i]->SetValue(intSettings[MO_SLI[i].key]);
	}

	ctrl_zoom_spin->SetValue(intSettings[UI_ZOOM[0].key]);
}

tab_ui::tab_ui(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : abstract_panel(parent, id, title, pos, size, style)
{
	ctrl_scroll_slider = new wxSlider*[ctrl_scroll_slider_size];
	ctrl_ui_chkb = new wxCheckBox*[ctrl_ui_chkb_size];
	pSizer = new wxFlexGridSizer(3, 15, 15);
	cSizerL = new wxFlexGridSizer(1, 10, 10);
	cSizerR = new wxFlexGridSizer(1, 10, 10);
	cSizerM = new wxFlexGridSizer(1, 10, 10);

	scrollSpeedSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Scroll Speeds (mouse + keyboard)"),
								WX_DEF_P, wxSize(-1, -1), 0, wxEmptyString),
						wxVERTICAL);


	cameraSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Default Camera Mode"),
							   WX_DEF_P, wxSize(-1, -1), 0, wxEmptyString),
					   wxVERTICAL);
	uiOptSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Misc. UI Options"),
							  WX_DEF_P, wxSize(-1, -1), 0, wxEmptyString),
					  wxVERTICAL);
	zoomSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Zoom"),
							 WX_DEF_P, wxSize(-1, -1), 0, wxEmptyString),
					 wxVERTICAL);

	initScrollSpeedSizer(scrollSpeedSizer);
	initUiOptSizer(uiOptSizer);
	initCameraSizer(cameraSizer);
	initZoomSizer(zoomSizer);


	cSizerM->Add(uiOptSizer, 0, wxALL, 5);
	cSizerL->Add(scrollSpeedSizer, 0, wxALL, 5);
	cSizerR->Add(cameraSizer, 0, wxALL, 5);
	cSizerR->Add(zoomSizer, 1, wxALL | wxEXPAND, 5);

	cSizerL->Fit(this);
	cSizerL->SetSizeHints(this);
	cSizerM->Fit(this);
	cSizerM->SetSizeHints(this);
	cSizerR->Fit(this);
	cSizerR->SetSizeHints(this);


	pSizer->Add(cSizerL, 0, wxALL | wxEXPAND, 10);
	pSizer->Add(cSizerM, 0, wxALL | wxEXPAND, 10);
	pSizer->Add(cSizerR, 0, wxALL | wxEXPAND, 10);

	updateControls(UPDATE_ALL);
	SetSizer(pSizer); // true --> delete old sizer if present
}

tab_ui::~tab_ui(void)
{
}

BEGIN_EVENT_TABLE(tab_ui, abstract_panel)
EVT_SLIDER(wxID_ANY, tab_ui::OnSliderMove)
EVT_TEXT(wxID_ANY, tab_ui::OnTextUpdate)
EVT_CHECKBOX(wxID_ANY, tab_ui::OnCheckBoxTick)
EVT_RADIOBUTTON(wxID_ANY, tab_ui::OnRadioButtonToggle)
//	EVT_IDLE(                       tab_ui::update)
END_EVENT_TABLE()
