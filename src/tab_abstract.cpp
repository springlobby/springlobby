/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (René Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    Settings++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Settings++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Settings++.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "tabs.h"

abstract_panel::abstract_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : wxPanel(parent, id, pos, size, style,title) {

}

abstract_panel::~abstract_panel(void) {

}

void abstract_panel::OnSliderMove(wxCommandEvent& event) {

	wxSlider* slider = (wxSlider*) event.GetEventObject();

    int value = slider->GetValue();

	switch (event.GetId()) {
		case ID_RO_SLI_0: {
			// shadow-map sizes
			int val = slider->GetValue();

			if (val < 1536) {
				slider->SetValue(1024);
				configHandler.SetInt(RO_SLI[0].key, 1024);
			}
			if (val > 1536 && val < 3072) {
				slider->SetValue(2048);
				configHandler.SetInt(RO_SLI[0].key, 2048);
			}
			if (val > 3072 && val < 6144) {
				slider->SetValue(4096);
				configHandler.SetInt(RO_SLI[0].key, 4096);
			}
		} break;

		case ID_RO_SLI_1: { configHandler.SetInt(RO_SLI[1].key, value); } break;
		case ID_RO_SLI_2: { configHandler.SetInt(RO_SLI[2].key, value); } break;
		case ID_RO_SLI_3: { configHandler.SetInt(RO_SLI[3].key, value); } break;
		case ID_RO_SLI_4: { configHandler.SetInt(RO_SLI[4].key, value); } break;
		case ID_RO_SLI_5: { configHandler.SetInt(RO_SLI[5].key, value); } break;
		case ID_RO_SLI_6: { configHandler.SetInt(RO_SLI[6].key, value); } break;
		case ID_RO_SLI_7: { configHandler.SetInt(RO_SLI[7].key, value); } break;
		case ID_RO_SLI_8: { configHandler.SetInt(RO_SLI[8].key, value); } break;

		case ID_VO_SLI_0: {
			configHandler.SetInt(VO_SLI_EXT[0].key, (value > 0)? 1: 0);
			configHandler.SetInt(VO_SLI[0].key, value);
		} break;

		case ID_AO_SLI_0: { configHandler.SetInt(AO_SLI[0].key, value); } break;
		case ID_AO_SLI_1: { configHandler.SetInt(AO_SLI[1].key, value); } break;
		case ID_AO_SLI_2: { configHandler.SetInt(AO_SLI[2].key, value); } break;

		case ID_DO_SLI_0: { configHandler.SetInt(DO_SLI[0].key, value); } break;

		case ID_MO_SLI_0: { configHandler.SetInt(MO_SLI[0].key, value); } break;
        case ID_MO_SLI_1: { configHandler.SetInt(MO_SLI[1].key, value); } break;
        case ID_MO_SLI_2: { configHandler.SetInt(MO_SLI[2].key, value); } break;
        case ID_MO_SLI_3: { configHandler.SetInt(MO_SLI[3].key, value); } break;
        case ID_MO_SLI_4: { configHandler.SetInt(MO_SLI[4].key, value); } break;

	}
}


void abstract_panel::OnTextUpdate(wxCommandEvent& event) {

	wxTextCtrl* textField = (wxTextCtrl*) event.GetEventObject();
	wxString wxStr = textField->GetValue();
    long* res = new long; 
    bool success = (wxStr.ToLong(res));

	switch (event.GetId()) {
		case ID_RES_CHOICES_LBOX_X: {
			// note: input validation?
			if (success)
			     configHandler.SetInt(RC_TEXT[0].key, int((*res)));
		} break;
		case ID_RES_CHOICES_LBOX_Y: {
			// note: input validation?
			if (success)
			     configHandler.SetInt(RC_TEXT[1].key, int((*res)));
		} break;
	}
}


void abstract_panel::OnCheckBoxTick(wxCommandEvent& event) {

	wxCheckBox* checkBox = (wxCheckBox*) event.GetEventObject();
	int checked = checkBox->IsChecked();
	int id = event.GetId();

	switch (id) {
		case ID_WINDOWP_VO_CBOX_0:
		case ID_WINDOWP_VO_CBOX_1:
		case ID_WINDOWP_VO_CBOX_2:
		case ID_WINDOWP_VO_CBOX_3:
		case ID_WINDOWP_VO_CBOX_4: {
			int i = id - VO_CBOX[0].id;
			configHandler.SetInt(VO_CBOX[i].key, checked);
		} break;

		case ID_WINDOWP_GO_CBOX_0:
		case ID_WINDOWP_GO_CBOX_1:
		case ID_WINDOWP_GO_CBOX_2:
		case ID_WINDOWP_GO_CBOX_3:
		case ID_WINDOWP_GO_CBOX_4:
		case ID_WINDOWP_GO_CBOX_5:
		case ID_WINDOWP_GO_CBOX_6:
		case ID_WINDOWP_GO_CBOX_7:
		case ID_WINDOWP_GO_CBOX_8:
		case ID_WINDOWP_GO_CBOX_9:
		case ID_WINDOWP_GO_CBOX_10:
		case ID_WINDOWP_GO_CBOX_11:
		case ID_WINDOWP_GO_CBOX_12:
		case ID_WINDOWP_GO_CBOX_13:
		case ID_WINDOWP_GO_CBOX_14:
		case ID_WINDOWP_GO_CBOX_15:
		case ID_WINDOWP_GO_CBOX_16:
		case ID_WINDOWP_GO_CBOX_17:
		case ID_WINDOWP_GO_CBOX_18:
		case ID_WINDOWP_GO_CBOX_19:
		case ID_WINDOWP_GO_CBOX_20:
		case ID_WINDOWP_GO_CBOX_21: {
			int i = id - GO_CBOX[0].id;
			configHandler.SetInt(GO_CBOX[i].key, checked);
		} break;

		case ID_WINDOWP_DO_CBOX_0: { configHandler.SetInt(DO_CBOX[0].key, checked); } break;
		case ID_WINDOWP_DO_CBOX_1: { configHandler.SetInt(DO_CBOX[1].key, checked); } break;
		// todo add mouse boxes
	}
}


void abstract_panel::OnRadioButtonToggle(wxCommandEvent& event) {

	wxRadioButton* radioButton = (wxRadioButton*) event.GetEventObject();
	radioButton->GetValue();

	switch (event.GetId()) {
		case ID_WINDOWP_VO_RBUT_0: { configHandler.SetInt(VO_RBUT[0].key, 16); } break;
		case ID_WINDOWP_VO_RBUT_1: { configHandler.SetInt(VO_RBUT[0].key, 24); } break;

		case ID_WINDOWP_WR_RBUT_0: { configHandler.SetInt(WR_RBUT[0].key, 0); } break;	// Basic
		case ID_WINDOWP_WR_RBUT_1: { configHandler.SetInt(WR_RBUT[0].key, 1); } break;	// Refl
		case ID_WINDOWP_WR_RBUT_2: { configHandler.SetInt(WR_RBUT[0].key, 3); } break;	// Refl + Refr
		case ID_WINDOWP_WR_RBUT_3: { configHandler.SetInt(WR_RBUT[0].key, 2); } break;	// Dyna
		// todo add mous buttons
	}
}


void abstract_panel::update(wxIdleEvent& event) {
	event.RequestMore();
}


BEGIN_EVENT_TABLE(abstract_panel, wxPanel)
	EVT_SLIDER(wxID_ANY,            abstract_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              abstract_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          abstract_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       abstract_panel::OnRadioButtonToggle)
	EVT_IDLE(                       abstract_panel::update)
END_EVENT_TABLE()
