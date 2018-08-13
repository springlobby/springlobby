/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
	audio_panel(wxWindow* parent, wxWindowID id = 1, const wxString& title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~audio_panel();

	void initAudioSizer(wxStaticBoxSizer*);
	void updateControls(int);

private:
	void OnClose(wxCloseEvent& event);
	void CreateGUIControls();

	wxSlider** ctrl_audio_sliders;
	static const int ctrl_audio_sliders_size = 6;

	wxSizer* parentSizer; // main window sizer (three columns)
	wxSizer* childLSizer;
	wxStaticBoxSizer* audioSizer;

	DECLARE_EVENT_TABLE()
};

#endif
