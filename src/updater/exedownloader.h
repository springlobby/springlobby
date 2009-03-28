#ifndef SPRINGLOBBY_HEADERGUARD_EXEDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_EXEPDOWNLOADER

#include <wx/string.h>

class ExeDownloader
{
public:
    ExeDownloader( const wxString& FileUrl, const wxString& DestPath );
    ~ExeDownloader();
    void OnComplete(wxCommandEvent& event);

protected:

    class ExeDownloaderThread : public wxThread
        {
        public:
            ExeDownloaderThread(  const wxString& FileUrl, const wxString& DestPath );
            ~ExeDownloaderThread();
            void Init();
            void* Entry();
            void CloseThread();
            bool TestDestroy();
        private:
            bool m_destroy;

            wxString m_destpath;
            wxString m_fileurl;
            bool Unzip();
        };

    ExeDownloaderThread* m_thread_updater;

};



#endif // SPRINGLOBBY_HEADERGUARD_ExeDOWNLOADER

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

