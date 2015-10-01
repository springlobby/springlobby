/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "tab_simple.h"

#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/event.h>
#include <wx/defs.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include <wx/button.h>

#include "ctrlconstants.h"
#include "presets.h"
#include "frame.h"
#include "settings.h"
#include <lslunitsync/unitsync.h>
#include <utils/conversion.h>

const wxString infoTextContent = _("These options let you roughly control Spring's rendering.\n\
									For more speed try lowering the settings.\n\
									Full control over all settings is available in the\n\
									\"Expert mode\", either click on the button on the\n\
									right or use the \"Mode\" menu in the top menubar.\n\
									You can go back to this mode at any time by choosing\n\
									\"Simple mode\" from the \"Mode\" menu.\n\
									If you encounter error messages concerning graphics\n\
									when running Spring it might be necessary to disable\n \
									some options in expert mode.\n");

const wxString renderQuality_CBX_lbl = _("Graphics quality");
const wxString renderDetail_CBX_lbl = _("Graphics detail");
const wxString videoMode_CBX_lbl = _("Screen resolution");
const wxString button_lbl = _("Switch to expert mode");

void tab_simple::getSetUpResolutionCBX()
{
	int x_res = 0;
	int y_res = 0;

	if (abstract_panel::settingsChanged) {
		x_res = intSettings[RC_TEXT[0].key];
		y_res = intSettings[RC_TEXT[1].key];
	} else {
		try {
			x_res = LSL::usync().GetSpringConfigInt(STD_STRING(RC_TEXT[0].key), FromwxString(RC_TEXT[0].def));
			y_res = LSL::usync().GetSpringConfigInt(STD_STRING(RC_TEXT[1].key), FromwxString(RC_TEXT[1].def));
		} catch (...) {
		}
	}
	wxString currentRes;
	currentRes << x_res << _T(" x ") << y_res;
	wxArrayString choices(vl_Resolution_Str_size, vl_Resolution_Str);
	choices.Add((currentRes + _(" (current)")));

	videoMode_CBX = new wxComboBox(this, ID_SIMPLE_MODE_CBX, choices.Last(), wxDefaultPosition, wxSize(220, 21),
				       choices, wxCB_DROPDOWN | wxCB_READONLY);
}

