/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/log.h>
#include <wx/intl.h>

#include <string>
#include <sstream>

#include "se_utils.h"

#include "gui/customdialogs.h"
#include "settings.h"

static bool standalonemode = true;

bool IsSettingsStandAlone()
{
	return standalonemode;
}

void SetSettingsStandAlone( bool value )
{
	standalonemode = value;
}


int fromString( const wxString& s ) {
	long temp = 0;
	s.ToLong( &temp );
	return int( temp );
}
