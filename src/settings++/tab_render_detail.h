/**
    This file is part of springsettings,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __TAB_RENDER_DETAIL_h__
#define __TAB_RENDER_DETAIL_h__

#include "tab_abstract.h"

class wxSlider;
class wxFlexGridSizer;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxStaticBoxSizer;

class tab_render_detail : public abstract_panel
{

	public:
		tab_render_detail(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~tab_render_detail();

		void initRendererSizer(wxFlexGridSizer*,wxFlexGridSizer*);
		void updateControls(int what_to_update);
    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();

		wxSlider** ctrl_detail_sliders;
		static const int ctrl_detail_sliders_size = 8;

		wxSizer* renderSizer ;
					wxSizer* parentSizer ;
					wxFlexGridSizer* rendererSizerA ;
					wxFlexGridSizer* rendererSizerB ;
					wxStaticBoxSizer* box ;

		DECLARE_EVENT_TABLE()
};

#endif
