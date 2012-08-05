#ifndef SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD
#define SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD

#include <wx/string.h>
#include "../defines.h"
#include <wx/thread.h>

static const wxEventType PlaybacksLoadedEvt = wxNewEventType();

#ifdef HAVE_WX29
#include <wx/event.h>
    template <class PlaybackTabImp >
    class PlaybackLoader : public wxEvtHandler
#else
    template <class PlaybackTabImp >
    class PlaybackLoader
#endif
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

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

