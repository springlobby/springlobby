#ifndef REPLAYLISTFILTERVALUES_H_INCLUDED
#define REPLAYLISTFILTERVALUES_H_INCLUDED

struct ReplayListFilterValues
{
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

#endif // REPLAYLISTFILTERVALUES_H_INCLUDED

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

