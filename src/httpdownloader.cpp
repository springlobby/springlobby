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
#include "utils.h"


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
    m_dialog = new wxProgressDialog( _("Download progress"), _("Downloading the requested file, please stand by"), m_httpstream->GetSize(), NULL, wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ESTIMATED_TIME );
    m_thread_updater = new UpdateProgressbar( *this );
    m_thread_updater->Init();
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
  if(m_httpstream == 0)
  {
    m_thread_updater->CloseThread();
    m_thread_updater = 0;
    return;
  }
  char* buff = new char [1024];
  if(m_httpstream != 0)
    m_httpstream->Read( buff, 1024 );
  unsigned int BytesRead = m_httpstream->LastRead();
//  m_progress = m_progress + BytesRead;
  m_stringbuffer += std::string(buff, buff + BytesRead);
   m_progress = m_stringbuffer.size();
  delete []buff;
  bool closethread = !m_dialog->Update( m_progress );
  if ( m_httpstream->GetSize() == m_progress || closethread )
  {
     m_thread_updater->CloseThread();
     m_thread_updater = 0;
  }
  if ( m_progress == m_httpstream->GetSize() ) DeflateFiles();
}


void HttpDownloader::DeflateFiles()
{
  wxStringInputStream instream(wxString(m_stringbuffer.c_str(), wxCSConv(_T("latin-1")) )); /// NOTE (BrainDamage#1#): wtf i have to do encoding conversion in a binary stream
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
  Init();
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
