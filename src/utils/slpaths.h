#ifndef SPRINGLOBBY_SLPATHS_H
#define SPRINGLOBBY_SLPATHS_H

class wxString;

//! Interface for accessing paths for differnt files
// like configuration, logs ...
//
// the functions shall return the correct path taking
// IsPortableMode() into account
class SlPaths
{
	public:
		//! used for passing config file at command line
		static bool m_user_defined_config;
		static wxString m_user_defined_config_path;

		static bool IsPortableMode();
		static wxString GetLocalConfigPath();
		static wxString GetDefaultConfigPath();
		static wxString GetConfigPath();
};

#endif // SPRINGLOBBY_PATHLISTFACTORY_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2014

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/


