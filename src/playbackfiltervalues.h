/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef PlaybackListFilterValues_H_INCLUDED
#define PlaybackListFilterValues_H_INCLUDED

#include <wx/string.h>

struct PlaybackListFilterValues {
	// checkboxes
	bool map_show;
	bool mod_show;

	//text fields
	wxString map;
	wxString mod;
	wxString filesize;
	wxString duration;
	//choices
	wxString player_num;


	//modifiers
	wxString player_mode;
	wxString filesize_mode;
	wxString duration_mode;
};

#endif // PlaybackListFilterValues_H_INCLUDED
