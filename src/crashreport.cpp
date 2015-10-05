/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Classes: NetDebugReport CrashReport
//
#include "utils/curlhelper.h" // has to be first, else compiler will warn about "please include winsock2.h before windows.h"
#include <curl/curl.h>
#include "crashreport.h"

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

#include "utils/platform.h"
#include "utils/conversion.h"
#include "utils/slpaths.h"
#include "utils/version.h"
#include "settings.h"
#include "stacktrace.h"

NetDebugReport::NetDebugReport(const char* url)
    : m_url(url)
{
}

bool NetDebugReport::Process()
{
	wxDebugReportCompress::Process(); //compress files into zip
	wxString filename = GetCompressedFileName();
	CwdGuard setCwd(wxPathOnly(filename));
	wxLogMessage(filename);
	wxStringOutputStream response;
	wxStringOutputStream rheader;
	CURL* curl_handle;
	curl_handle = curl_easy_init();
	struct curl_slist* m_pHeaders = NULL;
	struct curl_httppost* m_pPostHead = NULL;
	struct curl_httppost* m_pPostTail = NULL;
	struct curl_forms testform[2];

	// these header lines will overwrite/add to cURL defaults
	m_pHeaders = curl_slist_append(m_pHeaders, "Expect:");

	testform[0].option = CURLFORM_FILE;
	//we need to keep these buffers around for curl op duration
	wxCharBuffer filename_buffer = filename.mb_str();
	testform[0].value = (const char*)filename_buffer;
	testform[1].option = CURLFORM_END;
	curl_formadd(&m_pPostHead, &m_pPostTail, CURLFORM_COPYNAME,
		     "file",
		     CURLFORM_ARRAY, testform, CURLFORM_END);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, m_pHeaders);
	curl_easy_setopt(curl_handle, CURLOPT_URL, m_url);
	//	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "SpringLobby");
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void*)&rheader);
	curl_easy_setopt(curl_handle, CURLOPT_POST, TRUE);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, m_pPostHead);

	CURLcode ret = curl_easy_perform(curl_handle);

	wxLogError(rheader.GetString());

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	curl_formfree(m_pPostHead);

	wxString szResponse;
	if (ret == CURLE_OK)
		szResponse = wxT("SUCCESS!\n\n");
	else
		szResponse = wxT("FAILURE!\n\n");
	szResponse += wxString::Format(wxT("\nResponse Code: %d\n\n"), ret);
	szResponse += rheader.GetString();
	szResponse += wxT("\n\n");
	szResponse += response.GetString();
	szResponse += wxT("\n\n");
	wxLogMessage(szResponse);

	return ret == CURLE_OK;
}

bool NetDebugReport::OnServerReply(const wxArrayString& reply)
{
	if (reply.IsEmpty()) {
		wxLogError(_T( "Didn't receive the expected server reply." ));
		return false;
	}

	wxString s(_T( "Server replied:\n" ));
	const size_t count = reply.GetCount();
	for (size_t n = 0; n < count; n++) {
		s << _T('\t') << reply[n] << _T('\n');
	}

	wxLogMessage(_T( "%s" ), s.c_str());
	return true;
}

void SpringDebugReport::AddVFSFile(const wxString& fn, const wxString& id)
{
	const wxString file = TowxString(SlPaths::GetDataDir()) + wxFileName::GetPathSeparator() + fn;
	if (wxFile::Exists(file)) {
		AddFile(file, id);
	}
	return;
}

SpringDebugReport::SpringDebugReport()
    : NetDebugReport("http://infologs.springrts.com/upload")
{
	wxString tmp_filename = wxPathOnly(wxFileName::CreateTempFileName(_T("dummy"))) + wxFileName::GetPathSeparator() + _T("settings.txt");
	wxCopyFile(TowxString(SlPaths::GetSpringConfigFilePath()), tmp_filename);
	AddFile(tmp_filename, _T("Settings"));

	AddVFSFile(_T("infolog.txt"), _T("Infolog"));
	AddVFSFile(_T("script.txt"), _T("Script"));
	AddVFSFile(_T("ext.txt"), _T("Extensions"));
	AddVFSFile(_T("unitsync.log"), _T("unitsync"));

	wxString info;
	info << wxGetOsDescription() << (wxIsPlatform64Bit() ? _T(" 64bit\n") : _T(" 32bit\n"));
	AddText(_T("platform.txt"), info, _T("Platform"));
	AddText(_T("client.txt"), _T( "SpringLobby " ) + TowxString(getSpringlobbyVersion()), _T("Client"));
	AddText(_T("appname.txt"), TowxString(getSpringlobbyName()), _T("Application Name"));
}

void CrashReport::GenerateReport()
{
	wxLogMessage(_T( "Report generated in " ));
	CwdGuard cwgGuard(wxFileName::GetTempDir());
	NetDebugReport* report = new NetDebugReport("http://debug.springlobby.info/upload");
	//	NetDebugReport* report = new NetDebugReport( "http://localhost/upload" ) ;

	// add all standard files: currently this means just a minidump and an
	// XML file with system info and stack trace
	report->AddAll();

	wxString dir = report->GetDirectory();

	wxString SystemInfos;

	SystemInfos += _T( "SpringLobby version " ) + TowxString(getSpringlobbyVersion()) + _T( "\n" );
	SystemInfos += _T( "Built from " ) + wxString(wxVERSION_STRING) + _T(" on ") + wxPlatformInfo::Get().GetOperatingSystemIdName() + _T( "\n" );

	report->AddText(_T( "SystemInfos.txt" ), SystemInfos, _("System information"));
#if wxUSE_STD_IOSTREAM
	report->AddText(_T( "AppLog.txt" ), TowxString(crashlog.str()), _("Application verbose log"));
#endif
	wxString script_file = TowxString(SlPaths::GetLobbyWriteDir()) + _T("script.txt");
	if (wxFile::Exists(script_file))
		report->AddFile(script_file, _("Last generated spring launching script"));

#if wxUSE_STACKWALKER
	StackTrace stacktrace;
	stacktrace.Walk(3, 20);
	report->AddText(_T( "stacktrace.txt" ), _T("Call stack:\n") + stacktrace.GetStackTrace(), _("StackTrace"));
#endif

	wxString SlBuildFlags;
#ifdef DISABLE_SOUND
	SlBuildFlags += _T("sound=0");
#else
	SlBuildFlags += _T("sound=1");
#endif
	report->AddText(_T("buildflags.txt"), SlBuildFlags, _T("BuildFlags"));

	report->AddText(_T("nick.txt"),
			sett().GetServerAccountNick(sett().GetDefaultServer()), _T("Nick"));

	wxDebugReportPreviewStd* bkl = new wxDebugReportPreviewStd();
	// calling Show() is not mandatory, but is more polite
	if (bkl->Show(*report)) {
		if (report->Process()) {
			wxLogMessage(_T( "Report successfully uploaded." ));
		} else {
			wxLogMessage(_T( "Report generated in \"%s\", but failed to upload" ), report->GetCompressedFileName().c_str());
			report->Reset();
			wxLogMessage(_T("report reset"));
		}
	}
	//else: user cancelled the report

	delete report;
	wxLogMessage(_T("deleted report"));
}
