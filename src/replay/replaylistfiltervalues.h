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
