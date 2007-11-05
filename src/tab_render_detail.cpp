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

void tab_render_detail::initRendererSizer(wxFlexGridSizer* sizerL,wxFlexGridSizer* sizerR ) {
	const int extrema[] = {
		1024, 8192,   600, 3000,   20, 120,   100, 600,   0, 10,   0, 100,   100, 1000,   1000, 20000,   1, 20
	};

	// i < "sizeof"(RO_SLI)
	for (int i = 0; i <= 4; i++) {
		wxSlider* slider = new wxSlider(
			this, RO_SLI[i].id, configHandler.GetInt(RO_SLI[i].key,fromString<int>(RO_SLI[i].def)),
			extrema[i * 2],  extrema[(i * 2) + 1], WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V
		);
		slider->SetTickFreq((extrema[(i*2)+1] - extrema[i * 2]) / 10  ,1);
		sizerL->Add(new wxStaticText(this, _1,  _S(RO_SLI[i].lbl)), 0,wxALIGN_CENTER_VERTICAL|wxALL);
		sizerL->Add(slider, 0,wxALIGN_CENTER_VERTICAL|wxALL);
	}
	for (int i = 5; i <= 8; i++) {
		wxSlider* slider = new wxSlider(
			this, RO_SLI[i].id, configHandler.GetInt(RO_SLI[i].key,fromString<int>(RO_SLI[i].def)),
			extrema[i * 2],  extrema[(i * 2) + 1], WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V
		);
		slider->SetTickFreq((extrema[(i*2)+1] - extrema[i * 2]) / 10  ,1);
		sizerR->Add(new wxStaticText(this, _1,  _S(RO_SLI[i].lbl)), 0,wxALIGN_CENTER_VERTICAL|wxALL);
		sizerR->Add(slider, 0,wxALIGN_CENTER_VERTICAL|wxALL);
	}
}

tab_render_detail::tab_render_detail(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* parentSizer = new wxGridSizer(2,10,30);	// main window sizer (three columns)
//	wxSizer* childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer
//	wxSizer* childRSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	wxFlexGridSizer* rendererSizerA = new wxFlexGridSizer(2,40,10);
	wxFlexGridSizer* rendererSizerB = new wxFlexGridSizer(2,40,10);
	rendererSizerA->AddGrowableCol(0);
    rendererSizerA->AddGrowableCol(1);
    rendererSizerB->AddGrowableCol(0);
	rendererSizerB->AddGrowableCol(1);
	initRendererSizer(rendererSizerA,rendererSizerB);
	rendererSizerA->Fit(this);
    rendererSizerA->SetSizeHints(this);
    rendererSizerB->Fit(this);
    rendererSizerB->SetSizeHints(this);
    SetSizer(parentSizer);
    parentSizer->Add(rendererSizerA,0,wxALIGN_LEFT|wxALL,10);
    parentSizer->Add(rendererSizerB,0,wxALIGN_LEFT|wxALL,10);

//	childLSizer->Add(0, 5, 0);
 //	childLSizer->Add(rendererSizerA);
// 	//childRSizer->Add(0, 10, 0);
// 	//childRSizer->Add(rendererSizerB);
// 
// 	parentSizer->Add(5, 0, 0);
 //	parentSizer->Add(childLSizer);
// 
// 	parentSizer->Add(25, 0, 0);
// 	parentSizer->Add(childRSizer);

	

}

tab_render_detail::~tab_render_detail(void) {
	
}

BEGIN_EVENT_TABLE(tab_render_detail, abstract_panel)
	EVT_SLIDER(wxID_ANY,            rendering_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              rendering_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          rendering_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       rendering_panel::OnRadioButtonToggle)
	EVT_IDLE(                       rendering_panel::update)
END_EVENT_TABLE()
