/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: HttpDownloader
//

#include <wx/file.h>
#include <wx/intl.h>
#include <wx/protocol/http.h>
#include <wx/string.h>
#include <wx/zipstrm.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/app.h>

#include "httpdownloader.h"
#include "utils.h"
#include "globalevents.h"


HttpDownloader::HttpDownloader( const wxString& FileUrl, const wxString& DestPath,
                                const bool notify, const wxString& noticeErr, const wxString& noticeOk  )
    : m_thread_updater ( *this, FileUrl, DestPath, notify, noticeErr, noticeOk   )

{
}

HttpDownloader::~HttpDownloader()
{
}

HttpDownloader::HttpDownloaderThread::HttpDownloaderThread( HttpDownloader& CallingClass, const wxString& FileUrl, const wxString& DestPath,
                const bool notify, const wxString& noticeErr, const wxString& noticeOk   ) :
        m_calling_class(CallingClass),
        m_destroy(false),
        m_destpath(DestPath),
        m_fileurl(FileUrl),
        m_notifyOnDownloadEvent( notify),
        m_noticeErr(noticeErr),
        m_noticeOk(noticeOk)

{
    Init();
}

HttpDownloader::HttpDownloaderThread::~HttpDownloaderThread()
{
}

void HttpDownloader::HttpDownloaderThread::Init()
{
    Create();
    Run();
}


void* HttpDownloader::HttpDownloaderThread::Entry()
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
            //download success
            if (m_notifyOnDownloadEvent)
            {
                wxCommandEvent notice(httpDownloadEvtComplete,GetId());
                if (m_noticeOk == wxEmptyString)
                    notice.SetString(m_fileurl + _("\nsuccessfully saved to:\n") + m_destpath);
                else
                    notice.SetString(m_noticeOk);
                wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );
            }
            return NULL;
        }
        catch (...)
        {
            wxLogMessage(_T("exception on download of") + m_fileurl);
        }
    }

    //download failed
    if (m_notifyOnDownloadEvent)
    {
        wxCommandEvent notice(httpDownloadEvtFailed,GetId());
        if (m_noticeErr == wxEmptyString)
            notice.SetString(_("Could not save\n") + m_fileurl + _("\nto:\n") + m_destpath);
        else
            notice.SetString(m_noticeErr);
        wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );
    }

    return NULL;
}


bool HttpDownloader::HttpDownloaderThread::TestDestroy()
{
    return m_destroy;
}


void HttpDownloader::HttpDownloaderThread::CloseThread()
{
    m_destroy = true;
}
