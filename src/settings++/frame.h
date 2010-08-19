/**
    This file is part of springsettings,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __SETTINGS_FRAME_h__
#define __SETTINGS_FRAME_h__


#include <wx/frame.h>
#include "../gui/windowattributespickle.h"
class wxNotebook;
class tab_simple;
class tab_ui;
class tab_render_detail;
class tab_quality_video;
class audio_panel;
class hotkey_panel;
class abstract_panel;
class wxMenu;
class wxCommanEvent;
class wxCloseEvent;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxFlexGridSizer;
class wxNotebookEvent;
class PathOptionPanel;
class wxIcon;

class settings_frame : public wxFrame, public WindowAttributesPickle
{
	private:
		DECLARE_EVENT_TABLE();

	public:
		settings_frame(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize);
		virtual ~settings_frame();
		void handleExternExit();
		void switchToExpertMode();
		void buildGuiFromErrorPanel();
	void OnSetFocus(wxFocusEvent&);
	void OnKillFocus(wxFocusEvent&);

	private:
		tab_simple* simpleTab;
		tab_ui* uiTab;
		audio_panel* audioTab;
		tab_render_detail* detailTab;
		tab_quality_video* qualityTab;
		hotkey_panel* hotkeyTab;

		wxMenu* menuFile;
		wxMenu* menuMode;
		wxMenu* menuHelp;
		wxWindow* parentWindow;
		wxNotebook *notebook;
		wxFlexGridSizer *book_sizer;
		wxFlexGridSizer* book_sizer2;
		PathOptionPanel* pathOpt_panel;

		wxIcon* settingsIcon;

		void OnNBchange(wxNotebookEvent&);
		void initMenuBar();
		void handleExit();
		bool alreadyCalled;
		bool m_has_focus;

		enum
		{
			////GUI Enum Control ID Start
			ID_HOTKEY = 1014,
			ID_UI = 1013,
			ID_QUALITY_VIDEO = 1012,
			ID_RENDER_DETAIL = 1011,
			ID_SIMPLE = 1010,
			//ID_DEBUG = 1009,
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
		void resetSettings();
		void updateAllControls();

		bool saveSettingsAbstract(); //do not call abstract_panel::SaveSettings directly, call this instead!
};


#endif
