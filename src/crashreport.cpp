/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Classes: NetDebugReport CrashReport
//


#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)

#include "crashreport.h"
#include <wx/intl.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>


NetDebugReport::NetDebugReport() : wxDebugReportUpload ( _T("http://www.hd.chalmers.se/~tc/trace/"), _T("trace"), _T("upload.php") )
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

  bool online = true; // TODO (BrainDamage#1#): check if being online

  wxDebugReportCompress *report = online   ? new NetDebugReport
                                           : new wxDebugReportCompress;

  // add all standard files: currently this means just a minidump and an
  // XML file with system info and stack trace
  report->AddAll(ctx);

  wxString dir = report->GetDirectory();

  wxString SystemInfos;
#ifdef VERSION
  SystemInfos += _T("SpringLobby version ") + GetSpringLobbyVersion() +_T("\n") ;
#endif
  SystemInfos += _T("Built from ") + wxString(wxVERSION_STRING) + _T("\n") ;

  report->AddText( _T("SystemInfos.txt"), SystemInfos, _("System informations") );
#if wxUSE_STD_IOSTREAM
  report->AddText( _T("AppLog.txt"), WX_STRING( crashlog.str() ), _("Application verbose log") );
#endif
  report->AddFile( wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("script_debug.txt"), _("Last generated spring launching script") );

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



#endif // wxUSE_DEBUGREPORT
