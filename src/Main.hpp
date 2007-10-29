#ifndef MAIN_HPP
#define MAIN_HPP

#include <wx/wx.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "ConfigHandler.h"
#include "Defs.hpp"
#include "Window.hpp"
#include "Dialog.hpp"

// main application class
class Main: public wxApp {
	public:
		Main();
		~Main();
		virtual bool OnInit();

		static bool settingsChanged;
};

#endif
