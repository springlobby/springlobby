#ifndef __TAB_DEBUG_h__
#define __TAB_DEBUG_h__

class debug_panel : public abstract_panel
{

	public:
		debug_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~debug_panel();

		void initDebugSizer(wxStaticBoxSizer*);

    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};

#endif
