/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "tab_abstract.h"

#include <lslunitsync/unitsync.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/defs.h>
#include <wx/display.h>
#include <wx/event.h>
#include <wx/gbsizer.h>
#include <wx/log.h>
#include <wx/radiobut.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/string.h>
#include <wx/textctrl.h>

#include "ctrlconstants.h"
#include "gui/customdialogs.h"
#include "log.h"
#include "presets.h"
#include "settings.h"
#include "utils/conversion.h"

intMap abstract_panel::intSettings;
stringMap abstract_panel::stringSettings;
floatMap abstract_panel::floatSettings;

bool abstract_panel::settingsChanged = false;
//const category_sizes_map s_category_sizes ( entries_ , entries_ + sizeof(entries_[0]) );

const Control intControls[] = {
    // RO_SLI[8]
    RO_SLI[0], RO_SLI[1], RO_SLI[2], RO_SLI[3], RO_SLI[4], RO_SLI[5], RO_SLI[6], RO_SLI[7], RO_SLI[8],
    // VO_CBOX[3]
    VO_CBOX[0], VO_CBOX[1], VO_CBOX[2],
    //VO_RBUT[2] <- only one key
    VO_RBUT[0],
    // VO_SLI[1]
    VO_SLI[0],
    //VO_SLI_EXT[1]
    VO_SLI_EXT[0],
    // AO_SLI[3]
    AO_SLI[0], AO_SLI[1], AO_SLI[2], AO_SLI[3], AO_SLI[4], AO_SLI[5],
    // QA_CBOX[10]
    QA_CBOX[0], QA_CBOX[1], QA_CBOX[2], QA_CBOX[3], QA_CBOX[4], QA_CBOX[5], QA_CBOX[6], QA_CBOX[7], QA_CBOX[8],
    QA_CBOX[9], QA_CBOX[10], QA_CBOX[11],
    //UI_CBOX[16]
    UI_CBOX[0], UI_CBOX[1], UI_CBOX[2], UI_CBOX[3], UI_CBOX[4], UI_CBOX[5], UI_CBOX[6], UI_CBOX[7], UI_CBOX[8],
    UI_CBOX[9], UI_CBOX[10], UI_CBOX[11], UI_CBOX[12], UI_CBOX[13], UI_CBOX[14], UI_CBOX[15], UI_CBOX[16],
    //MO_SLI[5]
    MO_SLI[0], MO_SLI[1], MO_SLI[2], MO_SLI[3], MO_SLI[4],
    //MO_SLI_EXT[5]
    MO_SLI_EXT[0], MO_SLI_EXT[1], MO_SLI_EXT[2], MO_SLI_EXT[3], MO_SLI_EXT[4],
    //DO_SLI[1]
    DO_SLI[0],
    //DO_CBOX[1]
    DO_CBOX[0],
    //WR_COMBOX[1]
    WR_COMBOX[0],
    //MO_CBOX[2]
    MO_CBOX[0], MO_CBOX[1],
    //MO_RBUT[5] <- only first
    MO_RBUT[0],
    //RC_TEXT[2]
    RC_TEXT[0], RC_TEXT[1],
    //UI_ZOOM[1]
    UI_ZOOM[0],
    //W4_CONTROLS[7]
    W4_CONTROLS[0], W4_CONTROLS[1], W4_CONTROLS[2], W4_CONTROLS[3],
    W4_CONTROLS[4], W4_CONTROLS[5]

};

const Control floatControls[] = {
    W4_CONTROLS[6]};

const int intControls_size = sizeof(intControls) / sizeof(intControls[0]);
const int floatControls_size = sizeof(floatControls) / sizeof(floatControls[0]);
//const int allControls_size = intControls_size + floatControls_size;

