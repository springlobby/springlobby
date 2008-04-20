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

#include "httpdownloader.h"


HttpDownloader::HttpDownloader( const wxString& FileUrl, const wxString& DestPath, bool deflatezipstream ) :
m_destpath(DestPath),
m_progress(0)
{
  wxHTTP FileDownloading;
  /// normal timeout is 10 minutes.. set to 10 secs.
  FileDownloading.SetTimeout(10);
  FileDownloading.Connect( FileUrl.BeforeFirst(_T('/')), 80);
  m_httpstream = FileDownloading.GetInputStream( _T("/") + FileUrl.AfterFirst(_T('/')) );
  if (FileDownloading.GetError() == wxPROTO_NOERR)
  {
    m_dialog = new wxProgressDialog( _("Download progress"), _("Downloading the requested file, please stand by"), 100, NULL, wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ESTIMATED_TIME );
    m_thread_updater = new UpdateProgressbar( *this );

  }
}

HttpDownloader::~HttpDownloader()
{
  if (m_httpstream) delete m_httpstream;
  if (m_thread_updater) delete m_thread_updater;
  if (m_dialog) delete m_dialog;
}


void HttpDownloader::OnThreadUpdate()
{
  char * buff = new char [ m_httpstream->GetSize() / 100 ];
  m_httpstream->Read( buff, m_httpstream->GetSize() / 100 );
/// NOTE (BrainDamage#1#):  wtf i need to do encoding conversion on a buffer???
  m_stringbuffer += wxString ( buff, wxCSConv(_T("ascii-8")) );
  delete []buff;
  m_progress++;
  bool closethread = m_dialog->Update( m_progress );
  if ( m_progress == 100 || closethread ) m_thread_updater->CloseThread();
  if ( m_progress == 100 ) DeflateFiles();
}


void HttpDownloader::DeflateFiles()
{
  wxStringInputStream instream(m_stringbuffer);
  wxZipInputStream zipstream(instream);
  wxZipEntry* CurrentFileInfo = zipstream.GetNextEntry();
  while ( CurrentFileInfo != NULL )
  {
    wxString StringBuffer;
    wxStringOutputStream filestream(&StringBuffer);
    zipstream.Read( filestream );
    wxFile f( m_destpath + CurrentFileInfo->GetName(), wxFile::write );
    f.Write( StringBuffer );
    f.Close();
    CurrentFileInfo = zipstream.GetNextEntry();
  }
}



UpdateProgressbar::UpdateProgressbar( HttpDownloader& CallingClass ) :
m_calling_class(CallingClass),
m_destroy(false)
{
}


void UpdateProgressbar::Init()
{
  Create();
  Run();
}


void* UpdateProgressbar::Entry()
{
  while ( !TestDestroy() )
  {
    m_calling_class.OnThreadUpdate();
  }
}


bool UpdateProgressbar::TestDestroy()
{
  return m_destroy;
}


void UpdateProgressbar::CloseThread()
{
  m_destroy = true;
}
