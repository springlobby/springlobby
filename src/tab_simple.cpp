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

//void tab_simple::initRendererSizer(wxStaticBoxSizer* sizerL,wxStaticBoxSizer* sizerR ) {
//	
//}

tab_simple::tab_simple(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {

	wxSizer* parentSizer = new wxFlexGridSizer(2,0,0);	
		wxSizer* leftSizer = new wxFlexGridSizer(1,15,0);
		wxSizer* middleSizer = new wxFlexGridSizer(1,15,0);
		wxSizer* rightSizer = new wxFlexGridSizer(1,15,0);//for info
		wxFlexGridSizer* SizerA = new wxFlexGridSizer(1,10,10);
		wxFlexGridSizer* SizerB = new wxFlexGridSizer(1,15,10);
		wxFlexGridSizer* SizerC = new wxFlexGridSizer(1,15,10);
		wxFlexGridSizer* SizerD = new wxFlexGridSizer(1,5,10);
		wxStaticBoxSizer* boxA = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Render Quality Options"));
		wxStaticBoxSizer* boxB = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Video Mode Options"));
		wxStaticBoxSizer* boxC = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Anti-Aliasing Options"));
		wxStaticBoxSizer* boxD = new wxStaticBoxSizer(wxVERTICAL ,this,wxT("Z-/Depth-Buffer"));
		SizerA->AddGrowableCol(0);
	    SizerB->AddGrowableCol(0);
	    SizerC->AddGrowableCol(0);
	    SizerD->AddGrowableCol(0);

//		
//		initQualitySizer(SizerA);
//		initVideoSizer(SizerB);
//		initAASizer(SizerC);
//		initZBufferSizer(SizerD);
		
		SizerA->Fit(this);
	    SizerA->SetSizeHints(this);
	    SizerB->Fit(this);
	    SizerB->SetSizeHints(this);
	    SizerC->Fit(this);
	    SizerC->SetSizeHints(this);
	    SizerD->Fit(this);
	    SizerD->SetSizeHints(this);
	    
	    boxA->Add(SizerA);
	    boxB->Add(SizerB);
	    boxC->Add(SizerC);
	    boxD->Add(SizerD);
	    leftSizer->Add(boxB,0,wxEXPAND);
	    leftSizer->Add(boxC);
	    middleSizer->Add(boxA,0,wxEXPAND);
	    middleSizer->Add(boxD,0,wxEXPAND);
	    parentSizer->Add(leftSizer,0,wxALIGN_LEFT|wxALIGN_TOP |wxALL,10);
	    parentSizer->Add(middleSizer,0,wxALIGN_CENTER_HORIZONTAL|wxALL,10);
	   
	    SetSizer(parentSizer);
}

tab_simple::~tab_simple(void) {
	
}

BEGIN_EVENT_TABLE(tab_simple, abstract_panel)
	EVT_SLIDER(wxID_ANY,            tab_simple::OnSliderMove)
	EVT_TEXT(wxID_ANY,              tab_simple::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          tab_simple::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       tab_simple::OnRadioButtonToggle)
	EVT_IDLE(                       tab_simple::update)
END_EVENT_TABLE()


