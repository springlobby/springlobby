#ifndef SPRINGLOBBY_HEADERGUARD_DEFINES_H
#define SPRINGLOBBY_HEADERGUARD_DEFINES_H

#if ( !defined(HAVE_WX29) && !defined(HAVE_WX28) )
	#if( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 9 )
		#define HAVE_WX29
	#elif( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 8 )
		#define HAVE_WX28
	#endif
#endif


#endif // SPRINGLOBBY_HEADERGUARD_DEFINES_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

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
