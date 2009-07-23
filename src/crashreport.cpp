/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Classes: NetDebugReport CrashReport
//


//#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)

#include "crashreport.h"

#include <wx/intl.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/platinfo.h>

#include "utils/platform.h"
#include "utils/conversion.h"
#include "settings.h"
#include "curl/http.h"


NetDebugReport::NetDebugReport()
{

}

bool NetDebugReport::Process()
{
    wxDebugReportCompress::Process(); //compress files into zip
    wxString filename = GetCompressedFileName();

    wxCurlHTTP http( _T("http://localhost/upload.php") );
    struct curl_forms testform[2];

			wxString szValue = wxT("/tmp/pic.png");
			testform[0].option = CURLFORM_COPYCONTENTS;
			testform[0].value = szValue.c_str();
			testform[1].option = CURLFORM_END;

			assert(http.AddForm(false, _T(wxT("firstname")), testform));

//			if(http.Post(szData.ToAscii(), szData.Len()))
			assert(http.Post());
}

bool NetDebugReport::OnServerReply( const wxArrayString& reply )
{
	if ( reply.IsEmpty() )
	{
		wxLogError( _T( "Didn't receive the expected server reply." ) );
		return false;
	}

	wxString s( _T( "Server replied:\n" ) );

	const size_t count = reply.GetCount();
	for ( size_t n = 0; n < count; n++ )
	{
		s << _T( '\t' ) << reply[n] << _T( '\n' );
	}

	wxLogMessage( _T( "%s" ), s.c_str() );

	return true;
}

void CrashReport::GenerateReport( )
{
    wxLogMessage( _T( "Report generated in " ) );
	wxSetWorkingDirectory( wxFileName::GetTempDir() );
#if defined(__WXMSW__)
	bool online = false; // TODO (BrainDamage#1#): check if being online
#else
	bool online = true; // TODO (BrainDamage#1#): check if being online
#endif
	wxDebugReportCompress* report = online  ? new NetDebugReport
	                                : new wxDebugReportCompress;

	// add all standard files: currently this means just a minidump and an
	// XML file with system info and stack trace
	report->AddAll(  );

	wxString dir = report->GetDirectory();

	wxString SystemInfos;

	SystemInfos += _T( "SpringLobby version " ) + GetSpringLobbyVersion() + _T( "\n" ) ;
	SystemInfos += _T( "Built from " ) + wxString( wxVERSION_STRING ) + _T(" on ") + wxPlatformInfo::Get().GetOperatingSystemIdName() + _T( "\n" ) ;

	report->AddText( _T( "SystemInfos.txt" ), SystemInfos, _( "System informations" ) );
#if wxUSE_STD_IOSTREAM
	report->AddText( _T( "AppLog.txt" ), TowxString( crashlog.str() ), _( "Application verbose log" ) );
#endif
	report->AddFile( sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("script.txt"), _( "Last generated spring launching script" ) );

    wxDebugReportPreviewStd* bkl = new wxDebugReportPreviewStd();
	// calling Show() is not mandatory, but is more polite
	if ( bkl->Show( *report ) )
	{
		if ( report->Process() )
		{
			if ( online )
			{
				wxLogMessage( _T( "Report successfully uploaded." ) );
			}
			else
			{
				wxLogMessage( _T( "Report generated in \"%s\"." ),
				              report->GetCompressedFileName().c_str() );
				report->Reset();
			}
		}
	}
	//else: user cancelled the report

	delete report;
}



//#endif // wxUSE_DEBUGREPORT
