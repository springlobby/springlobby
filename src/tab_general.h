#ifndef __TAB_GENERAL_h__
#define __TAB_GENERAL_h__

class general_panel : public abstract_panel
{

	public:
		general_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~general_panel();

		void initGeneralSizer(wxStaticBoxSizer*);

    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};

#endif
