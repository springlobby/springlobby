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

#include "frame.h"
#include "se_settings.h"
#include "../springunitsynclib.h"

BEGIN_EVENT_TABLE(settings_frame,wxFrame)
	EVT_CLOSE(settings_frame::OnClose)
	EVT_MENU(wxID_ANY,settings_frame::OnMenuChoice)
END_EVENT_TABLE()

//TODO use icon
settings_frame::settings_frame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
	initMenuBar();
	//TODO call only when standalone
	//susynclib()->Load(OptionsHandler->getUsyncLoc());
}

settings_frame::~settings_frame()
{
}

void settings_frame::CreateGUIControls()
{
	notebook = new wxNotebook(this, ID_OPTIONS, wxPoint(0,0),TAB_SIZE, wxNB_TOP);
	notebook->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Tahoma")));
	try {
	simpleTab = new tab_simple(notebook,ID_SIMPLE);
		
	qualityTab = new tab_quality_video(notebook,ID_QUALITY_VIDEO);
       
    detailTab = new tab_render_detail(notebook,ID_RENDER_DETAIL);
   
    uiTab = new tab_ui(notebook,ID_UI);
    notebook->AddPage(uiTab, wxT("UI Options"));

    audioTab = new audio_panel(notebook,ID_AUDIO);
    
	debugTab = new debug_panel(notebook,ID_DEBUG);
	
	simpleTab->setTabs(detailTab,qualityTab);

	switch(OptionsHandler.getMode()){
		case SET_MODE_EXPERT: {
			notebook->AddPage(qualityTab, wxT("Render Quality / Video Mode"));
			notebook->AddPage(detailTab, wxT("Render Detail"));
			notebook->AddPage(audioTab, wxT("Audio"));
			notebook->AddPage(debugTab, wxT("Debug"));
		}
			break;
		case SET_MODE_SIMPLE: {
			notebook->InsertPage(0,simpleTab,wxT("SIMPLE"));
		}
		break;
	}
	} catch (...) {
		wxMessageBox(wxT("DOH. unitsync not loaded. closing..."), wxT(""), wxOK|wxICON_HAND, this);
		Destroy();
	}
	notebook->SetSelection(0);
	SetTitle(wxT("Settings++"));
	SetIcon(wxNullIcon);
	SetSize(8,8,760,550);
	Center();

}

void settings_frame::initMenuBar() {
	menuFile = new wxMenu();

	menuFile->Append(ID_MENUITEM_SAVE, wxT("Save settings"));
	menuFile->Append(ID_MENUITEM_RESET, wxT("Reset settings to default values"));
	menuFile->AppendCheckItem(ID_MENUITEM_DISABLE_WARN, wxT("disable expert mode warning"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_MENUITEM_QUIT, wxT("Quit"));

	menuFile->Check(ID_MENUITEM_DISABLE_WARN,OptionsHandler.getDisableWarning());
	
	menuMode = new wxMenu();
	menuMode->AppendRadioItem(ID_MENUITEM_SIMPLE,wxT("simple (few options)"));
	menuMode->AppendRadioItem(ID_MENUITEM_EXPERT,wxT("expert (all options"));
        
	switch(OptionsHandler.getMode()){
	case SET_MODE_EXPERT: {
			menuMode->Check(ID_MENUITEM_EXPERT,true);
		}
			break;
		case SET_MODE_SIMPLE: {
			menuMode->Check(ID_MENUITEM_SIMPLE,true);
		}
		break;
	}
	
	//wxMenu* menuMode = new wxMenu();
	
	
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, wxT("File"));
	menuBar->Append(menuMode, wxT("Mode"));

	SetMenuBar(menuBar);
}

void settings_frame::handleExit() {
    if (abstract_panel::settingsChanged) {
    	int action = wxMessageBox(wxT("Save settings before exiting?"), wxT(""), wxYES_NO|wxCANCEL, this);
        switch (action) {
        case wxYES:
        	if (abstract_panel::saveSettings())
        				 (abstract_panel::settingsChanged) = false;
        case wxNO:
        	OptionsHandler.save();
        	    Destroy();
        	    break;
        case wxCANCEL:
        	break;
        }
    	
    }
    else
    {
    	OptionsHandler.save();
    	Destroy();
    }
    
    
}

void settings_frame::OnMenuChoice(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_MENUITEM_SAVE: {
			if (abstract_panel::saveSettings())
			 (abstract_panel::settingsChanged) = false;
		} break;

		case ID_MENUITEM_QUIT: {
			handleExit();
		} break;

		case ID_MENUITEM_RESET: {
			if ((wxMessageBox(wxT("Reset ALL settings to default values?"), wxT(""), wxYES_NO, this)) == wxYES) {
						resetSettings();
			}
		} break;
		case ID_MENUITEM_SIMPLE: {
			if (OptionsHandler.getMode()==SET_MODE_EXPERT) {
				OptionsHandler.setMode(SET_MODE_SIMPLE);
				notebook->InsertPage(0,simpleTab,wxT("SIMPLE"));
				notebook->RemovePage(5);
				notebook->RemovePage(4);
				notebook->RemovePage(3);
				notebook->RemovePage(2);
				SetTitle(wxT("Settings++ (simple mode)"));
				updateAllControls();
				if (!OptionsHandler.getDisableWarning()){
					wxMessageBox(wxT("Changes made on Quality/Detail tab in expert mode"
							"\n will be lost if you change simple options again."), wxT(""), wxOK, this);
				}	
			}
		} break;
		case ID_MENUITEM_EXPERT: {
			if (OptionsHandler.getMode()==SET_MODE_SIMPLE) {
				OptionsHandler.setMode(SET_MODE_EXPERT);
				notebook->AddPage(qualityTab, wxT("Render Quality / Video Mode"));
				notebook->AddPage(detailTab, wxT("Render Detail"));
				notebook->AddPage(audioTab, wxT("Audio"));
				notebook->AddPage(debugTab, wxT("Debug"));
				notebook->RemovePage(0);
				SetTitle(wxT("Settings++ (expert mode)"));
				updateAllControls();
			}
		} break;
		case ID_MENUITEM_DISABLE_WARN:{
			OptionsHandler.setDisableWarning(menuFile->IsChecked(ID_MENUITEM_DISABLE_WARN));
		} break;
	}
}
void settings_frame::resetSettings()
{
	abstract_panel::loadDefaults();
	updateAllControls();
}

void settings_frame::updateAllControls()
{
	uiTab->updateControls(UPDATE_ALL);
	simpleTab->updateControls(UPDATE_ALL);
	detailTab->updateControls(UPDATE_ALL);
	qualityTab->updateControls(UPDATE_ALL);
	debugTab->updateControls(UPDATE_ALL);
	audioTab->updateControls(UPDATE_ALL);
}
void settings_frame::OnClose(wxCloseEvent& event)
{
	handleExit();
	Destroy();
}


