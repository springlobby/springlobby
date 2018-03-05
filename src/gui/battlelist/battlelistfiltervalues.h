/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/string.h>

struct BattleListFilterValues
{
	BattleListFilterValues()
	    : status_locked(false)
	    , status_passworded(false)
	    , status_start(false)
	    , status_full(false)
	    , status_open(false)
	    , map_show(false)
	    , game_show(false)
	    , highlighted_only(false)
	    , rank(_T("All"))
	    , player_num(_T("All"))
	    , maxplayer(_T("All"))
	    , spectator(_T( "All" ))
	    , rank_mode(_T( "<" ))
	    , player_mode(_T("="))
	    , maxplayer_mode(_T("="))
	    , spectator_mode(_T("="))
	{
	}
	// checkboxes
	bool status_locked;
	bool status_passworded;
	bool status_start;
	bool status_full;
	bool status_open;
	bool map_show;
	bool game_show;
	bool highlighted_only;
	//text fields
	wxString host;
	wxString description;
	wxString map;
	wxString game;
	//choices
	wxString rank;
	wxString player_num;
	wxString maxplayer;
	wxString spectator;
	//modifiers
	wxString rank_mode;
	wxString player_mode;
	wxString maxplayer_mode;
	wxString spectator_mode;
};
