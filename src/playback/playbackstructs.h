#ifndef PLAYBACKSTRUCTS_H_INCLUDED
#define PLAYBACKSTRUCTS_H_INCLUDED

#include "../offlinebattle.h"

struct Replay
{
    int id;
    int playernum;
    bool can_watch;
    int duration; //in seconds
    int size; //in bytes
    wxString MapName;
    wxString ModName;
    wxString SpringVersion;
    wxString Filename;
    wxString date;
    OfflineBattle battle;
    Replay():id(0),playernum(0),can_watch(false),duration(0),size(0){};

    bool Equals( const Replay& other ) const { return Filename == other.Filename; }
};


#endif // PLAYBACKSTRUCTS_H_INCLUDED
