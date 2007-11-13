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
#include "main.h"
#include "frame.h"

IMPLEMENT_APP(SettingsPlusPlus)

bool SettingsPlusPlus::OnInit()
{
    settings_frame* frame = new settings_frame(NULL,wxID_ANY,wxT("Settings++"),wxDefaultPosition,
    		wxDefaultSize,wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN);
    SetTopWindow(frame);
    frame->Show();
    return true;
}
 
int SettingsPlusPlus::OnExit()
{
	return 0;
}
