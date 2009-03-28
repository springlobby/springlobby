#ifndef SPRINGLOBBY_HEADERGUARD_REPLAYTHREAD
#define SPRINGLOBBY_HEADERGUARD_REPLAYTHREAD

#include <wx/string.h>
#include "replaylist.h"

extern const wxEventType ReplaysLoadedEvt;

class ReplayLoader
{
public:
    ReplayLoader( wxWindow* parent, ReplayList& list );
    ~ReplayLoader();
    void OnComplete(wxCommandEvent& event);

protected:

    class ReplayLoaderThread : public wxThread
        {
        public:
            ReplayLoaderThread(  ReplayList& list );
            ~ReplayLoaderThread();
            void Init();
            void* Entry();
            void CloseThread();
            bool TestDestroy();
        private:
            bool m_destroy;

            ReplayList& m_replays;
        };

    static wxWindow* m_parent;
    ReplayLoaderThread* m_thread_loader;

};



#endif // SPRINGLOBBY_HEADERGUARD_REPLAYTHREAD

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

