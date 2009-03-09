/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: HttpDownloader
//

#include <wx/file.h>
#include <wx/protocol/http.h>
#include <wx/string.h>
#include <wx/zipstrm.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/app.h>
#include <wx/log.h>
#include <memory>

#include "../settings++/custom_dialogs.h"
#include "exedownloader.h"
#include "../utils.h"
#include "../globalevents.h"


ExeDownloader::ExeDownloader( const wxString& FileUrl, const wxString& DestPath )
{
    m_thread_updater = new ExeDownloaderThread (  FileUrl, DestPath );
}

ExeDownloader::~ExeDownloader()
{
}

ExeDownloader::ExeDownloaderThread::ExeDownloaderThread(  const wxString& FileUrl, const wxString& DestPath ) :
       // m_calling_class(CallingClass),
        m_destroy(false),
        m_destpath(DestPath),
        m_fileurl(FileUrl)
{
    Init();
}

ExeDownloader::ExeDownloaderThread::~ExeDownloaderThread()
{
}

void ExeDownloader::ExeDownloaderThread::Init()
{
    Create();
    Run();
}


void* ExeDownloader::ExeDownloaderThread::Entry()
{
    wxHTTP FileDownloading;
    // normal timeout is 10 minutes.. set to 10 secs.
    FileDownloading.SetTimeout(10);
    FileDownloading.Connect( m_fileurl.BeforeFirst(_T('/')), 80);
    //customMessageBox(SL_MAIN_ICON, _("connect dl\n.")+m_destpath, _("Error"));
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

              bool unzipOk = Unzip();

              wxCommandEvent notice(ExeDownloadEvt,GetId());
              notice.SetInt( FileDownloading.GetError() );
              if ( unzipOk )
              {
                  notice.SetInt( 0 );
              }
              wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );

            return NULL;
        }
        catch (...)
        {
            wxLogMessage(_T("exception on download of") + m_fileurl);
        }
    }

    //download failed

        wxCommandEvent notice(ExeDownloadEvt,GetId());
        notice.SetInt( FileDownloading.GetError() );
        wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), notice );

    return NULL;
}

bool ExeDownloader::ExeDownloaderThread::Unzip()
{
    try {
        std::auto_ptr<wxZipEntry> entry;

        //wxString base = sett().GetSpringDir + wxFileName::GetPathSeparator() + base + wxFileName::GetPathSeparator();
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
            if ( ( entry->IsDir() )  ) {
                if ( !wxDirExists( file ) )
                wxFileName::Mkdir( file, 0775, wxPATH_MKDIR_FULL );
                wxLogWarning( path );
            }
            else {
                wxFFileOutputStream out( file );
                out.Write(zip);
                out.Close();
            }
        }
    }
    catch (...)
        {return false;}

    return true;

}

bool ExeDownloader::ExeDownloaderThread::TestDestroy()
{
    return m_destroy;
}


void ExeDownloader::ExeDownloaderThread::CloseThread()
{
    m_destroy = true;
}
