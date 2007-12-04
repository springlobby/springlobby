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
#include "se_utils.h"

intMap abstract_panel::intSettings;
stringMap abstract_panel::stringSettings;
floatMap abstract_panel::floatSettings;
bool abstract_panel::settingsChanged = false;
    
abstract_panel::abstract_panel(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos , const wxSize& size, long style)
                : wxPanel(parent, id, pos, size, style,title) {
//	abstract_panel::expertModeEnadbled = false;
}

abstract_panel::~abstract_panel(void) {

}

void abstract_panel::loadDefaults()
{
	//const Control RO_SLI[9]
	for (int i = 0;i< 9; ++i)
		intSettings[RO_SLI[i].key] = fromString<int>( RO_SLI[i].def);

	//const Control VO_CBOX[3]
	for (int i = 0;i< 3; ++i)
		intSettings[VO_CBOX[i].key] = fromString<int>( VO_CBOX[i].def);	

	//const Control VO_RBUT[2] 
	for (int i = 0;i< 2; ++i)
		intSettings[VO_RBUT[i].key] = fromString<int>( VO_RBUT[i].def);	

	//	const Control VO_SLI[1] 
	for (int i = 0;i< 1; ++i)
		intSettings[VO_SLI[i].key] = fromString<int>( VO_SLI[i].def);	

	//	const Control VO_SLI_EXT[1]   
	for (int i = 0;i< 1; ++i)
		intSettings[VO_SLI_EXT[i].key] = fromString<int>( VO_SLI_EXT[i].def);	

//	const Control AO_SLI[3]       
	for (int i = 0;i< 3; ++i)
		intSettings[AO_SLI[i].key] = fromString<int>( AO_SLI[i].def);	

	//	const Control QA_CBOX[10]
	for (int i = 0;i< 10; ++i)
		intSettings[QA_CBOX[i].key] = fromString<int>( QA_CBOX[i].def);	

	//	const Control UI_CBOX[14] 
	for (int i = 0;i< 14; ++i)
		intSettings[UI_CBOX[i].key] = fromString<int>(UI_CBOX [i].def);	

	//	const Control MO_SLI[5]  
	for (int i = 0;i< 5; ++i)
		intSettings[MO_SLI[i].key] = fromString<int>( MO_SLI[i].def);

	//	const Control MO_SLI_EXT[5] 
	for (int i = 0;i< 5; ++i)
		intSettings[MO_SLI_EXT[i].key] = fromString<int>( MO_SLI_EXT[i].def);

	//	const Control DO_SLI[1]      
	for (int i = 0;i< 1; ++i)
		intSettings[DO_SLI[i].key] = fromString<int>( DO_SLI[i].def);

	//	const Control DO_CBOX[2]
	for (int i = 0;i< 2; ++i)
		intSettings[DO_CBOX[i].key] = fromString<int>( DO_CBOX[i].def);

	//	const Control WR_COMBOX[4] 
	for (int i = 0;i< 1; ++i)
		intSettings[WR_COMBOX[i].key] = fromString<int>( WR_COMBOX[i].def);

	//	const Control MO_CBOX[2] 
	for (int i = 0;i< 2; ++i)
		intSettings[MO_CBOX[i].key] = fromString<int>( MO_CBOX[i].def);

	//	const Control MO_RBUT[5]
	for (int i = 0;i< 5; ++i)
		intSettings[MO_RBUT[i].key] = fromString<int>(MO_RBUT [i].def);

	//	const Control RC_TEXT[2]
	for (int i = 0;i< 2; ++i)
		intSettings[RC_TEXT[i].key] = fromString<int>( RC_TEXT[i].def);


}

void abstract_panel::OnSliderMove(wxCommandEvent& event) {

    settingsChanged = true;

	wxSlider* slider = (wxSlider*) event.GetEventObject();

    int value = slider->GetValue();

	switch (event.GetId()) {
		case ID_RO_SLI_0: {
			// shadow-map sizes
			int val = slider->GetValue();

			if (val < 1536) {
				slider->SetValue(1024);
				(intSettings)[RO_SLI[0].key]=1024;
			}
			if (val > 1536 && val < 3072) {
				slider->SetValue(2048);
				(intSettings)[RO_SLI[0].key]=2048;
			}
			if (val > 3072 && val < 6144) {
				slider->SetValue(4096);
				(intSettings)[RO_SLI[0].key]=4096;
			}
			if (val > 6144) {
				slider->SetValue(8192);
				(intSettings)[RO_SLI[0].key]=8192;
			}
		} break;

		case ID_RO_SLI_1: { (intSettings)[RO_SLI[1].key]= value; } break;
		case ID_RO_SLI_2: { (intSettings)[RO_SLI[2].key]= value; } break;
		case ID_RO_SLI_3: { (intSettings)[RO_SLI[3].key]= value; } break;
		case ID_RO_SLI_4: { (intSettings)[RO_SLI[4].key]= value; } break;
		case ID_RO_SLI_5: { (intSettings)[RO_SLI[5].key]= value; } break;
		case ID_RO_SLI_6: { (intSettings)[RO_SLI[6].key]= value; } break;
		case ID_RO_SLI_7: { (intSettings)[RO_SLI[7].key]= value; } break;
		case ID_RO_SLI_8: { (intSettings)[RO_SLI[8].key]= value; } break;

		case ID_VO_SLI_0: {
			(intSettings)[VO_SLI_EXT[0].key]= (value > 0)? 1: 0;
			(intSettings)[VO_SLI[0].key]= value;
		} break;

		case ID_AO_SLI_0: { (intSettings)[AO_SLI[0].key]= value; } break;
		case ID_AO_SLI_1: { (intSettings)[AO_SLI[1].key]= value; } break;
		case ID_AO_SLI_2: { (intSettings)[AO_SLI[2].key]= value; } break;

		case ID_DO_SLI_0: { (intSettings)[DO_SLI[0].key]= value; } break;

		case ID_MO_SLI_0: { (intSettings)[MO_SLI[0].key]= value; } break;
        case ID_MO_SLI_1: { (intSettings)[MO_SLI[1].key]= value; } break;
        case ID_MO_SLI_2: { (intSettings)[MO_SLI[2].key]= value; } break;
        case ID_MO_SLI_3: { (intSettings)[MO_SLI[3].key]= value; } break;
        case ID_MO_SLI_4: { (intSettings)[MO_SLI[4].key]= value; } break;

	}
}


