/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: HttpDownloader
//

#include <wx/file.h>
#include <wx/intl.h>
#include <wx/progdlg.h>
#include <wx/protocol/http.h>
#include <wx/string.h>
#include <wx/zipstrm.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/app.h>

#include "httpdownloader.h"
#include "utils.h"
#include "globalevents.h"


HttpDownloader::HttpDownloader( const wxString& FileUrl, const wxString& DestPath )
{

    m_thread_updater ( *this, FileUrl, DestPath );
}

HttpDownloader::~HttpDownloader()
{
//    if (m_thread_updater !=0)
//    {
//        delete m_thread_updater;
//        m_thread_updater = 0;
//    }

}

void HttpDownloader::SetPointer(UpdateProgressbar* up)
{
    m_thread_updater = up;
}



UpdateProgressbar::UpdateProgressbar( HttpDownloader& CallingClass, const wxString& FileUrl, const wxString& DestPath ) :
        m_calling_class(CallingClass),
        m_destroy(false),
        m_destpath(DestPath),
        m_fileurl(FileUrl)
{
    Init();
}

UpdateProgressbar::~UpdateProgressbar()
{
}

void UpdateProgressbar::Init()
{
    Create();
    Run();
}


void* UpdateProgressbar::Entry()
{
    wxHTTP FileDownloading;
    /// normal timeout is 10 minutes.. set to 10 secs.
    FileDownloading.SetTimeout(10);
    FileDownloading.Connect( m_fileurl.BeforeFirst(_T('/')), 80);
    wxInputStream* m_httpstream = FileDownloading.GetInputStream( _T("/") + m_fileurl.AfterFirst(_T('/')) );
    if ( m_httpstream )
    {
        try
        {
            wxFileOutputStream outs(m_destpath);
            m_httpstream->Read(outs);
            outs.Close();
            delete m_httpstream;
            wxCommandEvent notice(httpDownloadEvtComplete,GetId());
            notice.SetString(m_fileurl + _("\nsuccessfully saved to:\n") + m_destpath);
            wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );
            return NULL;
        }
        catch (...)
        {
            wxLogMessage(_T("exception on download of") + m_fileurl);
        }
    }

    //download failed
    wxCommandEvent notice(httpDownloadEvtFailed,GetId());
    notice.SetString(_("Could not save\n") + m_fileurl + _("\nto:\n") + m_destpath);
    wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );

    return NULL;
}


bool UpdateProgressbar::TestDestroy()
{
    return m_destroy;
}


void UpdateProgressbar::CloseThread()
{
    m_destroy = true;
    //m_calling_class.SetPointer(this);
}
