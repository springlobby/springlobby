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
#include <wx/icon.h>

#include "frame.h"
#include "../settings.h"
#include "../springunitsynclib.h"
#include "tab_render_detail.h"
#include "tab_quality_video.h"
#include "tab_abstract.h"
#include "tab_audio.h"
#include "tab_ui.h"
#include "tab_simple.h"
#include "Defs.hpp"
#include "panel_pathoption.h"
#include "custom_dialogs.h"
#include "../images/springsettings.xpm"
#include "helpmenufunctions.h"
#include "se_utils.h"

const wxString simpleTabCap= _("Combined Options");
const wxString qualityTabCap= _("Render quality / Video mode");
const wxString detailTabCap = _("Render detail");
const wxString uiTabCap= _("UI options");
const wxString audioTabCap = _("Audio");
const wxString expertModeWarning = _("Changes made on Quality/Detail tab in expert mode"
									"\n will be lost if you change simple options again.\n"
									"Also these changes WILL NOT be reflected by the \n"
									"selected choices on the Combined options tab.\n"
									"(this message can be disabled in the \"File\" menu)");

BEGIN_EVENT_TABLE(settings_frame,wxFrame)
	EVT_CLOSE(settings_frame::OnClose)
	EVT_MENU(wxID_ANY,settings_frame::OnMenuChoice)
END_EVENT_TABLE()

settings_frame::settings_frame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& pa_size)
: wxFrame(parent, id, title, position, pa_size)
{
	alreadyCalled = false;
	parentWindow = parent;

	if ( !susynclib().IsLoaded() ) loadUnitsync();

	notebook = new wxNotebook(this, ID_OPTIONS, wxPoint(0,0),TAB_SIZE, wxNB_TOP|wxNB_NOPAGETHEME);
	notebook->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, _T("Tahoma")));

	settingsIcon  = new wxIcon(springsettings_xpm);

    if (abstract_panel::loadValuesIntoMap())
	{
		CreateGUIControls();
		initMenuBar();
	}
	else
	{
		notebook->AddPage(new PathOptionPanel(notebook,this),_("Error!"));
		SetTitle(_T("SpringSettings"));
	}

     SetIcon(*settingsIcon);
     wxString name = _T("SETTINGSFRAME");
     wxPoint pos = sett().GetWindowPos( name, wxPoint( DEFSETT_SW_LEFT, DEFSETT_SW_TOP ) );
     wxSize size = sett().GetWindowSize( name, wxSize( DEFSETT_SW_WIDTH, DEFSETT_SW_HEIGHT ) );
     SetSize( pos.x , pos.y, size.GetWidth(), size.GetHeight() );
     Layout();
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
		notebook->AddPage(new PathOptionPanel(notebook,this),_("Error!"));
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
		{
			int choice = customMessageBox(SS_MAIN_ICON,_("Save Spring settings before exiting?"), _("Confirmation needed"), wxYES|wxNO |wxICON_QUESTION);
			if ( choice == wxYES)
			{
				abstract_panel::saveSettings();
				if (simpleTab!=0)
					simpleTab->saveCbxChoices();
			}
		}
	}

	sett().SaveSettings();
}

void settings_frame::handleExit() {
    if (abstract_panel::settingsChanged)
    {
    	int action = customMessageBox(SS_MAIN_ICON,_("Save Spring settings before exiting?"), _("Confirmation needed"),wxYES_NO|wxCANCEL|wxICON_QUESTION );
        switch (action) {
        case wxYES:
        	if (abstract_panel::saveSettings())
        				 (abstract_panel::settingsChanged) = false;
        	if (simpleTab!=0)
        			simpleTab->saveCbxChoices();
        case wxNO:
	        	sett().SaveSettings();
        	    Destroy();
        	break;

        case wxCANCEL:
        default:
        	break;
        }
    }
    else
    {
    	sett().SaveSettings();
    	Destroy();
    }
}

void settings_frame::CreateGUIControls()
{
	switch(sett().getMode()){
					case SET_MODE_EXPERT:

								qualityTab = new tab_quality_video(notebook,ID_QUALITY_VIDEO);
							    detailTab = new tab_render_detail(notebook,ID_RENDER_DETAIL);
							    uiTab = new tab_ui(notebook,ID_UI);
							    audioTab = new audio_panel(notebook,ID_AUDIO);
                                simpleTab = 0;
								notebook->AddPage(uiTab, uiTabCap);
								notebook->AddPage(qualityTab, qualityTabCap);
								notebook->AddPage(detailTab, detailTabCap);
								notebook->AddPage(audioTab,audioTabCap);

						break;
                    default:
					case SET_MODE_SIMPLE:
						simpleTab = new tab_simple(this,notebook,ID_SIMPLE);
						 uiTab = new tab_ui(notebook,ID_UI);
						notebook->AddPage(simpleTab,simpleTabCap);
						notebook->AddPage(uiTab, uiTabCap);
					break;
			}
			notebook->SetSelection(0);

	if (sett().getMode()==SET_MODE_EXPERT)
		SetTitle(_("SpringSettings (expert mode)"));
	else
		SetTitle(_("SpringSettings (simple mode)"));

	abstract_panel::settingsChanged = false;

}

