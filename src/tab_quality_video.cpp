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

void tab_quality_video::initVideoSizer(wxFlexGridSizer* sizer) {

	// i < "sizeof"(VO_CBOX)
	for (int i = 0; i < ctrl_vo_Boxes_size; i++) {
		ctrl_vo_Boxes[i] = new wxCheckBox(this, VO_CBOX[i].id, _S(VO_CBOX[i].lbl));
		ctrl_vo_Boxes[i]->SetValue(configHandler.GetInt(VO_CBOX[i].key,fromString<int>(VO_CBOX[i].def)));
		sizer->Add(ctrl_vo_Boxes[i], 0, wxTOP, (i == 0)? 10: 0);
	}

	ctrl_x_res = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_X, wxT(""), WX_DEF_P, wxSize(60, 20), 0);
	ctrl_y_res = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_Y, wxT(""), WX_DEF_P, wxSize(60, 20), 0);
	 std::string s;
	     toString<int>(s,configHandler.GetInt(RC_TEXT[0].key,fromString<int>(RC_TEXT[0].def))); 
	     ctrl_x_res->ChangeValue(_S(s.c_str()));
	     toString<int>(s,configHandler.GetInt(RC_TEXT[1].key,fromString<int>(RC_TEXT[1].def))); 
	     ctrl_y_res->ChangeValue(_S(s.c_str()));
	
	wxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);
	subSizer->Add(ctrl_x_res, 0, wxALIGN_LEFT, 10);
	subSizer->Add(5, 0, 0);
	subSizer->Add(new wxStaticText(this, -1, wxT("x")), 0, wxTOP | wxBOTTOM, 0);
	subSizer->Add(5, 0, 0);
	subSizer->Add(ctrl_y_res, 0, wxALIGN_RIGHT, 10);

	sizer->Add(new wxStaticText(this, -1, wxT("Screen Resolution")), 0, wxTOP , 15);
	sizer->Add(subSizer);

}

void tab_quality_video::updateControls()
{
//	
//	for (int i = 0; i < ctrl_vo_Boxes_size; i++) {
//		ctrl_vo_Boxes[i]->SetValue(intSettings[VO_CBOX[i].key]);
//	}
//	
//	
//	 std::string s;
//     toString<int>(s,intSettings[RC_TEXT[0].key]); 
//     ctrl_x_res->ChangeValue(_S(s.c_str()));
//     toString<int>(s,intSettings[RC_TEXT[1].key]); 
//     ctrl_y_res->ChangeValue(_S(s.c_str()));
     
     for (int i = 0; i < ctrl_qa_Boxes_size-3; i++) {
     		ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
     	}
     // QA_BOX[7] is left out (HIGH_RES_LOS_TEX)
//     for (int i = 8; i < ctrl_qa_Boxes_size; i++) {
//         		ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
//         	}
     
   
     int waterOptIndex;
     int waterSetting = intSettings[WR_COMBOX[0].key]; 
 	switch (waterSetting)
 	{
 		case 0:
 		case 1: waterOptIndex = waterSetting; break;
 		case 2: waterOptIndex = 3; break;
 		case 3: waterOptIndex = 2; break;
 	}
     ctrl_waterQ_CBox->SetValue(WR_COMBOX_CHOICES[waterOptIndex]);
     
     int useFSAA = intSettings[VO_SLI_EXT[0].key];
     int FSAALev = intSettings[VO_SLI[0].key];
     ctrl_fsaa_slider->SetValue((useFSAA == 1)? FSAALev: 0);
     
 	switch (intSettings[VO_RBUT[0].key]) {
 		case 16: { ctrl_z_radio1->SetValue(1); } break;
 		case 24: { ctrl_z_radio2->SetValue(1); } break;
 	}
}

