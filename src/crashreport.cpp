/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Classes: NetDebugReport CrashReport StackTrace
//


#include <wx/msgdlg.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>


#include "crashreport.h"
#include "utils.h"

#if wxUSE_DEBUGREPORT

NetDebugReport::NetDebugReport() : wxDebugReportUpload ( _T("http://trac.springlobby.info/attachment/ticket/236"), _T("attachment"), _T("?action=new") )
{
}

bool NetDebugReport::OnServerReply(const wxArrayString& reply)
{
    if ( reply.IsEmpty() )
    {
        wxLogError(_T("Didn't receive the expected server reply."));
        return false;
    }

    wxString s(_T("Server replied:\n"));

    const size_t count = reply.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        s << _T('\t') << reply[n] << _T('\n');
    }

    wxLogMessage(_T("%s"), s.c_str());

    return true;
}

void CrashReport::GenerateReport(wxDebugReport::Context ctx)
{
  wxSetWorkingDirectory( wxFileName::GetTempDir() );

  wxDebugReportCompress *report = true          ? new NetDebugReport /// TODO (cloud#1#): check if online
                                                 : new wxDebugReportCompress;

  // add all standard files: currently this means just a minidump and an
  // XML file with system info and stack trace
  report->AddAll(ctx);

wxString dir = report->GetDirectory();

#ifdef VERSION
  report->AddText( _T("AppVersion.txt"), WX_STRING( GetSpringLobbyVersion() ), _("build version") );
#endif


  // calling Show() is not mandatory, but is more polite
  if ( wxDebugReportPreviewStd().Show(*report) )
  {
    if ( report->Process() )
    {
      if ( true ) /// TODO (BrainDamage#1#): check if the network works
      {
        wxLogMessage(_T("Report successfully uploaded."));
      }
      else
      {
        wxLogMessage(_T("Report generated in \"%s\"."),
                     report->GetCompressedFileName().c_str());
        report->Reset();
      }
    }
  }
  //else: user cancelled the report

  delete report;
}

#else // wxUSE_DEBUGREPORT

void CrashReport::GenerateReport(wxDebugReport::Context ctx)
{
  wxMessageBox( _("The application has generated a fatal error and will be terminated\nGenerating a stacktrace is not possible\n\nplease enable wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR );
}

#endif// !wxUSE_DEBUGREPORT

CrashReport& crashreport()
{
  static CrashReport c;
  return c;
}
