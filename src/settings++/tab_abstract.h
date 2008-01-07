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

#ifndef __TAB_ABSTRACT_h__
#define __TAB_ABSTRACT_h__


//#include <wx/frame.h>
#include <wx/scrolwin.h>

#include <map>
class wxString;
class wxCommandEvent;

class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxSpinEvent;


typedef std::map<wxString,int> intMap;
typedef std::map<wxString,wxString> stringMap;
typedef std::map<wxString,float> floatMap;

class abstract_panel : public wxScrolledWindow
{

	public:
		abstract_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), 
				const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~abstract_panel();

		void OnSliderMove(wxCommandEvent&);
		void OnTextUpdate(wxCommandEvent&);
		void OnCheckBoxTick(wxCommandEvent&);
		void OnRadioButtonToggle(wxCommandEvent&);
		//void update(wxIdleEvent&);
		void OnComboBoxChange(wxCommandEvent& event);
		static intMap intSettings;
		//static stringMap stringSettings;
		//static floatMap floatSettings;
		static bool settingsChanged;
		static bool saveSettings();

		static void loadDefaults();
		virtual void updateControls(int what_to_update);
		static bool loadValuesIntoMap();
		void OnSpinControlChange(wxSpinEvent& event);
    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
				
    private:
    	
		
		DECLARE_EVENT_TABLE()
};

#endif
