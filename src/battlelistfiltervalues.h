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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

