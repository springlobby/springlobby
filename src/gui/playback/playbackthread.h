/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD
#define SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD

#include <wx/string.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <vector>

#include "defines.h"

static const wxEventType PlaybacksLoadedEvt = wxNewEventType();


template <class PlaybackTabImp >
class PlaybackLoader : public wxEvtHandler
{
protected:
    class PlaybackLoaderThread : public wxThread
    {
        protected:
            typedef PlaybackLoader<PlaybackTabImp>
                ParentType;

        public:
            PlaybackLoaderThread();
            void SetParent( ParentType* parent );
            void* Entry();

        protected:
            ParentType* m_parent;
    };

public:
    typedef PlaybackTabImp
        ParentType;
    typedef typename ParentType::PlaybackType
        PlaybackType;
    typedef typename ParentType::ListType
        ListType;
    typedef PlaybackLoader<PlaybackTabImp>
        ThisType;
    typedef PlaybackLoaderThread
        ThreadType;

    static const bool IsReplayType = ParentType::IsReplayType;

    PlaybackLoader( ParentType* parent );
    ~PlaybackLoader();
    void OnComplete();
    void Run();
    std::vector<std::string> GetPlaybackFilenames();

protected:
    std::vector<std::string> m_filenames;
    ParentType* m_parent;
    ThreadType* m_thread_loader;

};

#include "playbackthread.cpp"
#endif // SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD

