/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/sstream.h>
#include <wx/string.h>
#include <wx/protocol/http.h>
#include <wx/log.h>

#include "gui/customdialogs.h"
#include "log.h"
#include "utils/slconfig.h"

SLCONFIG("/General/UpdateUrl", "http://version.springlobby.info/current.txt", "Url to check for updates" );

//! @brief gets latest version from version.springlobby.info via HTTP
wxString GetLatestVersion()
{
	wxHTTP versionRequest;

	versionRequest.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
	const wxString updateurl = cfg().ReadString(_T("/General/UpdateUrl"));
	const wxString host = updateurl.AfterFirst('/').AfterFirst('/').BeforeFirst('/');
	const wxString url = updateurl.AfterFirst('/').AfterFirst('/').AfterFirst('/');
	// normal timeout is 10 minutes.. set to 10 secs.
	versionRequest.SetTimeout(10);
	versionRequest.Connect(host, 80);
	wxLogDebug(_T("Connecting to %s, %s"), host.c_str(), url.c_str());
	wxInputStream *stream = versionRequest.GetInputStream(_T("/") + url);
	wxString result;

	if (versionRequest.GetError() == wxPROTO_NOERR) {

		wxStringOutputStream output(&result);
		stream->Read(output);
	} else {
		wxString err;
		switch (versionRequest.GetError()) {

		case wxPROTO_NETERR:
			err = _T("Network Error");
			break;
		case wxPROTO_PROTERR:
			err = _T("Negotiation error");
			break;
		case wxPROTO_CONNERR:
			err = _T("Failed to connect to server");
			break;
		case wxPROTO_INVVAL:
			err = _T("Invalid Value");
			break;
		case wxPROTO_NOHNDLR:
			err = _T("No Handler");
			break;
		case wxPROTO_NOFILE:
			err = _T("File doesn't exit");
			break;
		case wxPROTO_ABRT:
			err = _T("Action Aborted");
			break;
		case wxPROTO_RCNCT:
			err = _T("Reconnection Error");
			break;
		default:
			err = _T("Unknown Error");
			break;
		}
		const wxString msg = wxString::Format(_T("Error connecting! Error is: %s"), err.c_str());
		wxLogError(msg);
		customMessageBoxNoModal(SL_MAIN_ICON, msg, wxEmptyString);

		return wxEmptyString;
	}

	wxDELETE(stream);
	versionRequest.Close();

	// Need to replace crap chars or versions will always be inequal
	result.Replace(_T(" "), wxEmptyString, true);
	result.Replace(_T("\n"), wxEmptyString, true);
	result.Replace(_T("\t"), wxEmptyString, true);

	return result;
}
