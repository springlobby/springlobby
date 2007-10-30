#ifndef __TAB_MOUSE_h__
#define __TAB_MOUSE_h__
#include "tabs.h"

class mouse_panel : public abstract_panel
{

	public:
		mouse_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~mouse_panel();

		void initMouseOptsSizer(wxStaticBoxSizer*);
		void initMouseSpeedSizer(wxStaticBoxSizer* );

    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};

#endif
