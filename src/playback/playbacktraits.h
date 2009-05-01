#ifndef PLAYBACKTRAITS_H_INCLUDED
#define PLAYBACKTRAITS_H_INCLUDED

#include "playbackstructs.h"
#include "replaylist.h"

class Ui;

class ReplayTraits {
    public:
        typedef Replay
            PlaybackType;

        typedef ReplayList
            ListType;

        static const bool IsReplayType = true;
};

#endif // PLAYBACKTRAITS_H_INCLUDED
