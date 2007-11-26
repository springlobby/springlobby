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

void tab_ui::initScrollSpeedSizer(wxStaticBoxSizer* sizer) {
	// i < "sizeof"(MO_SLI)
	sizer->Add(5,10,0);
	for (int i = 0; i < 5; i++) {
		wxSlider* slider = new wxSlider(this, MO_SLI[i].id, configHandler.GetInt(MO_SLI[i].key,fromString<int>(MO_SLI[i].def)), 0, 10, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
		if (i > 0)
			sizer->Add(5,32,0);
		sizer->Add(new wxStaticText(this, wxID_ANY, _S(MO_SLI[i].lbl), wxDefaultPosition, wxDefaultSize, 10));
		sizer->Add(slider, 0, wxTOP, 0);
	}
	sizer->Add(5,10,0);
}

void tab_ui::initCameraSizer(wxStaticBoxSizer* sizer) {
	wxRadioButton* radioButtonA0 = new wxRadioButton(this, MO_RBUT[0].id, _S(MO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	wxRadioButton* radioButtonA1 = new wxRadioButton(this, MO_RBUT[1].id, _S(MO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA2 = new wxRadioButton(this, MO_RBUT[2].id, _S(MO_RBUT[2].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA3 = new wxRadioButton(this, MO_RBUT[3].id, _S(MO_RBUT[3].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);
	wxRadioButton* radioButtonA4 = new wxRadioButton(this, MO_RBUT[4].id, _S(MO_RBUT[4].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	switch (configHandler.GetInt(MO_RBUT[0].key,fromString<int>(MO_RBUT[0].def))) {
		case 0: { radioButtonA3->SetValue(1); } break;	// CamMode 0: FPS
		case 1: { radioButtonA0->SetValue(1); } break;	// CamMode 1: OH
		case 2: { radioButtonA1->SetValue(1); } break;	// CamMode 2: ROH
		case 3: { radioButtonA2->SetValue(1); } break;	// CamMode 3: TW
		case 4: { radioButtonA4->SetValue(1); } break;	// CamMode 4: FC
	}

	sizer->Add(radioButtonA0, 0, wxTOP, 10);
	sizer->Add(radioButtonA1, 0, wxTOP, 0);
	sizer->Add(radioButtonA2, 0, wxTOP, 0);
	sizer->Add(radioButtonA3, 0, wxTOP, 0);
	sizer->Add(radioButtonA4, 0, wxTOP, 0);
}

void tab_ui::initUiOptSizer(wxStaticBoxSizer* sizer)
{
	for (int i = 0; i < 13; i++) {
			wxCheckBox* checkBox = new wxCheckBox(this, UI_CBOX[i].id, _S(UI_CBOX[i].lbl));
			checkBox->SetValue(configHandler.GetInt(UI_CBOX[i].key,fromString<int>(UI_CBOX[i].def)));
			sizer->Add(checkBox, 0, wxTOP, (i == 0)? 10: 0);
		}
}

tab_ui::tab_ui(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxFlexGridSizer* pSizer = new wxFlexGridSizer(2,15,15);
	wxFlexGridSizer* cSizerL = new wxFlexGridSizer(1,10,10);
	wxFlexGridSizer* cSizerR = new wxFlexGridSizer(1,10,10);

	wxStaticBoxSizer* scrollSpeedSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Scroll Speeds (0 to disable)"), WX_DEF_P, wxSize(100, 200), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* cameraSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Default Camera Mode"), WX_DEF_P, wxSize(100, 100), 0, _S("")), wxVERTICAL);
	wxStaticBoxSizer* uiOptSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Misc. UI Options"), WX_DEF_P, wxSize(100, 100), 0, _S("")), wxVERTICAL);

	initScrollSpeedSizer(scrollSpeedSizer);
	initCameraSizer(cameraSizer);
	initUiOptSizer(uiOptSizer);

	cSizerR->Add(uiOptSizer);
	cSizerL->Add(scrollSpeedSizer);
	cSizerR->Add(cameraSizer,0,wxEXPAND);
	//pSizer->Add(5, 0, 0);
	pSizer->Add(cSizerL,0,wxALL,10);
	//pSizer->Add(25, 0, 0);
	pSizer->Add(cSizerR,0,wxALL,10);

	SetSizer(pSizer);

}

tab_ui::~tab_ui(void) {

}

BEGIN_EVENT_TABLE(tab_ui, abstract_panel)
	EVT_SLIDER(wxID_ANY,            tab_ui::OnSliderMove)
	EVT_TEXT(wxID_ANY,              tab_ui::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          tab_ui::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       tab_ui::OnRadioButtonToggle)
	EVT_IDLE(                       tab_ui::update)
END_EVENT_TABLE()
