/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_DEFINES_H
#define SPRINGLOBBY_HEADERGUARD_DEFINES_H

#if ( !defined(HAVE_WX29) && !defined(HAVE_WX28) )
	#if( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 9 ) || (wxMAJOR_VERSION == 3 && wxMINOR_VERSION == 0)
		#define HAVE_WX29
	#elif( wxMAJOR_VERSION==2 && wxMINOR_VERSION == 8 )
		#define HAVE_WX28
	#endif
#endif


#endif // SPRINGLOBBY_HEADERGUARD_DEFINES_H

