#ifndef PLAYBACKTRAITS_H_INCLUDED
#define PLAYBACKTRAITS_H_INCLUDED

class Replay;
class ReplayList;

class ReplayTraits {
    public:
        typedef Replay
            PlaybackType;

        typedef ReplayList
            ListType;

        static const bool IsReplayType = true;
};

#endif // PLAYBACKTRAITS_H_INCLUDED
