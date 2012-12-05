/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://springrts.com/phpbb/viewtopic.php?t=12104
    for more info/help

    SpringLobby is free software: you can redistribute it and/or modify
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
#include <wx/version.h>

#include "ctrlconstants.h"

void audio_panel::initAudioSizer(wxStaticBoxSizer* sizer) {

    for (int i = 0; i < ctrl_audio_sliders_size; ++i ) {
        ctrl_audio_sliders[i] = new wxSlider(this, AO_SLI[i].id, 1, 0, i == 0 ? 128 : 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
        ctrl_audio_sliders[i]->SetToolTip(AO_SLI[i].tTip[0]);
#if wxCHECK_VERSION(2,9,2)
        ctrl_audio_sliders[i]->SetTickFreq( 10 );
#else
        ctrl_audio_sliders[i]->SetTickFreq( 10, 1 );
#endif
        sizer->Add( new wxStaticText(this, -1, (AO_SLI[i].lbl)), 0, wxTOP, 15 );
        sizer->Add( ctrl_audio_sliders[i], 0, wxALIGN_LEFT, 0 );
    }

#if wxCHECK_VERSION(2,9,2)
    ctrl_audio_sliders[0]->SetTickFreq( (128 / 10) );
#else
    ctrl_audio_sliders[0]->SetTickFreq( (128 / 10), 1 );
#endif
}

audio_panel::audio_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style)
{
    ctrl_audio_sliders = new wxSlider*[ctrl_audio_sliders_size];
	 parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	 childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
   audioSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Audio Options"),
    		WX_DEF_P, wxSize(230, 100), 0, wxEmptyString), wxVERTICAL);
	initAudioSizer(audioSizer);


	childLSizer->Add(0, 5, 0);
	childLSizer->Add(audioSizer,0,wxEXPAND|wxALL,5);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer,0,wxEXPAND|wxTOP,5);

	updateControls(UPDATE_ALL);
	SetSizer(parentSizer); // true --> delete old sizer if present
}

void audio_panel::updateControls(int /*unused*/)
{
    for (int i = 0; i < ctrl_audio_sliders_size; ++i ) {
        ctrl_audio_sliders[i]->SetValue(intSettings[AO_SLI[i].key]);
    }
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
