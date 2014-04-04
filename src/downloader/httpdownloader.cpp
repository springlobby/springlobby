/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
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
#include <wx/thread.h>
#include <wx/event.h>

#include <memory>

#include "httpdownloader.h"
#include "utils/conversion.h"

const wxEventType HttpDownloaderThread::httpDownloadEvtComplete = wxNewEventType();

HttpDownloaderThread::HttpDownloaderThread(const wxString& FileUrl, const wxString& DestPath, const bool unzip, wxObjectEventFunction func, wxEvtHandler* evt)
		: wxThread(wxTHREAD_JOINABLE),
		m_do_unzip( unzip ),
		m_destroy( false ),
		m_destpath( DestPath ),
		m_fileurl( FileUrl ),
		m_evt(evt)
{
	m_evt->Connect(httpDownloadEvtComplete, func);
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
	wxCommandEvent evt(httpDownloadEvtComplete);
	const wxString host = m_fileurl.AfterFirst('/').AfterFirst('/').BeforeFirst('/');
	const wxString url = m_fileurl.AfterFirst('/').AfterFirst('/').AfterFirst('/');
	wxHTTP FileDownloading;
	// normal timeout is 10 minutes.. set to 10 secs.
	FileDownloading.SetTimeout( 60 );
	FileDownloading.Connect(host, 80 );
	wxInputStream* m_httpstream = FileDownloading.GetInputStream( _T("/") + url );

	if (!m_httpstream ) {
		evt.SetInt(-1);
		m_evt->AddPendingEvent(evt);
		return NULL;
	}

	try
	{
		wxFileOutputStream outs( m_destpath );
		m_httpstream->Read( outs );
		outs.Close();
		delete m_httpstream;
		m_httpstream = 0;
		//download success
		if ( m_do_unzip ) {
			if(!Unzip()) {
				evt.SetInt(-1);
			}
		}

	} catch ( ... ) {
		evt.SetInt(-1);
		wxLogMessage( _T( "exception on download of" ) + m_fileurl );
	}

	m_evt->AddPendingEvent(evt);
	return NULL;
}

bool HttpDownloaderThread::Unzip()
{
	try
	{
		std::auto_ptr<wxZipEntry> entry;

		wxString base = wxPathOnly( m_destpath ) + wxFileName::GetPathSeparator();
		wxFFileInputStream in( m_destpath );
		wxZipInputStream zip( in );

		while ( entry.reset( zip.GetNextEntry() ), entry.get() != NULL )
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
			    wxLogDebug( _T("unzipping: ")+ file );
				wxFFileOutputStream out( file );
				if( !out.IsOk() )
                    wxLogError( _T("unzipping failed on: ")+ file );
                else {
                    out.Write( zip );
                    out.Close();
                }
			}
		}
	}
	catch ( std::exception& e )
	{
	    wxLogError( TowxString( e.what() ) );
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
