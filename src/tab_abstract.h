#ifndef __TAB_ABSTRACT_h__
#define __TAB_ABSTRACT_h__

class abstract_panel : public wxPanel
{

	public:
		abstract_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~abstract_panel();

		void OnSliderMove(wxCommandEvent&);
		void OnTextUpdate(wxCommandEvent&);
		void OnCheckBoxTick(wxCommandEvent&);
		void OnRadioButtonToggle(wxCommandEvent&);
		void update(wxIdleEvent&);

    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};

#endif
