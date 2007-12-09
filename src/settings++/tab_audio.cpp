/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "tab_audio.h"
#include "se_utils.h"
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/defs.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include "../springunitsynclib.h"
#include "Defs.hpp"

void audio_panel::initAudioSizer(wxStaticBoxSizer* sizer) {
	slider0 = new wxSlider(this, AO_SLI[0].id, configHandler->GetSpringConfigInt(AO_SLI[0].key,
			fromString(AO_SLI[0].def)), 8, 128, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	slider1 = new wxSlider(this, AO_SLI[1].id, configHandler->GetSpringConfigInt(AO_SLI[1].key,
			fromString(AO_SLI[1].def)), 1, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	slider2 = new wxSlider(this, AO_SLI[2].id, configHandler->GetSpringConfigInt(AO_SLI[2].key,
			fromString(AO_SLI[2].def)), 1, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);

    slider0->SetTickFreq((128-8) / 10   ,1);
    slider1->SetTickFreq(10             ,1);
    slider2->SetTickFreq(10             ,1);
	sizer->Add(new wxStaticText(this, -1, (AO_SLI[0].lbl)), 0, wxTOP, 15);
	sizer->Add(slider0, 0, wxALIGN_LEFT, 0);
	sizer->Add(new wxStaticText(this, -1, (AO_SLI[1].lbl)), 0, wxTOP, 15);
	sizer->Add(slider1, 0, wxALIGN_LEFT, 0);
	sizer->Add(new wxStaticText(this, -1, (AO_SLI[2].lbl)), 0, wxTOP, 15);
	sizer->Add(slider2, 0, wxALIGN_LEFT, 0);
}

audio_panel::audio_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	 parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	 childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
   audioSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxT("Audio Options"),
    		WX_DEF_P, wxSize(230, 100), 0, wxEmptyString), wxVERTICAL);
	initAudioSizer(audioSizer);


	childLSizer->Add(0, 5, 0);
	childLSizer->Add(audioSizer,1,wxEXPAND);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer,1,wxEXPAND);

	SetSizer(parentSizer); // true --> delete old sizer if present
}

void audio_panel::updateControls(int what_to_update)
{
	slider0->SetValue(intSettings[AO_SLI[0].key]);
	slider1->SetValue(intSettings[AO_SLI[1].key]);
	slider2->SetValue(intSettings[AO_SLI[2].key]);
}
audio_panel::~audio_panel(void) {

}




BEGIN_EVENT_TABLE(audio_panel, wxPanel)
	EVT_SLIDER(wxID_ANY,            audio_panel::OnSliderMove)
//	EVT_TEXT(wxID_ANY,              audio_panel::OnTextUpdate)
//	EVT_CHECKBOX(wxID_ANY,          audio_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       audio_panel::OnRadioButtonToggle)
//	EVT_IDLE(                       audio_panel::update)
END_EVENT_TABLE()
