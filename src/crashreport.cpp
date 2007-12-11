/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: CrashReport
//

#include <wx/debugrpt.h>
#include <wx/msgdlg.h>

#include "crashreport.h"
#include "utils.h"

#if wxUSE_DEBUGREPORT

void CrashReport::GenerateReport(wxDebugReport::Context ctx)
{
    wxDebugReportCompress *report = m_uploadReport ? new DebugReport
                                                   : new wxDebugReportCompress;

    // add all standard files: currently this means just a minidump and an
    // XML file with system info and stack trace
    report->AddAll(ctx);


    // calling Show() is not mandatory, but is more polite
    if ( wxDebugReportPreviewStd().Show(*report) )
    {
        if ( report->Process() )
        {
            if ( m_uploadReport ) /// TODO (BrainDamage#1#): check if the network
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
