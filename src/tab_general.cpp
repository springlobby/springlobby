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

void general_panel::initGeneralSizer(wxStaticBoxSizer* sizer) {
	// i < "sizeof"(GO_CBOX)
	for (int i = 0; i <= 21; i++) {
		wxCheckBox* checkBox = new wxCheckBox(this, GO_CBOX[i].id, _S(GO_CBOX[i].lbl));
		checkBox->SetValue(configHandler.GetInt(GO_CBOX[i].key,fromString<int>(GO_CBOX[i].def)));

		sizer->Add(checkBox, 0, wxTOP, ((i == 0)? 10: 0));
	}
}

general_panel::general_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	wxSizer* childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	wxStaticBoxSizer* generalSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("General Options"), WX_DEF_P, wxSize(220, 300), 0, _S("")), wxVERTICAL);

	initGeneralSizer(generalSizer);

	childLSizer->Add(0, 5, 0);
	childLSizer->Add(generalSizer);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer);

	SetSizer(parentSizer,true); // true --> delete old sizer if present

}

general_panel::~general_panel(void) {

}


BEGIN_EVENT_TABLE(general_panel, abstract_panel)
	EVT_SLIDER(wxID_ANY,            general_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              general_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          general_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       general_panel::OnRadioButtonToggle)
	EVT_IDLE(                       general_panel::update)
END_EVENT_TABLE()
