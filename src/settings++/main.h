/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef SPRINGSETTINGS_MAIN_h
#define SPRINGSETTINGS_MAIN_h

#include <wx/app.h>

class Springsettings : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
		virtual void OnFatalException();

		//! @brief Initializes the logging functions.
        ///initializes logging in an hidden stream and std::cout/gui messages
        void InitializeLoggingTargets();

};

#endif
