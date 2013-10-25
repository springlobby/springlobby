/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include "flagimages.h"
#include "flagimagedata.h"

#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <wx/log.h>

#include <map>

#include "utils/conversion.h"

typedef std::map<wxString, int> NameToPosMap;
static NameToPosMap m_name_to_pos;

int GetFlagIndex( const wxString& flag )
{
	if (( flag.IsEmpty() ) || (flag == _T("??")))
		return FLAG_NONE;

	NameToPosMap::const_iterator itor = m_name_to_pos.find( flag );
	if ( itor != m_name_to_pos.end() ) return itor->second;

	wxLogMessage( _T( "%s flag not found!" ), flag.c_str() );
	return FLAG_NONE;
}

int AddFlagImages( wxImageList& imgs )
{
	int poszero = 0;
	for ( int i = 0; flag_xpm[i]; ++i )
	{
		const int index = imgs.Add( wxBitmap( const_cast<const char**>( flag_xpm[i] ) ) );
		if ( i == 0 ) poszero = index;
		m_name_to_pos[TowxString( flag_str[i] )] = i;
	}
	return poszero;
}

