/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Classes: NetDebugReport CrashReport
//
#include "crashreport.h"

#if wxUSE_DEBUGREPORT && defined(ENABLE_DEBUG_REPORT)

#include <wx/intl.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/platinfo.h>

#include "updater/updatehelper.h"
#include "utils/conversion.h"
#include "settings.h"
#include "curl/http.h"
#include "stacktrace.h"

#if ! wxUSE_STACKWALKER
    #include "utils/stack.h"
    #include <algorithm>
    #include <iostream>
    #include <iterator>
#endif

NetDebugReport::NetDebugReport( const wxString& url )
	: m_url( url )
{
}

bool NetDebugReport::Process()
{
    wxDebugReportCompress::Process(); //compress files into zip
    wxString filename = GetCompressedFileName();
    CwdGuard setCwd( wxPathOnly( filename ) );
	wxLogMessage( filename );
	wxCurlHTTP http( m_url );
    struct curl_forms testform[2];

	testform[0].option = CURLFORM_FILE;
	//a cookie for him who riddles me the not working of next, but working second line
	//testform[0].value = filename.mb_str();
	testform[0].value = "springlobby.zip";
    testform[1].option = CURLFORM_END;

	http.AddForm(true, _T("file"), testform);

//			if(http.Post(szData.ToAscii(), szData.Len()))
    wxString szResponse;
	if(http.Post()) {
        wxMemoryOutputStream outStream;

        wxString szVerbose;
        http.GetVerboseString(szVerbose);

        szResponse = wxT("SUCCESS!\n\n");
        szResponse += http.GetResponseBody();
        szResponse += wxT("\n\nVERBOSE DATA:\n");
        szResponse += szVerbose;
        wxLogMessage( szResponse );
        return true;
    }
	else {
        szResponse = wxT("FAILURE!\n\n");
        szResponse += wxString::Format(wxT("\nResponse Code: %d\n\n"), http.GetResponseCode());
        szResponse += http.GetResponseHeader();
        szResponse += wxT("\n\n");
        szResponse += http.GetResponseBody();
        szResponse += wxT("\n\n");
        szResponse += http.GetErrorString();
        wxLogMessage( szResponse );
        return false;
    }

}

bool NetDebugReport::OnServerReply( const wxArrayString& reply )
{
	if ( reply.IsEmpty() ) {
		wxLogError( _T( "Didn't receive the expected server reply." ) );
		return false;
	}

	wxString s( _T( "Server replied:\n" ) );
	const size_t count = reply.GetCount();
	for ( size_t n = 0; n < count; n++ ) {
		s << _T( '\t' ) << reply[n] << _T( '\n' );
	}

	wxLogMessage( _T( "%s" ), s.c_str() );
	return true;
}

SpringDebugReport::SpringDebugReport()
	: NetDebugReport( _T("http://127.0.0.1/upload") )
{
	wxString dir = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator();
	wxString tmp_filename = wxPathOnly( wxFileName::CreateTempFileName(_T("dummy")) ) + wxFileName::GetPathSeparator() + _T("settings.txt");
	wxCopyFile( sett().GetCurrentUsedSpringConfigFilePath(), tmp_filename );
	AddFile( dir + _T("infolog.txt"), _T("Infolog") );
	AddFile( dir + _T("script.txt"), _T("Script") );
	AddFile( dir + _T("ext.txt"), _T("Infolog") );
	AddFile( dir + _T("unitsync.log"), _T("Infolog") );
	AddFile( tmp_filename, _T("Settings") );
	wxString info;
	info << wxGetOsDescription() << ( wxIsPlatform64Bit() ? _T(" 64bit\n") : _T(" 32bit\n") );
	AddText( _T("platform.txt"), info, _T("Platform") );
}

#if wxUSE_STACKWALKER
    void CrashReport::GenerateReport()
#else
    void CrashReport::GenerateReport(EXCEPTION_POINTERS* p)
#endif
{
    wxLogMessage( _T( "Report generated in " ) );
	wxSetWorkingDirectory( wxFileName::GetTempDir() );
#if defined(__WXMSW__)
	bool online = false; // TODO (BrainDamage#1#): check if being online
#else
	bool online = true; // TODO (BrainDamage#1#): check if being online
#endif
	NetDebugReport* report = new NetDebugReport( _T("http://debug.springlobby.info/upload.php") );
//	online  ? new NetDebugReport //Process() is not virtual --> THIS WAS FAIL
//	                                : new wxDebugReportCompress;

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
    wxString script_file = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("script.txt");
    if ( wxFile::Exists( script_file ) )
        report->AddFile( script_file, _( "Last generated spring launching script" ) );

#if wxUSE_STACKWALKER
    StackTrace stacktrace;
    stacktrace.Walk( 2, 20 );
    report->AddText( _T( "stacktrace.txt" ), stacktrace.GetStackTrace(), _( "StackTrace" ) );
#else
    dbg::stack stack;
    std::stringstream stack_str;
    std::copy(stack.begin(), stack.end(), std::ostream_iterator<dbg::stack_frame>(stack_str, "\n"));

    wxString trace_str ( TowxString( stack_str.str() ) );
    report->AddText( _T( "stacktrace.txt" ), trace_str, _( "StackTrace" ) );
#endif

    wxDebugReportPreviewStd* bkl = new wxDebugReportPreviewStd();
	// calling Show() is not mandatory, but is more polite
	if ( bkl->Show( *report ) ) {
		if ( report->Process() ) {
			if ( online ) {
				wxLogMessage( _T( "Report successfully uploaded." ) );
			}
			else {
				wxLogMessage( _T( "Report generated in \"%s\"." ),
				              report->GetCompressedFileName().c_str() );
				report->Reset();
			}
		}
		else {
		    wxLogMessage( _T( "Report generated in \"%s\", but failed to upload" ),
				              report->GetCompressedFileName().c_str() );
				report->Reset();
				wxLogMessage( _T("report reset") );
		}
	}
	//else: user cancelled the report

	delete report;
	wxLogMessage( _T("deleted report") );
}


#endif // wxUSE_DEBUGREPORT
