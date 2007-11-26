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

#ifndef __TAB_UI_h__
#define __TAB_UI_h__
#include "tabs.h"

class tab_ui : public abstract_panel
{

	public:
		tab_ui(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~tab_ui();

		void initCameraSizer(wxStaticBoxSizer*);
		void initScrollSpeedSizer(wxStaticBoxSizer* );
		void initUiOptSizer(wxStaticBoxSizer* );

    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};

#endif
