#ifndef TASCLIENTIMPORT_H
#define TASCLIENTIMPORT_H

#ifdef __WXMSW__
//! read some basic tasc settings from registry and map them to our setting handler
bool ImportTASClientSettings();
//! return true if "HKEY_CURRENT_USER\\Software\\TASClient\\Preferences" exists
bool TASClientPresent();
#endif

#endif // TASCLIENTIMPORT_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