void tab_quality_video::initQualitySizer(wxFlexGridSizer* sizer) {
	// i < 8 with High resolution LOS textures
	// i < 7 without
	for (int i = 0; i < ctrl_qa_Boxes_size-3; i++) {
		ctrl_qa_Boxes[i] = new wxCheckBox(this, QA_CBOX[i].id, _S(QA_CBOX[i].lbl));
		ctrl_qa_Boxes[i]->SetValue(configHandler.GetInt(QA_CBOX[i].key,fromString<int>(QA_CBOX[i].def)));
		sizer->Add(ctrl_qa_Boxes[i], 0, wxTOP, (i == 0)? 10: 0);
	}
	   int waterOptIndex;
	     int waterSetting = configHandler.GetInt(WR_COMBOX[0].key,fromString<int>(WR_COMBOX[0].def)); 
	 	switch (waterSetting)
	 	{
	 		case 0:
	 		case 1: waterOptIndex = waterSetting; break;
	 		case 2: waterOptIndex = 3; break;
	 		case 3: waterOptIndex = 2; break;
	 	}
	     
	
	sizer->Add(new wxStaticText(this, -1, wxT("Water Quality")), 0, wxTOP , 10);
	ctrl_waterQ_CBox = new wxComboBox(this, ID_WINDOWP_WR_COMBOX, WR_COMBOX_CHOICES[waterOptIndex], wxDefaultPosition, wxSize(220,21), 
			4,WR_COMBOX_CHOICES,wxCB_DROPDOWN|wxCB_READONLY);
	sizer->Add(ctrl_waterQ_CBox, 0, wxBOTTOM, 5);	
}

void tab_quality_video::initAASizer(wxFlexGridSizer* sizer){
    for (int i = 8; i < ctrl_qa_Boxes_size; i++) {
    	ctrl_qa_Boxes[i] = new wxCheckBox(this, QA_CBOX[i].id, _S(QA_CBOX[i].lbl));
    	ctrl_qa_Boxes[i]->SetValue(configHandler.GetInt(QA_CBOX[i].key,fromString<int>(QA_CBOX[i].def)));
		sizer->Add(ctrl_qa_Boxes[i], 0, wxTOP, (i == 8)? 10: 0);
	}
    int useFSAA = configHandler.GetInt(VO_SLI_EXT[0].key,fromString<int>(VO_SLI_EXT[0].def));
	int FSAALev = configHandler.GetInt(VO_SLI[0].key,fromString<int>(VO_SLI[0].def));
	ctrl_fsaa_slider = new wxSlider(this, VO_SLI[0].id, (useFSAA == 1)? FSAALev: 0, 0, 16, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
    sizer->Add(new wxStaticText(this, -1, _S(VO_SLI[0].lbl)), 0, wxTOP, 15);
	sizer->Add(ctrl_fsaa_slider, 0, wxALIGN_LEFT, 0);
}

void tab_quality_video::initZBufferSizer(wxFlexGridSizer* sizer)
{
    //z-buffer
	
	sizer->Add(new wxStaticText(this, -1, wxT("Resolution in bit")), 0, wxTOP ,15);
	
	ctrl_z_radio1 = new wxRadioButton(this, VO_RBUT[0].id, _S(VO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	ctrl_z_radio2 = new wxRadioButton(this, VO_RBUT[1].id, _S(VO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	switch (configHandler.GetInt(VO_RBUT[0].key,fromString<int>(VO_RBUT[0].def))) {
	 		case 16: { ctrl_z_radio1->SetValue(1); } break;
	 		case 24: { ctrl_z_radio2->SetValue(1); } break;
	 	}
	sizer->Add(ctrl_z_radio1, 0, wxTOP, 0);
	sizer->Add(ctrl_z_radio2, 0, wxTOP,  0);
    
}

tab_quality_video::tab_quality_video(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , 
		const wxSize& size, long style)
                : abstract_panel(parent, id , title , pos , size, style) {
	ctrl_qa_Boxes = new wxCheckBox*[ctrl_qa_Boxes_size];
	ctrl_vo_Boxes = new wxCheckBox*[ctrl_vo_Boxes_size];
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

	
	initQualitySizer(SizerA);
	initVideoSizer(SizerB);
	initAASizer(SizerC);
	initZBufferSizer(SizerD);
	
	
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
   
    SetSizer(parentSizer, true); // true --> delete old sizer if present
   
}

tab_quality_video::~tab_quality_video(void) {

}

BEGIN_EVENT_TABLE(tab_quality_video, abstract_panel)
	EVT_SLIDER(wxID_ANY,            tab_quality_video::OnSliderMove)
	EVT_TEXT(ID_RES_CHOICES_LBOX_X, tab_quality_video::OnTextUpdate)
	EVT_TEXT(ID_RES_CHOICES_LBOX_Y, tab_quality_video::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          tab_quality_video::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       tab_quality_video::OnRadioButtonToggle)
	EVT_IDLE(                       tab_quality_video::update)
	EVT_COMBOBOX(ID_WINDOWP_WR_COMBOX, 		tab_quality_video::OnComboBoxChange)
END_EVENT_TABLE()
