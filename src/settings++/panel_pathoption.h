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
#ifndef PANEL_PATHOPTION_H_
#define PANEL_PATHOPTION_H_

#include <wx/panel.h>

class wxWindow;
class wxButton;
class wxBoxSizer;
class wxStaticBoxSizer;
class wxTextCtrl;
class wxStaticText;
class wxFlexGridSizer;
class settings_frame;

/** \brief only used in standalone mode to get usync location if not immediately found */
class PathOptionPanel : public wxPanel
{

public:
	PathOptionPanel(wxWindow*,settings_frame*);
	virtual ~PathOptionPanel();
	void SetUsyncPath(wxCommandEvent& );
	void UsePaths(wxCommandEvent& );

private:
	    wxStaticText* usync_loc_lbl;
	    wxStaticText* explanation_text;

	    wxButton* paths_ok_btn;
	    wxButton* usync_browse_btn;

	    wxTextCtrl* usync_ctrl;
	    settings_frame* origin;
	    wxFlexGridSizer* usync_sizer;

	    wxBoxSizer* parentSizer;
	    wxStaticBoxSizer* main_sizer;

	    DECLARE_EVENT_TABLE()
};

#endif /*PANEL_PATHOPTION_H_*/
