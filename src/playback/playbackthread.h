#ifndef SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD
#define SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD

#include <wx/string.h>

static const wxEventType PlaybacksLoadedEvt = wxNewEventType();

template <class PlaybackTabImp >
class PlaybackLoader
{
protected:

    class PlaybackLoaderThread : public wxThread
    {
        private:
            typedef PlaybackLoader<PlaybackTabImp>
                ParentType;

        public:
            PlaybackLoaderThread();
            void SetParent( ParentType* parent );
            void* Entry();

        private:
            ParentType* m_parent;
    };

public:
    typedef PlaybackTabImp
        ParentType;
    typedef typename ParentType::PlaybackType
        PlaybackType;
    typedef PlaybackLoader<PlaybackTabImp>
        ThisType;
    typedef PlaybackLoaderThread
        ThreadType;


    PlaybackLoader( ParentType* parent );
    ~PlaybackLoader();
    void OnComplete();
    void Run();
    wxArrayString GetPlaybackFilenames();

protected:
    wxArrayString m_filenames;
    ParentType* m_parent;
    ThreadType* m_thread_loader;

};

#include "playbackthread.cpp"
#endif // SPRINGLOBBY_HEADERGUARD_PLAYBACKTHREAD

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

