#include "Main.hpp"

// static class variable indicating if user changed settings
bool (Main::settingsChanged) = false;

Main::Main() {}
Main::~Main() {}

bool Main::OnInit() {
	

	//if (parser->readFile(getenv("HOME"), ".springrc")) {
        
        //insert configHandler sanity check
        
		// windows need to be able to update and serialize dictionary
		Window* frame = new Window(wxT("Settings++"));
	
		frame->SetSize(0, 0, PWIN_SIZE_X, PWIN_SIZE_Y);
		frame->SetSizeHints(PWIN_SIZE_X, PWIN_SIZE_Y, PWIN_SIZE_X, PWIN_SIZE_Y);
		frame->CenterOnScreen(wxBOTH);
		frame->Show(true);

		return true;
//	}
//	else {
//		wxMessageBox(_S("Unable to locate .springrc file!"), _S(""), wxOK, NULL);
//		return false;
//	}
}


// macro providing main() entry point
IMPLEMENT_APP(Main)
