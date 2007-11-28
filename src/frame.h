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

#ifndef __PROJECT2FRM_h__
#define __PROJECT2FRM_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

#include <wx/panel.h>
#include <wx/notebook.h>
#include "tabs.h"
#undef Project2Frm_STYLE
#define Project2Frm_STYLE wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX


class settings_frame : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		settings_frame(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = Project2Frm_STYLE);
		virtual ~settings_frame();
		
	private:
		tab_simple* simpleTab;
//		wxPanel *Debug;
//		wxPanel *Mouse;
//		wxPanel *Audio;
//		wxPanel *General;
//		wxPanel *Video;
//		wxPanel *Rendering;
//		
		abstract_panel* detailTab;
		abstract_panel* qualityTab;
		
		wxPanel panelArray[6];
		
		wxNotebook *Options;
		
		void initMenuBar();
		void handleExit();
		enum
		{
			////GUI Enum Control ID Start
			ID_UI = 1013,
			ID_QUALITY_VIDEO = 1012,
			ID_RENDER_DETAIL = 1011,
			ID_SIMPLE = 1010,
			ID_DEBUG = 1009,
			ID_MOUSE = 1008,
			ID_AUDIO = 1007,
			ID_GENERAL = 1006,
			ID_VIDEO = 1005,
			ID_RENDERING = 1004,
			ID_OPTIONS = 1003,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		void OnClose(wxCloseEvent& event);
		void OnMenuChoice(wxCommandEvent& event);
		void CreateGUIControls();
};

#endif
