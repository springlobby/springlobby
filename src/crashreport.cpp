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
#include <wx/dialup.h>

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

  wxDialUpManager* network;
  bool online = network->IsAlwaysOnline() || network->IsOnline();
  delete network;

  wxDebugReportCompress *report = online   ? new NetDebugReport
                                           : new wxDebugReportCompress;

  // add all standard files: currently this means just a minidump and an
  // XML file with system info and stack trace
  report->AddAll(ctx);

  wxString dir = report->GetDirectory();

  wxString SystemInfos;
#ifdef VERSION
  SystemInfos += _T("SpringLobby version ") + WX_STRING( GetSpringLobbyVersion() ) +_T("\n") ;
#endif
  SystemInfos += _T("Built from ") + wxString(wxVERSION_STRING) + _T("\n") ;

  report->AddText( _T("SystemInfos.txt"), SystemInfos, _("System informations") );

  // calling Show() is not mandatory, but is more polite
  if ( wxDebugReportPreviewStd().Show(*report) )
  {
    if ( report->Process() )
    {
      if ( online )
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
  wxLogError( _("The application has generated a fatal error and will be terminated\nGenerating a stacktrace is not possible\n\nplease enable wxUSE_DEBUGREPORT"),_("Critical error"), wxICON_ERROR );
}

#endif// !wxUSE_DEBUGREPORT

CrashReport& crashreport()
{
  static CrashReport c;
  return c;
}