void abstract_panel::OnTextUpdate(wxCommandEvent& event) {

    settingsChanged = true;
    int eventID = event.GetId();
    
    if (eventID == ID_RES_CHOICES_LBOX_X || eventID == ID_RES_CHOICES_LBOX_Y)
    	    {
    		wxTextCtrl* textField = (wxTextCtrl*) event.GetEventObject();
    		wxString wxStr = textField->GetValue();
    	    long* res = new long; 
    	    bool success = (wxStr.ToLong(res));
    	    
    	    switch (eventID) {
    	    		case ID_RES_CHOICES_LBOX_X: {
    	    			// TODO: input validation?
    	    			if (success)
    	    			     (intSettings)[RC_TEXT[0].key]= int((*res));
    	    		} break;
    	    		case ID_RES_CHOICES_LBOX_Y: {
    	    			// TODO: input validation?
    	    			if (success)
    	    			     (intSettings)[RC_TEXT[1].key]= int((*res));
    	    		} break;
    	    	}
    }
	
	
}


void abstract_panel::OnCheckBoxTick(wxCommandEvent& event) {

    settingsChanged = true;

	wxCheckBox* checkBox = (wxCheckBox*) event.GetEventObject();
	int checked = checkBox->IsChecked();
	int id = event.GetId();

	switch (id) {
		case ID_WINDOWP_VO_CBOX_0:
		case ID_WINDOWP_VO_CBOX_1:
		case ID_WINDOWP_VO_CBOX_2: {
			int i = id - VO_CBOX[0].id;
			(intSettings)[VO_CBOX[i].key]= checked;
		} break;

		case ID_WINDOWP_QA_CBOX_0:
		case ID_WINDOWP_QA_CBOX_1:
		case ID_WINDOWP_QA_CBOX_2:
		case ID_WINDOWP_QA_CBOX_3:
		case ID_WINDOWP_QA_CBOX_4:
		case ID_WINDOWP_QA_CBOX_5:
		case ID_WINDOWP_QA_CBOX_6:
		case ID_WINDOWP_QA_CBOX_7:{
			int i = id - QA_CBOX[0].id;
			(intSettings)[QA_CBOX[i].key]= checked;
		} break;
            
        //case ID_WINDOWP_UI_CBOX_0:
		case ID_WINDOWP_UI_CBOX_1:
		case ID_WINDOWP_UI_CBOX_2:
		case ID_WINDOWP_UI_CBOX_3:
		case ID_WINDOWP_UI_CBOX_4:
		case ID_WINDOWP_UI_CBOX_5:
		case ID_WINDOWP_UI_CBOX_6:
		case ID_WINDOWP_UI_CBOX_7:
		case ID_WINDOWP_UI_CBOX_8:
		case ID_WINDOWP_UI_CBOX_9:
		case ID_WINDOWP_UI_CBOX_10:
		case ID_WINDOWP_UI_CBOX_11:
		case ID_WINDOWP_UI_CBOX_12:
		case ID_WINDOWP_UI_CBOX_13:
		case ID_WINDOWP_UI_CBOX_14: {
			int i = id - UI_CBOX[0].id;
			(intSettings)[UI_CBOX[i].key]= checked;
		} break;

		case ID_WINDOWP_DO_CBOX_0: { (intSettings)[DO_CBOX[0].key]= checked; } break;
		case ID_WINDOWP_DO_CBOX_1: { (intSettings)[DO_CBOX[1].key]= checked; } break;
		
		case ID_WINDOWC_MO_CBOX_0: { (intSettings)[MO_CBOX[0].key]= checked; } break;
		case ID_WINDOWC_MO_CBOX_1: { (intSettings)[MO_CBOX[1].key]= checked; } break;
	}
}


