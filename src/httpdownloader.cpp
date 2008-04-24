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
//#include

#include "httpdownloader.h"
#include "utils.h"
#include "uiutils.h"
#include "settings++/custom_dialogs.h"




HttpDownloader::HttpDownloader( const wxString& FileUrl, const wxString& DestPath )
{

    m_thread_updater = new UpdateProgressbar( *this, FileUrl, DestPath );
}

HttpDownloader::~HttpDownloader()
{
    if (m_thread_updater)
    {
        delete m_thread_updater;
        m_thread_updater = 0;
    }

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
    wxCommandEvent notice(httpDownloadEvt,1234);
    if ( m_httpstream )
    {
        try
        {
            wxFileOutputStream outs(m_destpath);
            m_httpstream->Read(outs);
            outs.Close();
            delete m_httpstream;
            notice.SetString(_("Download of File xyz complete"));
            int k = SL_GlobalEvtHandler::GetSL_GlobalEvtHandler().ProcessEvent(notice) ;
            wxLogMessage (_T("fail?: ") + i2s(k)) ;
            //customMessageBox(SL_MAIN_ICON,_("Download of File xyz complete"),_("Download complete") );
        }
        catch (...)
        {
            wxLogMessage(_("download of _SOMEFILE_ failed"));

  //          customMessageBoxNoModal(SL_MAIN_ICON,_("Download of File xyz failed"),_("Download failed") );
        }
    }
    //else
    //     customMessageBoxNoModal(SL_MAIN_ICON,_("Download of File xyz failed"),_("Download failed") );

}


bool UpdateProgressbar::TestDestroy()
{
    return m_destroy;
}


void UpdateProgressbar::CloseThread()
{
    m_destroy = true;

}
