#ifndef __TAB_VIDEO_h__
#define __TAB_VIDEO_h__
#include "tabs.h"

class video_panel : public abstract_panel
{

	public:
		video_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~video_panel();

		void initVideoSizer(wxStaticBoxSizer*);

	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
    protected:
		DECLARE_EVENT_TABLE()
};

#endif
