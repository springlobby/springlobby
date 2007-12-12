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

#include <wx/notebook.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/icon.h>

#include "frame.h"
#include "se_settings.h"
#include "../springunitsynclib.h"
#include "tab_render_detail.h"
#include "tab_quality_video.h"
#include "tab_abstract.h"
#include "tab_audio.h"
#include "tab_debug.h"
#include "tab_ui.h"
#include "tab_simple.h"
#include "Defs.hpp"
#include "panel_pathoption.h"

#include "../images/springsettings.xpm"

BEGIN_EVENT_TABLE(settings_frame,wxFrame)
	EVT_CLOSE(settings_frame::OnClose)
	EVT_MENU(wxID_ANY,settings_frame::OnMenuChoice)
END_EVENT_TABLE()

settings_frame::settings_frame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	alreadyCalled = false;
	parentWindow = parent;
	wxSetWorkingDirectory(OptionsHandler.getSpringDir());
	susynclib()->Load(OptionsHandler.getUsyncLoc());
	
	notebook = new wxNotebook(this, ID_OPTIONS, wxPoint(0,0),TAB_SIZE, wxNB_TOP|wxNB_NOPAGETHEME);
	notebook->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Tahoma")));
	
	SetIcon( wxIcon(springsettings_xpm) );
	
	
	 if (abstract_panel::loadValuesIntoMap())
	{
		CreateGUIControls();
		initMenuBar();
	}
	else
	{
		notebook->AddPage(new PathOptionPanel(notebook,this),_T("Error!"));
		SetTitle(wxT("SpringSettings"));
	}
	 
	 SetSize(8,8,760,550);
	 Center();
}

void settings_frame::buildGuiFromErrorPanel()
{
	notebook->DeletePage(0);
	
	//to be safe we'll try again
	if (abstract_panel::loadValuesIntoMap())
	{
		CreateGUIControls();
		initMenuBar();
	}
	else
	{
		notebook->AddPage(new PathOptionPanel(notebook,this),_T("Error!"));
	}
}

settings_frame::~settings_frame()
{
	
}

void settings_frame::handleExternExit()
{
	if ( !alreadyCalled){
		alreadyCalled = true;
		if (abstract_panel::settingsChanged)
		{	//TODO use custom frame for to set icon
			int choice = wxMessageBox(wxT("Save Spring settings before exiting?"), wxT("Confirmation needed"), wxYES_NO |wxICON_QUESTION,NULL);	
			if ( choice == wxYES)
						  abstract_panel::saveSettings();	
			//abstract_panel::settingsChanged = false;   
		}
		
	}
	OptionsHandler.save();
}

