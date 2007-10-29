#ifndef DIALOG_HPP
#define DIALOG_HPP

#include "Main.hpp"
#include <wx/wx.h>
// child-window class (submenu for mouse input settings)
class Dialog: public wxFrame {
	public:
		Dialog(const wxString& title);
		~Dialog(void);

		void OnClose(wxCloseEvent& event);
		void OnSliderMove(wxCommandEvent&);
		void OnCheckBoxTick(wxCommandEvent&);
		void OnRadioButtonToggle(wxCommandEvent&);

		void init(void);
		void initMouseSpeedSizer(wxStaticBoxSizer*);
		void initMouseOptsSizer(wxStaticBoxSizer*);
	private:
		//class FileParser* parser;
		DECLARE_EVENT_TABLE()
};

#endif
