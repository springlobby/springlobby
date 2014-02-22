/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/string.h>

struct BattleListFilterValues
{
    // checkboxes
        bool status_locked;
        bool status_passworded;
        bool status_start;
        bool status_full;
        bool status_open;
        bool map_show;
        bool mod_show;
        bool highlighted_only;
    //text fields
        wxString host;
        wxString description;
        wxString map;
        wxString mod;
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

