#ifndef SPRINGLOBBY_UPDATER_H_INCLUDED
#define SPRINGLOBBY_UPDATER_H_INCLUDED

#include <wx/event.h>
#include <downloader/httpdownloader.h>

class UpdaterClass : public wxEvtHandler
{
  public:
    UpdaterClass();
    ~UpdaterClass();
    void CheckForUpdates();

    bool StartUpdate( const wxString& latestVersion, const wxString& exe_to_update );
    void OnDownloadEvent( wxCommandEvent& event );

protected:

    bool UpdateExe( const wxString& newexe, bool WaitForReboot );
    bool UpdateLocale( const wxString& newdir, bool WaitForReboot );

    bool PostMinGW44( const wxString& newdir );

    wxString m_newexe;
    wxString m_currentexe;
    wxString m_latest_version;

    HttpDownloaderThread<UpdaterClass>* m_http_thread;

    DECLARE_EVENT_TABLE()
};

UpdaterClass& Updater();

#endif // SPRINGLOBBY_GLOBALEVENTS_H_INCLUDED

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