void settings_frame::handleExit() {
    if (abstract_panel::settingsChanged) 
    {
    	int action = wxMessageBox(wxT("Save Spring settings before exiting?"), wxT("Confirmation needed"),wxYES_NO|wxCANCEL|wxICON_QUESTION , this);
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

void settings_frame::CreateGUIControls()
{
	switch(OptionsHandler.getMode()){
					case SET_MODE_EXPERT: 
						
								qualityTab = new tab_quality_video(notebook,ID_QUALITY_VIDEO);
							    detailTab = new tab_render_detail(notebook,ID_RENDER_DETAIL);
							    uiTab = new tab_ui(notebook,ID_UI);
							    audioTab = new audio_panel(notebook,ID_AUDIO);
							    debugTab = new debug_panel(notebook,ID_DEBUG);
							    
								notebook->AddPage(uiTab, uiTabCap);
								notebook->AddPage(qualityTab, qualityTabCap);
								notebook->AddPage(detailTab, detailTabCap);					
								notebook->AddPage(audioTab,audioTabCap);
								notebook->AddPage(debugTab, debugTabCap);
					
						break;
					case SET_MODE_SIMPLE:
						simpleTab = new tab_simple(this,notebook,ID_SIMPLE);
						 uiTab = new tab_ui(notebook,ID_UI);
						notebook->AddPage(simpleTab,simpleTabCap);
						notebook->AddPage(uiTab, uiTabCap);					
					break;
			}
			notebook->SetSelection(0);
		
	if (OptionsHandler.getMode()==SET_MODE_EXPERT)
		SetTitle(wxT("SpringSettings (expert mode)"));
	else
		SetTitle(wxT("SpringSettings (simple mode)"));
	
	abstract_panel::settingsChanged = false; 

}

void settings_frame::initMenuBar() {
	menuFile = new wxMenu();

	menuFile->Append(ID_MENUITEM_SAVE, wxT("Save settings"));
	menuFile->Append(ID_MENUITEM_RESET, wxT("Reset settings to default values"));
	menuFile->AppendCheckItem(ID_MENUITEM_DISABLE_WARN, wxT("Disable expert mode warning"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_MENUITEM_QUIT, wxT("Quit"));

	menuFile->Check(ID_MENUITEM_DISABLE_WARN,OptionsHandler.getDisableWarning());
	
	menuMode = new wxMenu();
	menuMode->AppendRadioItem(ID_MENUITEM_SIMPLE,wxT("Simple (few options)"));
	menuMode->AppendRadioItem(ID_MENUITEM_EXPERT,wxT("Expert (all options"));
        
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
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, wxT("File"));
	menuBar->Append(menuMode, wxT("Mode"));
	
	//TODO PROFILES!!!
	
	SetMenuBar(menuBar);
}

void settings_frame::OnMenuChoice(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_MENUITEM_SAVE: 
			if (abstract_panel::saveSettings())
			 (abstract_panel::settingsChanged) = false;
		 break;

		case ID_MENUITEM_QUIT: 
				handleExit();
		 break;

		case ID_MENUITEM_RESET: 
			if ((wxMessageBox(wxT("Reset ALL settings to default values?"), wxT("Confirmation needed"), wxYES_NO, this)) == wxYES) {
						resetSettings();
			}
		 break;
		 
		case ID_MENUITEM_SIMPLE: 
			if (OptionsHandler.getMode()==SET_MODE_EXPERT) 
				OptionsHandler.setMode(SET_MODE_SIMPLE);
				
				simpleTab = new tab_simple(this,notebook,ID_SIMPLE);
				notebook->InsertPage(0,simpleTab,simpleTabCap);
				simpleTab->updateControls(UPDATE_ALL);
				
				notebook->DeletePage(5);
				notebook->DeletePage(4);
				notebook->DeletePage(3);
				notebook->DeletePage(2);
				qualityTab = 0;
				detailTab = 0;
				audioTab = 0;
				debugTab = 0;
				
				SetTitle(wxT("SpringSettings (simple mode)"));
				if (!OptionsHandler.getDisableWarning()){
					wxMessageBox(expertModeWarning, wxT(""), wxOK, this);
				}	
		  break;
		  
		case ID_MENUITEM_EXPERT: 
			if (OptionsHandler.getMode()==SET_MODE_SIMPLE) {
				switchToExpertMode();
			}
			break;
		case ID_MENUITEM_DISABLE_WARN:
			OptionsHandler.setDisableWarning(menuFile->IsChecked(ID_MENUITEM_DISABLE_WARN));
			break;
	}
}
void settings_frame::resetSettings()
{
	abstract_panel::loadDefaults();
	updateAllControls();
}

void settings_frame::switchToExpertMode()
{
	OptionsHandler.setMode(SET_MODE_EXPERT);
					
	qualityTab = new tab_quality_video(notebook,ID_QUALITY_VIDEO);
    detailTab = new tab_render_detail(notebook,ID_RENDER_DETAIL);
    audioTab = new audio_panel(notebook,ID_AUDIO);
    debugTab = new debug_panel(notebook,ID_DEBUG);
	notebook->AddPage(qualityTab, qualityTabCap);
	notebook->AddPage(detailTab, detailTabCap);
	notebook->AddPage(audioTab,audioTabCap);
	notebook->AddPage(debugTab, debugTabCap);
	
	notebook->DeletePage(0);
	simpleTab = 0;
	SetTitle(wxT("SpringSettings (expert mode)"));
	uiTab->updateControls(UPDATE_ALL);
	detailTab->updateControls(UPDATE_ALL);
	qualityTab->updateControls(UPDATE_ALL);
	debugTab->updateControls(UPDATE_ALL);
	audioTab->updateControls(UPDATE_ALL);
}

void settings_frame::updateAllControls()
{
	if (uiTab)
		uiTab->updateControls(UPDATE_ALL);
	if (simpleTab)
		simpleTab->updateControls(UPDATE_ALL);
	if (detailTab)
		detailTab->updateControls(UPDATE_ALL);
	if (qualityTab)
		qualityTab->updateControls(UPDATE_ALL);
	if (debugTab)
		debugTab->updateControls(UPDATE_ALL);
	if (audioTab)
		audioTab->updateControls(UPDATE_ALL);
}
void settings_frame::OnClose(wxCloseEvent& event)
{
	if ( !alreadyCalled){
		handleExit();
	}
}


