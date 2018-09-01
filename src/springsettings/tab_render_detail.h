/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
	tab_render_detail(wxWindow* parent, wxWindowID id = 1, const wxString& title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	virtual ~tab_render_detail();

	void initRendererSizer(wxFlexGridSizer*, wxFlexGridSizer*);
	void updateControls(int what_to_update);

private:
	void OnClose(wxCloseEvent& event);
	void CreateGUIControls();

	wxSlider** ctrl_detail_sliders;
	static const int ctrl_detail_sliders_size = 9;

	wxSizer* renderSizer;
	wxSizer* parentSizer;
	wxFlexGridSizer* rendererSizerA;
	wxFlexGridSizer* rendererSizerB;
	wxStaticBoxSizer* box;

	DECLARE_EVENT_TABLE()
};

#endif
