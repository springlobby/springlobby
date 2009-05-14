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
#include <wx/log.h>
#include <memory>

#include "httpdownloader.h"
#include "utils.h"
#include "globalevents.h"


HttpDownloaderThread::HttpDownloaderThread(  const wxString& FileUrl, const wxString& DestPath, wxEvtHandler& parent, int code, const bool notify, const wxString& noticeErr, const wxString& noticeOk   ) :
       // m_calling_class(CallingClass),
        m_destroy(false),
        m_destpath(DestPath),
        m_fileurl(FileUrl),
        m_notifyOnDownloadEvent( notify),
        m_noticeErr(noticeErr),
        m_noticeOk(noticeOk),
        m_id_code(code),
        m_parent(parent)

{
    Init();
}

HttpDownloaderThread::~HttpDownloaderThread()
{
}

void HttpDownloaderThread::Init()
{
    Create();
    Run();
}


void* HttpDownloaderThread::Entry()
{
    wxHTTP FileDownloading;
    // normal timeout is 10 minutes.. set to 10 secs.
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
            m_httpstream = 0;
            //download success
            if (m_notifyOnDownloadEvent)
            {
                bool unzipOk = Unzip();
                wxCommandEvent notice(httpDownloadEvtComplete, m_id_code);
                if (m_noticeOk == wxEmptyString)
                    if ( unzipOk )
                        notice.SetString(m_fileurl + _("\nsuccessfully unzipped in:\n") + m_destpath);
                    else
                        notice.SetString(m_fileurl + _("\nsuccessfully saved to:\n") + m_destpath +
                            _("\n unzipping failed, please correct manually"));
                else
                    notice.SetString(m_noticeOk);
								notice.SetInt( FileDownloading.GetError() );
                wxPostEvent( &m_parent, notice );
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
        wxCommandEvent notice(httpDownloadEvtFailed, m_id_code);
        if (m_noticeErr == wxEmptyString)
            notice.SetString(_("Could not save\n") + m_fileurl + _("\nto:\n") + m_destpath );
        else
            notice.SetString(m_noticeErr);
        notice.SetString( notice.GetString() + _("\nError number: ") + TowxString(FileDownloading.GetError() ) );
        notice.SetInt( FileDownloading.GetError() );
        wxPostEvent( &m_parent, notice );
    }

    return NULL;
}

bool HttpDownloaderThread::Unzip()
{
    try
    {
        std::auto_ptr<wxZipEntry> entry;

        wxString base = wxPathOnly( m_destpath ) + wxFileName::GetPathSeparator();
				wxFFileInputStream in(m_destpath);
        wxZipInputStream zip(in);

        while (entry.reset(zip.GetNextEntry()), entry.get() != NULL)
        {
            // access meta-data
            wxString name = entry->GetInternalName();
//            name.Replace( wxT("/") , wxT("\\") );
            // read 'zip' to access the entry's data
            using namespace std;
            wxString file = base + name;
            wxString path = wxPathOnly( file );
            if ( ( entry->IsDir() )  )
            {
                if ( !wxDirExists( file ) )
                wxFileName::Mkdir( file, 0775, wxPATH_MKDIR_FULL );
                wxLogWarning( path );
            }
            else
            {
                wxFFileOutputStream out( file );
                out.Write(zip);
                out.Close();
            }
        }
    }
    catch (...)
		{
			return false;
		}

    return true;

}

bool HttpDownloaderThread::TestDestroy()
{
    return m_destroy;
}


void HttpDownloaderThread::CloseThread()
{
    m_destroy = true;
}
