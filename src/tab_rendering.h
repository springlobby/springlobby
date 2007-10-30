#ifndef __TAB_RENDERING_h__
#define __TAB_RENDERING_h__
#include "tabs.h"

class rendering_panel : public abstract_panel
{
 	
	public:
		rendering_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~rendering_panel();

		void initRendererSizer(wxStaticBoxSizer*,wxStaticBoxSizer*);

    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};   
    
#endif    
