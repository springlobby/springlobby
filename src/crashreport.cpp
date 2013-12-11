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
#include <wx/sstream.h>
#include <wx/utils.h>

#include "utils/curlhelper.h"
#include "utils/platform.h"
#include "updater/updatehelper.h"
#include "utils/conversion.h"
#include "settings.h"
#include "stacktrace.h"

NetDebugReport::NetDebugReport( const char* url )
	: m_url( url )
{
}

bool NetDebugReport::Process()
{
    wxDebugReportCompress::Process(); //compress files into zip
    wxString filename = GetCompressedFileName();
    CwdGuard setCwd( wxPathOnly( filename ) );
	wxLogMessage( filename );
	wxStringOutputStream response;
	wxStringOutputStream rheader;
	CURL *curl_handle;
	curl_handle = curl_easy_init();
	struct curl_slist* m_pHeaders = NULL;
	struct curl_httppost*   m_pPostHead = NULL;
	struct curl_httppost*   m_pPostTail = NULL;
	struct curl_forms testform[2];

	// these header lines will overwrite/add to cURL defaults
	m_pHeaders = curl_slist_append(m_pHeaders, "Expect:") ;

	testform[0].option = CURLFORM_FILE;
	//we need to keep these buffers around for curl op duration
	wxCharBuffer filename_buffer = filename.mb_str();
	testform[0].value = (const char*)filename_buffer;
	testform[1].option = CURLFORM_END;
	curl_formadd(&m_pPostHead, &m_pPostTail, CURLFORM_COPYNAME,
						   "file",
						   CURLFORM_ARRAY, testform, CURLFORM_END);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, m_pHeaders);
	curl_easy_setopt(curl_handle, CURLOPT_URL, m_url );
//	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "SpringLobby");
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&rheader);
	curl_easy_setopt(curl_handle, CURLOPT_POST, TRUE);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, m_pPostHead);

	CURLcode ret = curl_easy_perform(curl_handle);

	wxLogError( rheader.GetString()  );

  /* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	curl_formfree(m_pPostHead);

	wxString szResponse;
	if(ret == CURLE_OK)
		szResponse = wxT("SUCCESS!\n\n");
	else
		szResponse = wxT("FAILURE!\n\n");
	szResponse += wxFormat(wxT("\nResponse Code: %d\n\n") ) % ret;
	szResponse += rheader.GetString();
	szResponse += wxT("\n\n");
	szResponse += response.GetString();
	szResponse += wxT("\n\n");
	wxLogMessage( szResponse );

	return ret == CURLE_OK;
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

void SpringDebugReport::AddVFSFile( const wxString& fn, const wxString& id )
{
	wxString dir = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator();
	AddFile( dir + fn, id );
	return;
}

SpringDebugReport::SpringDebugReport()
	: NetDebugReport( "http://infologs.springrts.com/upload" )
{
	wxString tmp_filename = wxPathOnly( wxFileName::CreateTempFileName(_T("dummy")) ) + wxFileName::GetPathSeparator() + _T("settings.txt");
	wxCopyFile( sett().GetCurrentUsedSpringConfigFilePath(), tmp_filename );
	AddFile( tmp_filename, _T("Settings") );

	AddVFSFile( _T("infolog.txt"),		_T("Infolog") );
	AddVFSFile( _T("script.txt"),		_T("Script") );
	AddVFSFile( _T("ext.txt"),			_T("Extensions") );
	AddVFSFile( _T("unitsync.log"),		_T("unitsync") );

	wxString info;
	info << wxGetOsDescription() << ( wxIsPlatform64Bit() ? _T(" 64bit\n") : _T(" 32bit\n") );
	AddText( _T("platform.txt"), info, _T("Platform") );
	AddText( _T("client.txt"), _T( "SpringLobby " ) + GetSpringLobbyVersion(), _T("Client") );
	AddText( _T("appname.txt"), GetAppName(), _T("Application Name"));
}

#if wxUSE_STACKWALKER && !__WXMSW__
    void CrashReport::GenerateReport()
#else
    void CrashReport::GenerateReport(EXCEPTION_POINTERS* p)
#endif
{
    wxLogMessage( _T( "Report generated in " ) );
	CwdGuard cwgGuard( wxFileName::GetTempDir() );
	NetDebugReport* report = new NetDebugReport( "http://debug.springlobby.info/upload" ) ;
//	NetDebugReport* report = new NetDebugReport( "http://localhost/upload" ) ;

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

#if wxUSE_STACKWALKER && !__WXMSW__
    StackTrace stacktrace;
	stacktrace.Walk( 3, 20 );
	report->AddText( _T( "stacktrace.txt" ), _T("Call stack:\n") + stacktrace.GetStackTrace(), _( "StackTrace" ) );
#else
	wxString report_fn = ( wxGetCwd() + wxFileName::GetPathSeparator() + _T("stacktrace.txt") );
	if ( wxFile::Exists( report_fn ) )
		wxRemoveFile( report_fn );
	wxCharBuffer report_fn_char = report_fn.mb_str();
	DrMingwGenerateStacktrace( p, (const char*)report_fn_char );
	report->AddFile( report_fn, _( "StackTrace" ) );
#endif

	wxString SlBuildFlags;
	#ifdef DISABLE_SOUND
		SlBuildFlags += _T("sound=0");
	#else
		SlBuildFlags += _T("sound=1");
	#endif
	report->AddText( _T("buildflags.txt"), SlBuildFlags, _T("BuildFlags") );

	report->AddText( _T("nick.txt"),
		sett().GetServerAccountNick( sett().GetDefaultServer() ), _T("Nick") );

	wxDebugReportPreviewStd* bkl = new wxDebugReportPreviewStd();
	// calling Show() is not mandatory, but is more polite
	if ( bkl->Show( *report ) ) {
		if ( report->Process() ) {
			wxLogMessage( _T( "Report successfully uploaded." ) );
		}
		else {
				wxLogMessage( _T( "Report generated in \"%s\", but failed to upload" ), report->GetCompressedFileName().c_str() );
				report->Reset();
				wxLogMessage( _T("report reset") );
		}
	}
	//else: user cancelled the report

	delete report;
	wxLogMessage( _T("deleted report") );
}


#endif // wxUSE_DEBUGREPORT
