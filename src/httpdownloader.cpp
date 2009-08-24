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


#include "utils/conversion.h"

template <class ParentClass>
HttpDownloaderThread<ParentClass>::HttpDownloaderThread(  const wxString& FileUrl, const wxString& DestPath,
        ParentClass& parent, int code, const bool notify, const bool unzip,
        const wxString& noticeErr, const wxString& noticeOk   )
		: m_destroy( false ),
		m_destpath( DestPath ),
		m_fileurl( FileUrl ),
		m_do_unzip( unzip ),
		m_notifyOnDownloadEvent( notify ),
		m_noticeErr( noticeErr ),
		m_noticeOk( noticeOk ),
		m_id_code( code ),
		m_parent( parent )
{
	Init();
}

template <class ParentClass>
HttpDownloaderThread<ParentClass>::~HttpDownloaderThread()
{
}

template <class ParentClass>
void HttpDownloaderThread<ParentClass>::Init()
{
	Create();
	Run();
}

template <class ParentClass>
void* HttpDownloaderThread<ParentClass>::Entry()
{
	wxHTTP FileDownloading;
	// normal timeout is 10 minutes.. set to 10 secs.
	FileDownloading.SetTimeout( 60 );
	FileDownloading.Connect( m_fileurl.BeforeFirst( _T( '/' ) ), 80 );
	wxInputStream* m_httpstream = FileDownloading.GetInputStream( _T( "/" ) + m_fileurl.AfterFirst( _T( '/' ) ) );

	if ( m_httpstream )
	{
		try
		{
			wxFileOutputStream outs( m_destpath );
			m_httpstream->Read( outs );
			outs.Close();
			delete m_httpstream;
			m_httpstream = 0;
			//download success
			if ( m_notifyOnDownloadEvent )
			{
				wxCommandEvent notice( httpDownloadEvtComplete, m_id_code );
				notice.SetString( m_fileurl + _( "\nsuccessfully saved to:\n" ) + m_destpath );
				if ( m_do_unzip )
				{
					bool unzipOk = Unzip();
					if ( m_noticeOk == wxEmptyString )
					{
						if ( unzipOk ) notice.SetString( m_fileurl + _( "\nsuccessfully unzipped in:\n" ) + m_destpath );
						else notice.SetString( notice.GetString() + _( "\n unzipping failed, please correct manually" ) );
					}
				}
				if ( m_noticeOk != wxEmptyString ) notice.SetString( m_noticeOk );
				notice.SetInt( FileDownloading.GetError() );
				wxPostEvent( &m_parent, notice );
			}
			return NULL;
		}
		catch ( ... )
		{
			wxLogMessage( _T( "exception on download of" ) + m_fileurl );
		}
	}

	//download failed
	if ( m_notifyOnDownloadEvent )
	{
		wxCommandEvent notice( httpDownloadEvtFailed, m_id_code );
		if ( m_noticeErr == wxEmptyString )
			notice.SetString( _( "Could not save\n" ) + m_fileurl + _( "\nto:\n" ) + m_destpath );
		else
			notice.SetString( m_noticeErr );
		notice.SetString( notice.GetString() + _( "\nError number: " ) + TowxString( FileDownloading.GetError() ) );
		notice.SetInt( FileDownloading.GetError() );
		wxPostEvent( &m_parent, notice );
	}

	return NULL;
}

template <class ParentClass>
bool HttpDownloaderThread<ParentClass>::Unzip()
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
				wxFFileOutputStream out( file );
				out.Write( zip );
				out.Close();
			}
		}
	}
	catch ( ... )
	{
		return false;
	}

	return true;

}

template <class ParentClass>
bool HttpDownloaderThread<ParentClass>::TestDestroy()
{
	return m_destroy;
}

template <class ParentClass>
void HttpDownloaderThread<ParentClass>::CloseThread()
{
	m_destroy = true;
}
