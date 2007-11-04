/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (René Milk)
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
#include "tabs.h"

BEGIN_EVENT_TABLE(settings_frame,wxFrame)
	EVT_CLOSE(settings_frame::OnClose)
	EVT_MENU(wxID_ANY,settings_frame::OnMenuChoice)
END_EVENT_TABLE()

settings_frame::settings_frame(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
	initMenuBar();
}

settings_frame::~settings_frame()
{
}

void settings_frame::CreateGUIControls()
{

	Options = new wxNotebook(this, ID_OPTIONS, wxPoint(0,0),TAB_SIZE, wxNB_TOP);
	Options->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Tahoma")));

//	Simple = new wxPanel(Options, ID_SIMPLE, wxPoint(4,24), wxSize(724,552));
//	Simple->SetFont(wxFont(8, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Tahoma")));
//	Simple->Add(new wxString(wxT("nothing here yet")));
//	Options->AddPage(Simple, wxT("Simple"));

	Options->AddPage(new rendering_panel(Options,ID_RENDERING), wxT("Rendering"));

	Options->AddPage(new video_panel(Options,ID_VIDEO), wxT("Video"));

	Options->AddPage(new general_panel(Options, ID_GENERAL), wxT("General"));

	Options->AddPage(new audio_panel(Options,ID_AUDIO), wxT("Audio"));

	Options->AddPage(new mouse_panel(Options, ID_MOUSE), wxT("Mouse"));

	Options->AddPage(new debug_panel(Options,ID_DEBUG), wxT("Debug"));

	SetTitle(wxT("Settings++"));
	SetIcon(wxNullIcon);
	SetSize(8,8,520,560);
	Center();

}

void settings_frame::initMenuBar() {
	wxMenu* menuFile = new wxMenu();

	menuFile->Append(ID_MENUITEM_SAVE, wxT("Save settings"));
	menuFile->Append(ID_MENUITEM_RESET, wxT("Reset settings to default values"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_MENUITEM_QUIT, wxT("Quit"));

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, wxT("File"));

	SetMenuBar(menuBar);
}

void settings_frame::OnMenuChoice(wxCommandEvent& event) {
	switch (event.GetId()) {
		case ID_MENUITEM_SAVE: {
			if (abstract_panel::saveSettings())
			 (abstract_panel::settingsChanged) = false;
			    
			 
		} break;

		case ID_MENUITEM_QUIT: {
			//handleExit();
		} break;

		case ID_MENUITEM_RESET: {
			//dialog->Show(true);
		} break;
	}
}

void settings_frame::OnClose(wxCloseEvent& event)
{
	Destroy();
}
