/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#ifdef __WXMAC__
	#include <wx/bitmap.h>
#endif

#include "flagimages.h"
#include "flagimagedata.h"

#include <wx/imaglist.h>
#include <wx/log.h>

#include "utils/conversion.h"

int GetFlagIndex( const wxString& flag )
{
	if ( flag.IsEmpty() )
		return FLAG_NONE;

	for ( int i = 0; flag_str[i]; ++i ) {
		if ( flag == TowxString( flag_str[i] ) ) {
			return i;
		}
	}
	wxLogMessage( _T( "%s flag not found!" ), flag.c_str() );
	return FLAG_NONE;
}

int AddFlagImages( wxImageList& imgs )
{
	int index, poszero;
	index = poszero = 0;
	for ( int i = 0; flag_xpm[i]; ++i )
	{
		index = imgs.Add( wxBitmap( const_cast<const char**>( flag_xpm[i] ) ) );
		if ( i == 0 ) poszero = index;
	}
	return poszero;
}

