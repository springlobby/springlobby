/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
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

void mouse_panel::initMouseSpeedSizer(wxStaticBoxSizer* sizer) {
	// i < "sizeof"(MO_SLI)
	for (int i = 0; i < 5; i++) {
		wxSlider* slider = new wxSlider(this, MO_SLI[i].id, configHandler.GetInt(MO_SLI[i].key,fromString<int>(MO_SLI[i].def)), 0, 10, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
		sizer->Add(new wxStaticText(this, wxID_ANY, _S(MO_SLI[i].lbl), wxDefaultPosition, wxDefaultSize, 10));
		sizer->Add(slider, 0, wxTOP, 0);
	}
}

void mouse_panel::initMouseOptsSizer(wxStaticBoxSizer* sizer) {
	wxCheckBox* checkBox0 = new wxCheckBox(this, MO_CBOX[0].id, _S(MO_CBOX[0].lbl));
	wxCheckBox* checkBox1 = new wxCheckBox(this, MO_CBOX[1].id, _S(MO_CBOX[1].lbl));
	wxRadioButton* radioButtonA0 = new wxRadioButton(this, MO_RBUT[0].id, _S(MO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	wxRadioButton* radioButtonA1 = new wxRadioButton(this, MO_RBUT[1].id, _S(MO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA2 = new wxRadioButton(this, MO_RBUT[2].id, _S(MO_RBUT[2].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA3 = new wxRadioButton(this, MO_RBUT[3].id, _S(MO_RBUT[3].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA4 = new wxRadioButton(this, MO_RBUT[4].id, _S(MO_RBUT[4].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	checkBox0->SetValue(configHandler.GetInt(MO_CBOX[0].key,fromString<int>(MO_CBOX[0].def)));
	checkBox1->SetValue(configHandler.GetInt(MO_CBOX[1].key,fromString<int>(MO_CBOX[0].def)));

	// Default Camera Mode radiobuttons
	// (unrelated to which modes are enabled)
	switch (configHandler.GetInt(MO_RBUT[0].key,fromString<int>(MO_RBUT[0].def))) {
		case 0: { radioButtonA3->SetValue(1); } break;	// CamMode 0: FPS
		case 1: { radioButtonA0->SetValue(1); } break;	// CamMode 1: OH
		case 2: { radioButtonA1->SetValue(1); } break;	// CamMode 2: ROH
		case 3: { radioButtonA2->SetValue(1); } break;	// CamMode 3: TW
		case 4: { radioButtonA4->SetValue(1); } break;	// CamMode 4: FC
	}

	sizer->Add(checkBox0, 0, wxTOP, 0);
	sizer->Add(checkBox1, 0, wxTOP, 0);
	sizer->Add(0, 10, 0);
	sizer->Add(new wxStaticText(this, -1, wxT("Default Camera Mode"), wxDefaultPosition, wxDefaultSize, 10));
	sizer->Add(radioButtonA0, 0, wxTOP, 0);
	sizer->Add(radioButtonA1, 0, wxTOP, 0);
	sizer->Add(radioButtonA2, 0, wxTOP, 0);
	sizer->Add(radioButtonA3, 0, wxTOP, 0);
	sizer->Add(radioButtonA4, 0, wxTOP, 0);
}

mouse_panel::mouse_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* pSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer* cSizerL = new wxBoxSizer(wxVERTICAL);
	wxSizer* cSizerR = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* mouseSpeedSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Mouse Speed"), WX_DEF_P, wxSize(100, 200), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* mouseOptsSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Mouse Options"), WX_DEF_P, wxSize(100, 100), 0, _S("")), wxVERTICAL);

	initMouseSpeedSizer(mouseSpeedSizer);
	initMouseOptsSizer(mouseOptsSizer);

	cSizerL->Add(mouseSpeedSizer);
	cSizerR->Add(mouseOptsSizer);
	pSizer->Add(5, 0, 0);
	pSizer->Add(cSizerL);
	pSizer->Add(25, 0, 0);
	pSizer->Add(cSizerR);

	SetSizer(pSizer, true); // true --> delete old sizer if present

}

mouse_panel::~mouse_panel(void) {

}

BEGIN_EVENT_TABLE(mouse_panel, abstract_panel)
	EVT_SLIDER(wxID_ANY,            mouse_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              mouse_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          mouse_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       mouse_panel::OnRadioButtonToggle)
	EVT_IDLE(                       mouse_panel::update)
END_EVENT_TABLE()