void tab_simple::initOptSizer(wxFlexGridSizer* sizer)
{
	sizer->Add(new wxStaticText(this, -1, renderQuality_CBX_lbl), 0, wxTOP | wxBOTTOM, 15);

	renderQuality_CBX = new wxComboBox(this, ID_SIMPLE_QUAL_CBX, sett().getSimpleQuality(), wxDefaultPosition, wxSize(220, 21),
					   levels_vlow_To_vHigh_size, levels_vlow_To_vHigh, wxCB_DROPDOWN | wxCB_READONLY);
	renderQuality_CBX->SetToolTip(_("Sets all quality options to predefined values according to your choice."));
	sizer->Add(renderQuality_CBX, 0, wxTOP | wxBOTTOM, 15);

	sizer->Add(new wxStaticText(this, -1, renderDetail_CBX_lbl), 0, wxALL);
	renderDetail_CBX = new wxComboBox(this, ID_SIMPLE_DETAIL_CBX, sett().getSimpleDetail(), wxDefaultPosition, wxSize(220, 21),
					  levels_low_To_High_size, levels_low_To_High, wxCB_DROPDOWN | wxCB_READONLY);
	renderDetail_CBX->SetToolTip(_("Sets all detail options to predefined values according to your choice."));
	sizer->Add(renderDetail_CBX, 0, wxBOTTOM, 15);

	sizer->Add(new wxStaticText(this, -1, videoMode_CBX_lbl), 0, wxALL);
	getSetUpResolutionCBX();
	videoMode_CBX->SetToolTip(_("Select the resolution fitting your monitor(s).\n\
			Selecting a dual screen resolution will automatically enable dual screen mode.\n\
			If the appropriate resolution is not available you can set it manually in expert mode.\n\
			Please also contact the author so it can be added in future releases."));
	sizer->Add(videoMode_CBX, 0, wxBOTTOM, 15);

	sizer->Add(new wxStaticText(this, -1, (AO_SLI[1].lbl)), 0, wxTOP, 15);
	audioVolume_SLI = new wxSlider(this, AO_SLI[1].id, 0, 0, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	audioVolume_SLI->SetToolTip(AO_SLI[1].tTip[0]);
	sizer->Add(audioVolume_SLI, 0, wxBOTTOM, 15);
}

void tab_simple::initInfoSizer(wxFlexGridSizer* sizer)
{
	infoText = new wxStaticText(this, -1, infoTextContent);
	sizer->Add(infoText, 0, wxALL, 10);
}

void tab_simple::initButSizer(wxSizer* /*sizer*/)
{
	goExpert_BUT = new wxButton(this, ID_SIMPLE_GOEXPERT_BUT, button_lbl, wxPoint(-1, -1), wxSize(-1, -1), wxBU_EXACTFIT);
//	sizer->Add(goExpert_BUT, 0, wxEXPAND);
}

tab_simple::tab_simple(settings_frame* _origin, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : abstract_panel(parent, id, title, pos, size, style)
{

	origin = _origin;

	parentSizer = new wxFlexGridSizer(2, 1, 1);
	leftSizer = new wxFlexGridSizer(1, 15, 1);
	rightSizer = new wxFlexGridSizer(1, 15, 1);
	Sizer_CBX = new wxFlexGridSizer(2, 10, 10);
	Sizer_info = new wxFlexGridSizer(1, 15, 10);
	Sizer_BUT = new wxBoxSizer(wxVERTICAL);
	boxA = new wxStaticBoxSizer(wxVERTICAL, this, _("Options"));
	boxB = new wxStaticBoxSizer(wxVERTICAL, this, _("Info"));

	initOptSizer(Sizer_CBX);
	initInfoSizer(Sizer_info);
	initButSizer(Sizer_BUT);

	Sizer_CBX->Fit(this);
	Sizer_CBX->SetSizeHints(this);
	Sizer_info->Fit(this);
	Sizer_info->SetSizeHints(this);
	Sizer_BUT->Fit(this);
	Sizer_BUT->SetSizeHints(this);

	boxA->Add(Sizer_CBX, 1, wxEXPAND);
	boxB->Add(Sizer_info, 1, wxEXPAND);

	leftSizer->Add(boxA, 1, wxEXPAND);
	leftSizer->Add(boxB, 1, wxEXPAND);
	rightSizer->Add(Sizer_BUT, 1, wxALIGN_CENTER);

	parentSizer->Add(leftSizer, 1, wxALL | wxEXPAND, 15);
	parentSizer->Add(goExpert_BUT, 0, wxLEFT | wxALIGN_CENTER, 35);

	updateControls(UPDATE_ALL);
	SetSizer(parentSizer); // true --> delete old sizer if present
}

void tab_simple::updateControls(int /*what_to_update*/)
{
	audioVolume_SLI->SetValue(intSettings[AO_SLI[1].key]);
}

tab_simple::~tab_simple(void)
{
}

void tab_simple::OnComboBoxChange(wxCommandEvent& event)
{
	abstract_panel::OnComboBoxChange(event);
}

void tab_simple::OnButtonClick(wxCommandEvent&)
{
	origin->switchToExpertMode();
}

void tab_simple::saveCbxChoices()
{
	sett().setSimpleDetail(renderDetail_CBX->GetValue());
	sett().setSimpleRes(videoMode_CBX->GetValue());
	sett().setSimpleQuality(renderQuality_CBX->GetValue());
}

BEGIN_EVENT_TABLE(tab_simple, abstract_panel)
EVT_SLIDER(wxID_ANY, tab_simple::OnSliderMove)
//	EVT_TEXT(wxID_ANY,              tab_simple::OnTextUpdate)
EVT_CHECKBOX(wxID_ANY, tab_simple::OnCheckBoxTick)
//	EVT_RADIOBUTTON(wxID_ANY,       tab_simple::OnRadioButtonToggle)
//	EVT_IDLE(                       tab_simple::update)
EVT_COMBOBOX(wxID_ANY, tab_simple::OnComboBoxChange)
EVT_BUTTON(ID_SIMPLE_GOEXPERT_BUT, tab_simple::OnButtonClick)
END_EVENT_TABLE()
