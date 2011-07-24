/**
    This file is part of springsettings,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __TAB_AUDIO_h__
#define __TAB_AUDIO_h__

#include "tab_abstract.h"


class wxSlider;
class wxStaticBoxSizer;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;

class audio_panel : public abstract_panel
{

	public:
		audio_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~audio_panel();

		void initAudioSizer(wxStaticBoxSizer*);
		void updateControls(int);
    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();

        wxSlider** ctrl_audio_sliders;
		static const int ctrl_audio_sliders_size = 6;

		wxSizer* parentSizer ;	// main window sizer (three columns)
        wxSizer* childLSizer;
        wxStaticBoxSizer* audioSizer;

		DECLARE_EVENT_TABLE()
};

#endif