abstract_panel::abstract_panel(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxScrolledWindow(parent, id, pos, size, style | wxTAB_TRAVERSAL | wxHSCROLL, title)
{
	//	abstract_panel::expertModeEnadbled = false;
	SetScrollbars(10, 10, 62, 62);
	Layout();
}

abstract_panel::~abstract_panel(void)
{
}

bool abstract_panel::loadValuesIntoMap()
{
	try {
		//special treatment for resolution that'll set proper defaults for res
		wxDisplay display(0);
		//		wxRect display_rect ( display.GetGeometry() );
		//		const int current_x_res = LSL::susynclib().GetSpringConfigInt(RC_TEXT[0].key,display_rect.width);
		//		const int current_y_res = LSL::susynclib().GetSpringConfigInt(RC_TEXT[1].key,display_rect.height);

		for (int i = 0; i < intControls_size; ++i) {
			intSettings[intControls[i].key] = LSL::usync().GetSpringConfigInt(STD_STRING(intControls[i].key), FromwxString(intControls[i].def));
		}
		for (int i = 0; i < floatControls_size; ++i) {
			floatSettings[floatControls[i].key] = LSL::usync().GetSpringConfigFloat(STD_STRING(floatControls[i].key), FromwxString(floatControls[i].def));
		}
	} catch (...) {
		customMessageBox(SS_MAIN_ICON, _("Could not access your settings.\n"), _("Error"), wxOK | wxICON_HAND, 0);
		abstract_panel::settingsChanged = false;
		return false;
	}

	return true; // SUCCESS!
}

void abstract_panel::loadDefaults()
{
	//const Control RO_SLI[9]
	for (int i = 0; i < s_category_sizes[_T("RO_SLI")]; ++i)
		intSettings[RO_SLI[i].key] = FromwxString(RO_SLI[i].def);

	//const Control VO_CBOX[3]
	for (int i = 0; i < s_category_sizes[_T("VO_CBOX")]; ++i)
		intSettings[VO_CBOX[i].key] = FromwxString(VO_CBOX[i].def);

	//const Control VO_RBUT[2]
	for (int i = 0; i < s_category_sizes[_T("VO_RBUT")]; ++i)
		intSettings[VO_RBUT[i].key] = FromwxString(VO_RBUT[i].def);

	//	const Control VO_SLI[1]
	for (int i = 0; i < s_category_sizes[_T("VO_SLI")]; ++i)
		intSettings[VO_SLI[i].key] = FromwxString(VO_SLI[i].def);

	//	const Control VO_SLI_EXT[1]
	for (int i = 0; i < s_category_sizes[_T("VO_SLI_EXT")]; ++i)
		intSettings[VO_SLI_EXT[i].key] = FromwxString(VO_SLI_EXT[i].def);

	//	const Control AO_SLI[3]
	for (int i = 0; i < s_category_sizes[_T("AO_SLI")]; ++i)
		intSettings[AO_SLI[i].key] = FromwxString(AO_SLI[i].def);

	//	const Control QA_CBOX[10]
	for (int i = 0; i < s_category_sizes[_T("QA_CBOX")]; ++i)
		intSettings[QA_CBOX[i].key] = FromwxString(QA_CBOX[i].def);

	//	const Control UI_CBOX[17]
	for (int i = 0; i < s_category_sizes[_T("UI_CBOX")]; ++i)
		intSettings[UI_CBOX[i].key] = FromwxString(UI_CBOX[i].def);

	//	const Control MO_SLI[5]
	for (int i = 0; i < s_category_sizes[_T("MO_SLI")]; ++i)
		intSettings[MO_SLI[i].key] = FromwxString(MO_SLI[i].def);

	//	const Control MO_SLI_EXT[5]
	for (int i = 0; i < s_category_sizes[_T("MO_SLI_EXT")]; ++i)
		intSettings[MO_SLI_EXT[i].key] = FromwxString(MO_SLI_EXT[i].def);

	//	const Control DO_SLI[1]
	for (int i = 0; i < s_category_sizes[_T("DO_SLI")]; ++i)
		intSettings[DO_SLI[i].key] = FromwxString(DO_SLI[i].def);

	//	const Control DO_CBOX[2]
	for (int i = 0; i < s_category_sizes[_T("DO_CBOX")]; ++i)
		intSettings[DO_CBOX[i].key] = FromwxString(DO_CBOX[i].def);

	//	const Control WR_COMBOX[4]
	for (int i = 0; i < s_category_sizes[_T("WR_COMBOX")]; ++i)
		intSettings[WR_COMBOX[i].key] = FromwxString(WR_COMBOX[i].def);

	//	const Control MO_CBOX[2]
	for (int i = 0; i < s_category_sizes[_T("MO_CBOX")]; ++i)
		intSettings[MO_CBOX[i].key] = FromwxString(MO_CBOX[i].def);

	//	const Control MO_RBUT[5]
	for (int i = 0; i < s_category_sizes[_T("MO_RBUT")]; ++i)
		intSettings[MO_RBUT[i].key] = FromwxString(MO_RBUT[i].def);

	//	const Control RC_TEXT[2]
	for (int i = 0; i < s_category_sizes[_T("RC_TEXT")]; ++i)
		intSettings[RC_TEXT[i].key] = FromwxString(RC_TEXT[i].def);

	//	const Control UI_ZOOM[1]
	for (int i = 0; i < s_category_sizes[_T("UI_ZOOM")]; ++i)
		intSettings[UI_ZOOM[i].key] = FromwxString(UI_ZOOM[i].def);

	for (int i = 0; i < s_category_sizes[_T("W4_CONTROLS")] - 1; ++i)
		intSettings[W4_CONTROLS[i].key] = FromwxString(W4_CONTROLS[i].def);

	const size_t idx = s_category_sizes[_T("W4_CONTROLS")] - 1;
	floatSettings[W4_CONTROLS[idx].key] = FromwxString(W4_CONTROLS[idx].def); //TODO add comment: what does this do?
}

void abstract_panel::OnSliderMove(wxCommandEvent& event)
{

	settingsChanged = true;

	wxSlider* slider = (wxSlider*)event.GetEventObject();

	int value = slider->GetValue();
	int id = event.GetId();

	switch (id) {
		case ID_RO_SLI_0: {
			// shadow-map sizes
			int val = slider->GetValue();

			if (val < 1536) {
				slider->SetValue(1024);
				(intSettings)[RO_SLI[0].key] = 1024;
			}
			if (val > 1536 && val < 3072) {
				slider->SetValue(2048);
				(intSettings)[RO_SLI[0].key] = 2048;
			}
			if (val > 3072 && val < 6144) {
				slider->SetValue(4096);
				(intSettings)[RO_SLI[0].key] = 4096;
			}
			if (val > 6144) {
				slider->SetValue(8192);
				(intSettings)[RO_SLI[0].key] = 8192;
			}
		} break;

		case ID_RO_SLI_1:
		case ID_RO_SLI_2:
		case ID_RO_SLI_3:
		case ID_RO_SLI_4:
		case ID_RO_SLI_5:
		case ID_RO_SLI_6:
		case ID_RO_SLI_7:
		case ID_RO_SLI_8: {
			int i = id - RO_SLI[0].id;
			(intSettings)[RO_SLI[i].key] = value;
		} break;


		case ID_VO_SLI_0: {
			(intSettings)[VO_SLI_EXT[0].key] = (value > 0) ? 1 : 0;
			(intSettings)[VO_SLI[0].key] = value;
		} break;

		case ID_AO_SLI_0:
		case ID_AO_SLI_1:
		case ID_AO_SLI_2:
		case ID_AO_SLI_3:
		case ID_AO_SLI_4:
		case ID_AO_SLI_5: {
			int i = id - AO_SLI[0].id;
			(intSettings)[AO_SLI[i].key] = value;
		} break;


		case ID_DO_SLI_0: {
			(intSettings)[DO_SLI[0].key] = value;
		} break;

		case ID_MO_SLI_0: {
			(intSettings)[MO_SLI[0].key] = value;
			(intSettings)[MO_SLI_EXT[0].key] = (value > 0 ? 1 : 0);
		} break;
		case ID_MO_SLI_1: {
			(intSettings)[MO_SLI[1].key] = value;
			(intSettings)[MO_SLI_EXT[1].key] = (value > 0 ? 1 : 0);
		} break;
		case ID_MO_SLI_2: {
			(intSettings)[MO_SLI[2].key] = value;
			(intSettings)[MO_SLI_EXT[2].key] = (value > 0 ? 1 : 0);
		} break;
		case ID_MO_SLI_3: {
			(intSettings)[MO_SLI[3].key] = value;
			(intSettings)[MO_SLI_EXT[3].key] = (value > 0 ? 1 : 0);
		} break;
		case ID_MO_SLI_4: {
			(intSettings)[MO_SLI[4].key] = value;
			(intSettings)[MO_SLI_EXT[4].key] = (value > 0 ? 1 : 0);
		} break;
		default:
			wxLogWarning(_T("Unhandled switch case: %d"), id);
			break;
	}
}


void abstract_panel::OnTextUpdate(wxCommandEvent& event)
{

	settingsChanged = true;
	int eventID = event.GetId();

	if (eventID == ID_RES_CHOICES_LBOX_X || eventID == ID_RES_CHOICES_LBOX_Y) {
		wxTextCtrl* textField = (wxTextCtrl*)event.GetEventObject();
		wxString wxStr = textField->GetValue();
		long* res = new long;
		bool success = (wxStr.ToLong(res));

		switch (eventID) {
			case ID_RES_CHOICES_LBOX_X: {
				// TODO: input validation?
				if (success)
					(intSettings)[RC_TEXT[0].key] = int((*res));
			} break;
			case ID_RES_CHOICES_LBOX_Y: {
				// TODO: input validation?
				if (success)
					(intSettings)[RC_TEXT[1].key] = int((*res));
			} break;

			default:
				wxLogWarning(_T("Unhandled switch case: %d"), eventID);
				break;
		}
	}
}


void abstract_panel::OnCheckBoxTick(wxCommandEvent& event)
{

	settingsChanged = true;

	wxCheckBox* checkBox = (wxCheckBox*)event.GetEventObject();
	int checked = checkBox->IsChecked();
	int id = event.GetId();

	switch (id) {
		case ID_WINDOWP_VO_CBOX_0:
		case ID_WINDOWP_VO_CBOX_1:
		case ID_WINDOWP_VO_CBOX_2: {
			int i = id - VO_CBOX[0].id;
			(intSettings)[VO_CBOX[i].key] = checked;
		} break;

		case ID_WINDOWP_QA_CBOX_0:
		case ID_WINDOWP_QA_CBOX_1:
		case ID_WINDOWP_QA_CBOX_2:
		case ID_WINDOWP_QA_CBOX_3:
		case ID_WINDOWP_QA_CBOX_4:
		case ID_WINDOWP_QA_CBOX_5:
		case ID_WINDOWP_QA_CBOX_6:
		case ID_WINDOWP_QA_CBOX_7:
		case ID_WINDOWP_QA_CBOX_8:
		case ID_WINDOWP_QA_CBOX_9:
		case ID_WINDOWP_QA_CBOX_10:
		case ID_WINDOWP_QA_CBOX_11: {
			int i = id - QA_CBOX[0].id;
			(intSettings)[QA_CBOX[i].key] = checked;
		} break;

		//case ID_WINDOWP_UI_CBOX_0:
		case ID_WINDOWP_UI_CBOX_1:
		case ID_WINDOWP_UI_CBOX_2:
		case ID_WINDOWP_UI_CBOX_3:
		case ID_WINDOWP_UI_CBOX_4:
		case ID_WINDOWP_UI_CBOX_5:
		case ID_WINDOWP_UI_CBOX_6:
		case ID_WINDOWP_UI_CBOX_7:
		case ID_WINDOWP_UI_CBOX_8:
		case ID_WINDOWP_UI_CBOX_9:
		case ID_WINDOWP_UI_CBOX_10:
		case ID_WINDOWP_UI_CBOX_11:
		case ID_WINDOWP_UI_CBOX_12:
		case ID_WINDOWP_UI_CBOX_13:
		case ID_WINDOWP_UI_CBOX_14:
		case ID_WINDOWP_UI_CBOX_15:
		case ID_WINDOWP_UI_CBOX_16:
		case ID_WINDOWP_UI_CBOX_17:
		case ID_WINDOWP_UI_CBOX_18: {
			int i = id - UI_CBOX[0].id;
			(intSettings)[UI_CBOX[i].key] = checked;
		} break;

		case ID_W4_BumpWaterBlurReflection:
		case ID_W4_BumpWaterReflection:
		case ID_W4_BumpWaterShoreWaves:
		case ID_W4_BumpWaterUseDepthTexture: {
			int i = id - W4_CONTROLS[0].id;
			(intSettings)[W4_CONTROLS[i].key] = checked;
		} break;


		case ID_WINDOWP_DO_CBOX_0: {
			(intSettings)[DO_CBOX[0].key] = checked;
		} break;


		case ID_WINDOWC_MO_CBOX_0: {
			(intSettings)[MO_CBOX[0].key] = checked;
		} break;
		case ID_WINDOWC_MO_CBOX_1: {
			(intSettings)[MO_CBOX[1].key] = checked;
		} break;

		default:
			wxLogWarning(_T("Unhandled switch case: %d"), id);
			break;
	}
}


void abstract_panel::OnRadioButtonToggle(wxCommandEvent& event)
{

	settingsChanged = true;

	wxRadioButton* radioButton = (wxRadioButton*)event.GetEventObject();
	radioButton->GetValue();

	switch (event.GetId()) {
		case ID_WINDOWP_VO_RBUT_0: {
			(intSettings)[VO_RBUT[0].key] = 16;
		} break;
		case ID_WINDOWP_VO_RBUT_1: {
			(intSettings)[VO_RBUT[0].key] = 24;
		} break;

		case ID_WINDOWC_MO_RBUT_0: {
			(intSettings)[MO_RBUT[0].key] = 1;
		} break; // OH button (CamMode 1)
		case ID_WINDOWC_MO_RBUT_1: {
			(intSettings)[MO_RBUT[1].key] = 3;
		} break; // ROH button (CamMode 2)
		case ID_WINDOWC_MO_RBUT_2: {
			(intSettings)[MO_RBUT[2].key] = 2;
		} break; // TW button (CamMode 3)
		case ID_WINDOWC_MO_RBUT_3: {
			(intSettings)[MO_RBUT[3].key] = 0;
		} break; // FPS button (CamMode 0)
		case ID_WINDOWC_MO_RBUT_4: {
			(intSettings)[MO_RBUT[4].key] = 4;
		} break; // FC button (CamMode 4)

		default:
			wxLogWarning(_T("Unhandled switch case: %d"), event.GetId());
			break;
	}
}

void abstract_panel::OnComboBoxChange(wxCommandEvent& event)
{

	settingsChanged = true;

	wxComboBox* comboBox = (wxComboBox*)event.GetEventObject();
	const wxString choice = comboBox->GetValue();

	switch (event.GetId()) {
		case ID_WINDOWP_WR_COMBOX: {
			int choiceIndex = 0;
			for (unsigned int i = 1; i < sizeof(WR_COMBOX_CHOICES) / sizeof(WR_COMBOX_CHOICES[0]); ++i) {
				if (choice == WR_COMBOX_CHOICES[i])
					choiceIndex = i;
			}
			switch (choiceIndex) {
				case 0: {
					(intSettings)[WR_COMBOX[0].key] = 0;
				} break; // Basic
				case 1: {
					(intSettings)[WR_COMBOX[0].key] = 1;
				} break; // Refl
				case 2: {
					(intSettings)[WR_COMBOX[0].key] = 3;
				} break; // Refl + Refr
				case 3: {
					(intSettings)[WR_COMBOX[0].key] = 2;
				} break; // Dyna
				case 4: {
					(intSettings)[WR_COMBOX[0].key] = 4;
				} break; // BumpWater

				default:
					wxLogWarning(_T("Unhandled switch case: %d"), choiceIndex);
					break;
			}
			break;
		}

		case ID_W4_BumpWaterRefraction: {
			int choiceIndex = 0;
			for (unsigned int i = 1; i < sizeof(W4_REFRACTION_CHOICES) / sizeof(W4_REFRACTION_CHOICES[0]); ++i) {
				if (choice == W4_REFRACTION_CHOICES[i])
					choiceIndex = i;
			}

			(intSettings)[W4_CONTROLS[5].key] = choiceIndex;
			break;
		}

		case ID_W4_BumpWaterTexSizeReflection: {
			int choiceIndex = 0;
			for (unsigned int i = 1; i < sizeof(W4_TEXSIZE_CHOICES) / sizeof(W4_TEXSIZE_CHOICES[0]); ++i) {
				if (choice == W4_TEXSIZE_CHOICES[i])
					choiceIndex = i;
			}

			(intSettings)[W4_CONTROLS[4].key] = choiceIndex;
			break;
		}

		case ID_SHADOW_CHOICES: {
			int choiceIndex = comboBox->GetCurrentSelection();

			//really off = -1, off = 0, on = 1
			//const wxString SHADOW_CHOICES[] = { _T("disabled"), _T("off"), _T("on") };
			(intSettings)[QA_CBOX[0].key] = choiceIndex - 1;
			break;
		}

		case ID_SIMPLE_QUAL_CBX: {

			for (int i = 0; i < prVal_RenderQuality_size; ++i) {
				presetValues<int, 5> pop = prVal_RenderQuality[i];
				int k = (pop.values[choice]);

				(intSettings)[prVal_RenderQuality[i].key] = k;
			}

			for (int i = 0; i < prVal_W4_size; ++i) {
				presetValues<int, 5> pop = prVal_W4[i];
				int k = (pop.values[choice]);

				(intSettings)[prVal_W4[i].key] = k;
			}

			presetValues<float, 5> pop = pr_w4_BumpWaterAnisotropy;
			float k = (pop.values[choice]);
			wxString key = pr_w4_BumpWaterAnisotropy.key;
			(floatSettings)[key] = k;
			//float debug = (floatSettings)[key]
			break;
		}
		case ID_SIMPLE_DETAIL_CBX: {
			for (int i = 0; i < prVal_RenderDetail_size; ++i) {
				presetValues<int, 3> pop = prVal_RenderDetail[i];
				int k = (pop.values[choice]);
				(intSettings)[prVal_RenderDetail[i].key] = k;
			}
			break;
		}
		case ID_SIMPLE_MODE_CBX: {
			int modeIndex = -1;
			for (int i = 0; i < vl_Resolution_Str_size; ++i) {
				if (choice == vl_Resolution_Str[i])
					modeIndex = i;
			}
			if (modeIndex != -1) {
				(intSettings)[_T("XResolution")] = vl_Resolution_X[modeIndex];
				(intSettings)[_T("YResolution")] = vl_Resolution_Y[modeIndex];
				if (modeIndex > vl_Resolution_startOfDualScreenRes)
					(intSettings)[_T("DualScreenMode")] = 1;
				else
					(intSettings)[_T("DualScreenMode")] = 0;
			}
			break;
		}
		default:
			wxLogWarning(_T("Unhandled switch case: %d"), event.GetId());
			break;
	}
}

void abstract_panel::OnSpinCtrlDoubleChange(wxSpinDoubleEvent& event)
{
	if (event.GetId() == ID_W4_BumpWaterAnisotropy) {
		wxSpinCtrlDouble* aniso = (wxSpinCtrlDouble*)event.GetEventObject();
		(floatSettings)[W4_CONTROLS[6].key] = aniso->GetValue();
		settingsChanged = true;
	}
}

void abstract_panel::OnSpinControlChange(wxSpinEvent& event)
{
	if (event.GetId() == ID_WINDOWP_UI_MW_SPD) {
		wxSpinCtrl* zoom = (wxSpinCtrl*)event.GetEventObject();
		(intSettings)[UI_ZOOM[0].key] = zoom->GetValue();
		settingsChanged = true;
	}
}

//TODO inquire about floatsettings
bool abstract_panel::saveSettings()
{
	try {
		for (intMap::const_iterator i = intSettings.begin(); i != intSettings.end(); ++i) {
			LSL::usync().SetSpringConfigInt(STD_STRING(i->first), i->second);
		}
		for (stringMap::const_iterator s = stringSettings.begin(); s != stringSettings.end(); ++s) {
			LSL::usync().SetSpringConfigString(STD_STRING(s->first), STD_STRING(s->second));
		}
		for (floatMap::const_iterator f = floatSettings.begin(); f != floatSettings.end(); ++f) {
			LSL::usync().SetSpringConfigFloat(STD_STRING(f->first), f->second);
		}
	} catch (...) {
		customMessageBox(SS_MAIN_ICON, _("Could not save, unitsync not properly loaded"), _("SpringSettings Error"), wxOK | wxICON_HAND, 0);
		return false;
	}

	return true;
}

void abstract_panel::updateControls(int /*unused*/)
{
}

// note that the SpinCtrlDouble change is hadnled explicitly via the control calling the right handler
BEGIN_EVENT_TABLE(abstract_panel, wxPanel)
EVT_SLIDER(wxID_ANY, abstract_panel::OnSliderMove)
EVT_TEXT(wxID_ANY, abstract_panel::OnTextUpdate)
EVT_CHECKBOX(wxID_ANY, abstract_panel::OnCheckBoxTick)
EVT_RADIOBUTTON(wxID_ANY, abstract_panel::OnRadioButtonToggle)
//	EVT_IDLE(                       abstract_panel::update)
EVT_SPINCTRL(wxID_ANY, abstract_panel::OnSpinControlChange)

EVT_COMBOBOX(wxID_ANY, abstract_panel::OnComboBoxChange)
END_EVENT_TABLE()
