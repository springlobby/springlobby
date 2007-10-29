#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <unistd.h>
#include "Main.hpp"

// parent-window class
class Window: public wxFrame {
	public:
		Window(const wxString& title);
		~Window(void);

		void setParser(class FileParser*);
		void handleExit(void);

		void OnCloseButtonPress(wxCloseEvent&);
		void OnMenuChoice(wxCommandEvent&);
		void OnSliderMove(wxCommandEvent&);
		void OnTextUpdate(wxCommandEvent&);
		void OnCheckBoxTick(wxCommandEvent&);
		void OnRadioButtonToggle(wxCommandEvent&);
		void update(wxIdleEvent&);

		void initRendererSizer(wxStaticBoxSizer*);
		void initGeneralSizer(wxStaticBoxSizer*);
		void initVideoSizer(wxStaticBoxSizer*);
		void initAudioSizer(wxStaticBoxSizer*);
		void initWaterSizer(wxStaticBoxSizer*);
		void initDebugSizer(wxStaticBoxSizer*);
		void initMenuBar(void);

	private:
		class Dialog* dialog;
		

		DECLARE_EVENT_TABLE()
};

#endif
