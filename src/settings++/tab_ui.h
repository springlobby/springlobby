/**
    This file is part of springsettings,
    Copyright (C) 2007-2010

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

#ifndef __TAB_UI_h__
#define __TAB_UI_h__

#include "tab_abstract.h"

class wxCheckBox;
class wxStaticBoxSizer;
class wxRadioButton;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxSlider;
class wxStaticBoxSizer;
class wxFlexGridSizer;
class wxSpinCtrl;

class tab_ui : public abstract_panel
{

	public:
		tab_ui(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~tab_ui();

		void initCameraSizer(wxStaticBoxSizer*);
		void initScrollSpeedSizer(wxStaticBoxSizer* );
		void initUiOptSizer(wxStaticBoxSizer* );
		void initZoomSizer(wxStaticBoxSizer* );
		void updateControls(int what_to_update);
    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		wxSlider** ctrl_scroll_slider;
		static const int ctrl_scroll_slider_size = 5;
		wxRadioButton* ctrl_cam_radio0;
		wxRadioButton* ctrl_cam_radio1;
		wxRadioButton* ctrl_cam_radio2;
		wxRadioButton* ctrl_cam_radio3;
		wxRadioButton* ctrl_cam_radio4;
		wxCheckBox** ctrl_ui_chkb;
		wxSpinCtrl* ctrl_zoom_spin;
		static const int ctrl_ui_chkb_size = 17;

		wxFlexGridSizer* pSizer;
			wxFlexGridSizer* cSizerL ;
			wxFlexGridSizer* cSizerR;
			wxFlexGridSizer* cSizerM;

			wxStaticBoxSizer* zoomSizer;
			wxStaticBoxSizer* scrollSpeedSizer ;
			wxStaticBoxSizer* cameraSizer;
			wxStaticBoxSizer* uiOptSizer;

		DECLARE_EVENT_TABLE()
};

#endif
