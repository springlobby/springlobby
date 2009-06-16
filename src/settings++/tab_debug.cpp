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

#include "tab_debug.h"
#include "se_utils.h"
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/event.h>
#include <wx/defs.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>


#include "Defs.hpp"

void debug_panel::initDebugSizer(wxStaticBoxSizer* sizer) {
	checkBox0 = new wxCheckBox(this, DO_CBOX[0].id, (DO_CBOX[0].lbl));
	slider = new wxSlider(this, DO_SLI[0].id, 0, 0, 10, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);

	slider->SetToolTip( DO_SLI[0].tTip[0]);
	checkBox0->SetToolTip(DO_CBOX[0].tTip[0]);

	sizer->Add(0, 10, 0);
	sizer->Add(checkBox0, 0, wxTOP, 5);

	sizer->Add(0, 10, 0);
	sizer->Add(new wxStaticText(this, -1, (DO_SLI[0].lbl)), 0, wxTOP, 15);
	sizer->Add(slider, 0, wxTOP, 5);
	sizer->Add(0, 10, 0);
}

debug_panel::debug_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	 parentSizer = new wxBoxSizer(wxHORIZONTAL);	// main window sizer (three columns)
	 childLSizer = new wxBoxSizer(wxVERTICAL);		// main window left column sizer

	// sizers for static boxes containing sliders, checkboxes, radiobuttons
	 debugSizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Debug Options"), WX_DEF_P, wxSize(-1, -1), 0, wxEmptyString), wxVERTICAL);

	initDebugSizer(debugSizer);


	childLSizer->Add(0, 5, 0);
	childLSizer->Add(debugSizer,0,wxEXPAND|wxALL,5);

	parentSizer->Add(10, 0, 0);
	parentSizer->Add(childLSizer,0,wxEXPAND|wxTOP,5);

	updateControls(UPDATE_ALL);
	SetSizer(parentSizer); // true --> delete old sizer if present

}
void debug_panel::updateControls(int what_to_update)
{
	checkBox0->SetValue(intSettings[DO_CBOX[0].key]);

	slider->SetValue(intSettings[DO_SLI[0].key]);
}
debug_panel::~debug_panel(void) {

}

BEGIN_EVENT_TABLE(debug_panel, abstract_panel)
	EVT_SLIDER(wxID_ANY,            debug_panel::OnSliderMove)
//	EVT_TEXT(wxID_ANY,              debug_panel::OnTextUpdate)
//	EVT_CHECKBOX(wxID_ANY,          debug_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       debug_panel::OnRadioButtonToggle)
	//EVT_IDLE(                       debug_panel::update)
END_EVENT_TABLE()
