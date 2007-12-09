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

#include "tab_simple.h"
#include "se_utils.h"
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/event.h>
#include <wx/defs.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include "../springunitsynclib.h"
#include "Defs.hpp"
#include "presets.h"

const wxString infoTextContent= _T("INFOTEXT HERE");


void tab_simple::initOptSizer(wxFlexGridSizer* sizer ) {
	sizer->Add(new wxStaticText(this, -1,  wxT("RENDER_QUALITY")), 0,wxALL);
	//sizer->Add(new wxStaticText(this, -1, wxT("Water Quality")), 0, wxTOP , 10);
	renderQuality_CBX = new wxComboBox(this, ID_SIMPLE_QUAL_CBX, levels_vlow_To_vHigh[0], wxDefaultPosition, wxSize(220,21), 
			5,levels_vlow_To_vHigh,wxCB_DROPDOWN|wxCB_READONLY);
	sizer->Add(renderQuality_CBX, 0, wxBOTTOM, 15);	

	sizer->Add(new wxStaticText(this, -1,  wxT("RENDER_DETAIL")), 0,wxALL);
	renderDetail_CBX = new wxComboBox(this, ID_SIMPLE_DETAIL_CBX, levels_low_To_High[0], wxDefaultPosition, wxSize(220,21), 
			3,levels_low_To_High,wxCB_DROPDOWN|wxCB_READONLY);
	sizer->Add(renderDetail_CBX, 0, wxBOTTOM, 15);	

	sizer->Add(new wxStaticText(this, -1,  wxT("VIDEO_MODE")), 0,wxALL);
	videoMode_CBX = new wxComboBox(this, ID_SIMPLE_MODE_CBX, vl_Resolution_Str[0], wxDefaultPosition, wxSize(220,21), 
			vl_Resolution_Str_size,vl_Resolution_Str,wxCB_DROPDOWN|wxCB_READONLY);
	sizer->Add(videoMode_CBX, 0, wxBOTTOM, 15);	
	
	sizer->Add(new wxStaticText(this, -1, (AO_SLI[1].lbl)), 0, wxTOP, 15);
	audioVolume_SLI =  new wxSlider(this, AO_SLI[1].id, configHandler->GetSpringConfigInt(AO_SLI[1].key,fromString(AO_SLI[1].def)), 0, 100, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	sizer->Add(audioVolume_SLI, 0, wxBOTTOM, 15);	
}

void tab_simple::initInfoSizer(wxFlexGridSizer* sizer)
{
	infoText = new wxStaticText(this,-1,infoTextContent);
	sizer->Add(infoText,0,wxALL,10);
}

tab_simple::tab_simple(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
: abstract_panel(parent, id , title , pos , size, style) {

	SetSizer(0, true);
	 parentSizer = new wxFlexGridSizer(2,0,0);	
	//wxSizer* leftSizer = new wxFlexGridSizer(1,15,0);
	 middleSizer = new wxFlexGridSizer(1,15,0);
	//wxSizer* rightSizer = new wxFlexGridSizer(1,15,0);//for info
	 Sizer_CBX = new wxFlexGridSizer(2,10,10);
	 Sizer_info = new wxFlexGridSizer(1,15,10);
	//wxFlexGridSizer* SizerC = new wxFlexGridSizer(1,15,10);
	//wxFlexGridSizer* SizerD = new wxFlexGridSizer(1,5,10);
	 boxA = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("SIMPLE_OPTIONS"));
	 boxB = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Info"));
	//		wxStaticBoxSizer* boxC = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Anti-Aliasing Options"));
	//		wxStaticBoxSizer* boxD = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Z-/Depth-Buffer"));
	//		Sizer_CBX->AddGrowableCol(0);
	//		Sizer_CBX->AddGrowableCol(1);
	//	    SizerB->AddGrowableCol(0);
	//	    SizerC->AddGrowableCol(0);
	//	    SizerD->AddGrowableCol(0);


	initOptSizer(Sizer_CBX);
	initInfoSizer(Sizer_info);
	
	//		initVideoSizer(SizerB);
	//		initAASizer(SizerC);
	//		initZBufferSizer(SizerD);

	Sizer_CBX->Fit(this);
	Sizer_CBX->SetSizeHints(this);
    Sizer_info->Fit(this);
    Sizer_info->SetSizeHints(this);
	//	    SizerC->Fit(this);
	//	    SizerC->SetSizeHints(this);
	//	    SizerD->Fit(this);
	//	    SizerD->SetSizeHints(this);

	boxA->Add(Sizer_CBX);
	boxB->Add(Sizer_info);
	//	    boxC->Add(SizerC);
	//	    boxD->Add(SizerD);
	//	    leftSizer->Add(boxB,0,wxEXPAND);
	//	    leftSizer->Add(boxC);
	middleSizer->Add(boxA,0,wxEXPAND);
	middleSizer->Add(boxB,0,wxEXPAND);
	//  parentSizer->Add(leftSizer,0,wxALIGN_LEFT|wxALIGN_TOP |wxALL,10);
	parentSizer->Add(middleSizer,0,wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND,10);

	SetSizer(parentSizer, true); // true --> delete old sizer if present
}

void tab_simple::updateControls(int what_to_update)
{
	audioVolume_SLI->SetValue(intSettings[AO_SLI[1].key]);
}

tab_simple::~tab_simple(void) {

}

void tab_simple::OnComboBoxChange(wxCommandEvent& event)
{
	abstract_panel::OnComboBoxChange(event);
	switch( event.GetId()){
		case ID_SIMPLE_MODE_CBX: {
			qualityTab->updateControls(UPDATE_VIDEO_MODE);
			break;
		}
		case ID_SIMPLE_DETAIL_CBX: {
			detailTab->updateControls(UPDATE_ALL);
			break;
		}
		case ID_SIMPLE_QUAL_CBX: {
			qualityTab->updateControls(UPDATE_QA_BOXES);
			break;
		}
	}
}

void tab_simple::setTabs(abstract_panel* a,abstract_panel* b)
{
	detailTab = a;
	qualityTab = b;
}

BEGIN_EVENT_TABLE(tab_simple, abstract_panel)
	EVT_SLIDER(wxID_ANY,            tab_simple::OnSliderMove)
	EVT_TEXT(wxID_ANY,              tab_simple::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          tab_simple::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       tab_simple::OnRadioButtonToggle)
//	EVT_IDLE(                       tab_simple::update)
	EVT_COMBOBOX(wxID_ANY, 		tab_simple::OnComboBoxChange)
END_EVENT_TABLE()