void abstract_panel::OnRadioButtonToggle(wxCommandEvent& event) {

    settingsChanged = true;

	wxRadioButton* radioButton = (wxRadioButton*) event.GetEventObject();
	radioButton->GetValue();

	switch (event.GetId()) {
		case ID_WINDOWP_VO_RBUT_0: { (intSettings)[VO_RBUT[0].key]= 16; } break;
		case ID_WINDOWP_VO_RBUT_1: { (intSettings)[VO_RBUT[0].key]= 24; } break;

		case ID_WINDOWC_MO_RBUT_0: { (intSettings)[MO_RBUT[0].key]= 1; } break;	// OH button (CamMode 1)
		case ID_WINDOWC_MO_RBUT_1: { (intSettings)[MO_RBUT[1].key]= 2; } break;	// ROH button (CamMode 2)
		case ID_WINDOWC_MO_RBUT_2: { (intSettings)[MO_RBUT[2].key]= 3; } break;	// TW button (CamMode 3)
		case ID_WINDOWC_MO_RBUT_3: { (intSettings)[MO_RBUT[3].key]= 0; } break;	// FPS button (CamMode 0)
		case ID_WINDOWC_MO_RBUT_4: { (intSettings)[MO_RBUT[4].key]= 4; } break;	// FC button (CamMode 4)
	}
}

void abstract_panel::OnComboBoxChange(wxCommandEvent& event) {

    settingsChanged = true;

	wxComboBox* comboBox = (wxComboBox*) event.GetEventObject();
	const wxString choice = comboBox->GetValue();
			
	switch (event.GetId())
	{
		case ID_WINDOWP_WR_COMBOX:
		{
			int choiceIndex=0;
			for (int i =1; i<4;++i)
			{
				if (choice==WR_COMBOX_CHOICES[i])
					choiceIndex = i;
			}
			switch (choiceIndex) {
				case 0: { (intSettings)[WR_COMBOX[0].key]= 0; } break;	// Basic
				case 1: { (intSettings)[WR_COMBOX[0].key]= 1; } break;	// Refl
				case 2: { (intSettings)[WR_COMBOX[0].key]= 3; } break;	// Refl + Refr
				case 3: { (intSettings)[WR_COMBOX[0].key]= 2; } break;	// Dyna
			}
			break;
		}
		case ID_SIMPLE_QUAL_CBX:
		{
			for (int i=0; i<prVal_RenderQuality_size;++i)
			{
				presetValues<int,5> pop = prVal_RenderQuality[i]; 
				 int k = (pop.values[choice]);
				 
				(intSettings)[prVal_RenderQuality[i].key]= k;
			}
			break;
		}
		case ID_SIMPLE_DETAIL_CBX:
		{
			for (int i=0; i<prVal_RenderDetail_size;++i)
						{
							presetValues<int,3> pop = prVal_RenderDetail[i]; 
							 int k = (pop.values[choice]);
							(intSettings)[prVal_RenderDetail[i].key]= k;
						}
			break;
		}
		case ID_SIMPLE_MODE_CBX:
		{
			int modeIndex=-1;
			for (int i=0; i<vl_Resolution_Str_size;++i)
			{
				if (choice == vl_Resolution_Str[i])
					modeIndex = i;
			}
			if (modeIndex!=-1)
			{
				(intSettings)[wxT("XResolution")] = vl_Resolution_X[modeIndex];
				(intSettings)[wxT("YResolution")] = vl_Resolution_Y[modeIndex];
			}
			break;
		}
	}
}

//TODO inquire about floatsettings
bool abstract_panel::saveSettings() {
    try {
	    for (intMap::iterator i = intSettings.begin(); i != intSettings.end();++i)
	    {
	        configHandler->SetSpringConfigInt(i->first,i->second);
	    }
	    for (stringMap::iterator s = stringSettings.begin(); s != stringSettings.end();++s)
	    {
	    	//not used
	        //configHandler->SetSpringConfigString(s->first,s->second);
	    }
	    for (floatMap::iterator f = floatSettings.begin(); f != floatSettings.end();++f)
	    {
	        // not used
	        //configHandler->SetSpringConfigFloat(f->first,f->second);
	    }
    } catch (...) {
    	wxMessageBox(_T("Could not save, unitsync not properly loaded"), wxT(""), wxOK|wxICON_HAND, 0);
    	return false;
    }
    //test ???
    return true; 
}

//TODO what good is this actually
void abstract_panel::update(wxIdleEvent& event) {
	
}

void abstract_panel::updateControls(int what_to_update)
{}

BEGIN_EVENT_TABLE(abstract_panel, wxPanel)
	EVT_SLIDER(wxID_ANY,            abstract_panel::OnSliderMove)
	EVT_TEXT(wxID_ANY,              abstract_panel::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          abstract_panel::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       abstract_panel::OnRadioButtonToggle)
	EVT_IDLE(                       abstract_panel::update)
	EVT_COMBOBOX(wxID_ANY, 		abstract_panel::OnComboBoxChange)
END_EVENT_TABLE()
