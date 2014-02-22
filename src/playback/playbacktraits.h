/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef PLAYBACKTRAITS_H_INCLUDED
#define PLAYBACKTRAITS_H_INCLUDED

struct Replay;
class ReplayList;

class ReplayTraits {
    public:
        typedef Replay
            PlaybackType;

        typedef ReplayList
            ListType;

        static const bool IsReplayType = true;
};

struct Savegame;
class SavegameList;

class SavegameTraits {
    public:
        typedef Savegame
            PlaybackType;

        typedef SavegameList
            ListType;

        static const bool IsReplayType = false;
};

#endif // PLAYBACKTRAITS_H_INCLUDED
