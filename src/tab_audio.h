#ifndef __TAB_AUDIO_h__
#define __TAB_AUDIO_h__

class audio_panel : public abstract_panel
{

	public:
		audio_panel(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~audio_panel();

		void initAudioSizer(wxStaticBoxSizer*);
	
    protected:
        void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		DECLARE_EVENT_TABLE()
};

#endif
