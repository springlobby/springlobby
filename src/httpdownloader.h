#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

class HttpDownloader;
#include <wx/string.h>


class HttpDownloader
{
public:
    HttpDownloader( const wxString& FileUrl, const wxString& DestPath, const bool notify = true,
                    const wxString& noticeErr = wxEmptyString, const wxString& noticeOk  = wxEmptyString);
    ~HttpDownloader();
    void OnComplete(wxCommandEvent& event);

protected:

    class HttpDownloaderThread : public wxThread
        {
        public:
            HttpDownloaderThread(  const wxString& FileUrl, const wxString& DestPath,
                                  const bool notify = true, const wxString& noticeErr = wxEmptyString, const wxString& noticeOk = wxEmptyString);
            ~HttpDownloaderThread();
            void Init();
            void* Entry();
            void CloseThread();
            bool TestDestroy();
        private:
            bool m_destroy;

            wxString m_destpath;
            wxString m_fileurl;
            bool Unzip();
            bool m_notifyOnDownloadEvent;

            wxString m_noticeErr;
            wxString m_noticeOk;

        };

    HttpDownloaderThread* m_thread_updater;

};



#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

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

