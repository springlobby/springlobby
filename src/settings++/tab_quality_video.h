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

#ifndef __TAB_QUALITY_VIDEO_h__
#define __TAB_QUALITY_VIDEO_h__

#include "tab_abstract.h"
#include <vector>

class wxComboBox;
class wxCheckBox;
class wxBoxSizer;
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
class wxSpinCtrlDbl;

class tab_quality_video : public abstract_panel
{

	public:
		tab_quality_video(wxWindow *parent, wxWindowID id, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~tab_quality_video();

		void initVideoSizer(wxFlexGridSizer*);
		void initQualitySizer(wxFlexGridSizer* );
		void initAASizer(wxFlexGridSizer* );
		void initZBufferSizer(wxFlexGridSizer* );
		void initW4Sizer(wxSizer* );
		void updateControls(int);
		virtual void OnComboBoxChange(wxCommandEvent& event);

	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();

		wxComboBox* ctrl_waterQ_CBox;
		wxCheckBox** ctrl_vo_Boxes;
		static const int ctrl_vo_Boxes_size = 3;
		wxTextCtrl* ctrl_x_res;
		wxTextCtrl* ctrl_y_res;
		wxCheckBox** ctrl_qa_Boxes;
		static const int ctrl_qa_Boxes_size = 12;
		wxSlider* ctrl_fsaa_slider;

		wxRadioButton* ctrl_z_radio1;
		wxRadioButton* ctrl_z_radio2;

		wxSizer* parentSizer ;
        wxSizer* leftSizer ;
        wxSizer* middleSizer;
        wxSizer* rightSizer;
        wxFlexGridSizer* SizerA ;
        wxFlexGridSizer* SizerB ;
        wxFlexGridSizer* SizerC ;
        wxFlexGridSizer* SizerD ;
        wxBoxSizer* SizerE ;
        wxStaticBoxSizer* boxA ;
        wxStaticBoxSizer* boxB ;
        wxStaticBoxSizer* boxC ;
        wxStaticBoxSizer* boxD ;
        wxStaticBoxSizer* boxE ;

        bool m_enable_w4;

        std::vector<wxControl*> m_w4_controls;

        wxSpinCtrlDbl* m_aniso_spin;

    protected:
		DECLARE_EVENT_TABLE()
};

#endif
