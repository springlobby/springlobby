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

#ifndef __TAB_ABSTRACT_h__
#define __TAB_ABSTRACT_h__
#include <map>
#include <string>
typedef std::map<std::string,int> intMap;
typedef std::map<std::string,std::string> stringMap;
typedef std::map<std::string,float> floatMap;

class abstract_panel : public wxPanel
{

	public:
		abstract_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~abstract_panel();

		void OnSliderMove(wxCommandEvent&);
		void OnTextUpdate(wxCommandEvent&);
		void OnCheckBoxTick(wxCommandEvent&);
		void OnRadioButtonToggle(wxCommandEvent&);
		void update(wxIdleEvent&);
		void OnComboBoxChange(wxCommandEvent& event);
		static intMap intSettings;
		static stringMap stringSettings;
		static floatMap floatSettings;
		static bool expertModeEnabled;
		static bool settingsChanged;
		static bool saveSettings();
		static void enableExpertMode(bool);
		static bool isExpertModeEnabled();

		static void loadDefaults();
		virtual void updateControls(int what_to_update);

        
    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		wxArrayString wxArrayStringFromCStringArray(const wxString* stdAr);
		
    private:
    	
		
		DECLARE_EVENT_TABLE()
};

#endif
