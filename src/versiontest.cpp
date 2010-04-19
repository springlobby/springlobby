/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <iostream>

int main ( int , char** )
{
#ifdef HAVE_CONFIG_H
	#include "springlobby_config.h"
	std::cout << "config.h found\n";
#else
	std::cout << "config.h NOT found\n";
#endif //HAVE_CONFIG_H

#ifndef VERSION
	#define VERSION "unknown"
	std::cout << "VERSION not defined, set to unknown";
#else
	std::cout << "VERSION was defined to" << VERSION;
#endif //VERSION
	std::cout << std::endl;
	return 0;
}
