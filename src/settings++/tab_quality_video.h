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

#ifndef __TAB_QUALITY_VIDEO_h__
#define __TAB_QUALITY_VIDEO_h__

#include "tab_abstract.h"

class wxComboBox;
class wxCheckBox;
class wxTextCtrl;
class wxFlexGridSizer;
class wxRadioButton;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxSlider;
class wxStaticBoxSizer;

class tab_quality_video : public abstract_panel
{

	public:
		tab_quality_video(wxWindow *parent, wxWindowID id, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~tab_quality_video();

		void initVideoSizer(wxFlexGridSizer*);
		void initQualitySizer(wxFlexGridSizer* );
		void initAASizer(wxFlexGridSizer* );
		void initZBufferSizer(wxFlexGridSizer* );
		void updateControls(int);
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();

		wxComboBox* ctrl_waterQ_CBox;
		wxCheckBox** ctrl_vo_Boxes;
		static const int ctrl_vo_Boxes_size = 3; 
		wxTextCtrl* ctrl_x_res;
		wxTextCtrl* ctrl_y_res;
		wxCheckBox** ctrl_qa_Boxes;
		static const int ctrl_qa_Boxes_size = 10;
		wxSlider* ctrl_fsaa_slider;
		
		wxRadioButton* ctrl_z_radio1;
		wxRadioButton* ctrl_z_radio2;
		
		wxSizer* parentSizer ;	
			wxSizer* leftSizer ;
			wxSizer* middleSizer;
			//wxSizer* rightSizer = new wxFlexGridSizer(1,15,0);//for info
			wxFlexGridSizer* SizerA ;
			wxFlexGridSizer* SizerB ;
			wxFlexGridSizer* SizerC ;
			wxFlexGridSizer* SizerD ;
			wxStaticBoxSizer* boxA ;
			wxStaticBoxSizer* boxB ;
			wxStaticBoxSizer* boxC ;
			wxStaticBoxSizer* boxD ;
		
    protected:
		DECLARE_EVENT_TABLE()
};

#endif