void settings_frame::initMenuBar() {
	menuFile = new wxMenu();

	menuFile->Append(ID_MENUITEM_SAVE, _("Save settings"));
	menuFile->Append(ID_MENUITEM_RESET, _("Reset settings to default values"));
	menuFile->AppendCheckItem(ID_MENUITEM_DISABLE_WARN, _("Disable expert mode warning"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_MENUITEM_QUIT, _("Quit"));

	menuFile->Check(ID_MENUITEM_DISABLE_WARN,sett().getDisableWarning());

	menuMode = new wxMenu();
	menuMode->AppendRadioItem(ID_MENUITEM_SIMPLE,_("Simple (few options)"));
	menuMode->AppendRadioItem(ID_MENUITEM_EXPERT,_("Expert (all options"));

	menuHelp = new wxMenu();
	menuHelp->Append(ID_MENUITEM_ABOUT,_("About"));
	menuHelp->Append(ID_MENUITEM_CONTACT, _("Contact"));
	menuHelp->Append(ID_MENUITEM_CREDITS, _("Credits"));
	menuHelp->Append(ID_MENUITEM_BUGREPORT, _("Report a bug"));

	switch(sett().getMode()){
	case SET_MODE_EXPERT: {
			menuMode->Check(ID_MENUITEM_EXPERT,true);
		}
			break;
        default:
		case SET_MODE_SIMPLE: {
			menuMode->Check(ID_MENUITEM_SIMPLE,true);
		}
		break;
	}
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, _("File"));
	menuBar->Append(menuMode, _("Mode"));
	menuBar->Append(menuHelp, _("Info/Help"));

	//TODO PROFILES!!!

	SetMenuBar(menuBar);
}

void settings_frame::OnMenuChoice(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_MENUITEM_SAVE:
			if (abstract_panel::saveSettings())
			 (abstract_panel::settingsChanged) = false;
			if (simpleTab!=0)
        			simpleTab->saveCbxChoices();
		 break;

		case ID_MENUITEM_QUIT:
				handleExit();
		 break;

		case ID_MENUITEM_RESET:
			if ((customMessageBox(SS_MAIN_ICON,_("Reset ALL settings to default values?"), _("Confirmation needed"), wxYES_NO)) == wxYES) {
						resetSettings();
			}
		 break;

		case ID_MENUITEM_SIMPLE:
			if (sett().getMode()==SET_MODE_EXPERT)
				sett().setMode(SET_MODE_SIMPLE);

				simpleTab = new tab_simple(this,notebook,ID_SIMPLE);
				notebook->InsertPage(0,simpleTab,simpleTabCap);
				simpleTab->updateControls(UPDATE_ALL);

				//if not on ui page goto simple
				if (notebook->GetSelection()!=1)
					notebook->SetSelection(0);

				notebook->DeletePage(4);
				notebook->DeletePage(3);
				notebook->DeletePage(2);
				qualityTab = 0;
				detailTab = 0;
				audioTab = 0;

				SetTitle(_("SpringSettings (simple mode)"));
				if (!sett().getDisableWarning()){
					customMessageBox(SS_MAIN_ICON,expertModeWarning, _("Hint"), wxOK);
				}
		  break;

		case ID_MENUITEM_EXPERT:
			if (sett().getMode()==SET_MODE_SIMPLE) {
				switchToExpertMode();
			}
			break;
		case ID_MENUITEM_DISABLE_WARN:
			sett().setDisableWarning(menuFile->IsChecked(ID_MENUITEM_DISABLE_WARN));
			break;

		case ID_MENUITEM_ABOUT:
			showAbout();
			break;
		case ID_MENUITEM_CREDITS:
			showCredits();
			break;
		case ID_MENUITEM_CONTACT:
			openContactPage();
			break;
		case ID_MENUITEM_BUGREPORT:
			openNewTicket();
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
	sett().setMode(SET_MODE_EXPERT);
	menuMode->Check(ID_MENUITEM_EXPERT,true);

	qualityTab = new tab_quality_video(notebook,ID_QUALITY_VIDEO);
    	detailTab = new tab_render_detail(notebook,ID_RENDER_DETAIL);
    	audioTab = new audio_panel(notebook,ID_AUDIO);
	notebook->AddPage(qualityTab, qualityTabCap);
	notebook->AddPage(detailTab, detailTabCap);
	notebook->AddPage(audioTab,audioTabCap);

	notebook->DeletePage(0);
	simpleTab = 0;
	SetTitle(_("SpringSettings (expert mode)"));
	uiTab->updateControls(UPDATE_ALL);
	detailTab->updateControls(UPDATE_ALL);
	qualityTab->updateControls(UPDATE_ALL);
	audioTab->updateControls(UPDATE_ALL);
}

void settings_frame::updateAllControls()
{
	if (uiTab)
		uiTab->updateControls(UPDATE_ALL);
	if (simpleTab!=0)
		simpleTab->updateControls(UPDATE_ALL);
	if (detailTab)
		detailTab->updateControls(UPDATE_ALL);
	if (qualityTab)
		qualityTab->updateControls(UPDATE_ALL);
	if (audioTab)
		audioTab->updateControls(UPDATE_ALL);
}
void settings_frame::OnClose(wxCloseEvent& /*unused*/)
{
	if ( !alreadyCalled ){
        wxString name = _T("SETTINGSFRAME");
        sett().SetWindowSize( name, GetSize() );
        sett().SetWindowPos( name, GetPosition() );
        sett().SaveSettings();
        handleExit();
	}
}


