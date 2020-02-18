/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/log.h>
#include "curlhelper.h"
#include "settings.h"

#include <curl/curl.h>
#include <curl/easy.h>
#include <json/writer.h>

#include "downloader/prdownloader.h"
#include "downloader/lib/src/Downloader/CurlWrapper.h"
#include "utils/conversion.h"
#include "utils/version.h"

extern "C" {
size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream)
{
	size_t iRealSize = size * nmemb;

	wxOutputStream* pBuf = (wxOutputStream*)stream;

	if (pBuf) {
		pBuf->Write(ptr, iRealSize);

		return pBuf->LastWrite();
	}

	return 0;
}
} //end extern "C"


wxString Paste2Logs(const wxString& body, const wxString& comment)
{
	wxStringOutputStream response;
	wxStringOutputStream rheader;
	prDownloader().UpdateSettings();
	CurlWrapper cw;
	CURL* curl_handle = cw.GetHandle();
	struct curl_slist* m_pHeaders = NULL;
	static const char* url = "http://logs.springrts.com/logfiles/";
	// these header lines will overwrite/add to cURL defaults
	m_pHeaders = curl_slist_append(m_pHeaders, "Content-Type: application/json");
	// Necessary, otherwise the server returns error 417 for long POSTs.
	m_pHeaders = curl_slist_append(m_pHeaders, "Expect:");

	//we need to keep these buffers around for curl op duration
	wxString myNick = sett().GetServerAccountNick(sett().GetDefaultServer());
	wxString name(_T("Springlobby engine crash report"));
	if (!comment.IsEmpty()) {
		name += _(": ");
		name += comment;
	}

	Json::Value root;
	root["name"] = STD_STRING(name);
	root["text"] = STD_STRING(body);
	//root["tags"].append("type=test");
	root["tags"].append(STD_STRING(wxString::Format("lobbyNick=%s", myNick)));
	root["tags"].append(STD_STRING(wxString::Format("lobbyClientName=%s", GetSpringlobbyName(/*lowercase=*/true))));
	root["tags"].append(STD_STRING(wxString::Format("lobbyClientVersion=%s", GetSpringlobbyVersion())));

	std::string serialised(root.toStyledString());
	//wxLogDebug("JSON: %s", serialised);

	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, m_pHeaders);
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, serialised.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void*)&rheader);

	CURLcode ret = curl_easy_perform(curl_handle);
	curl_slist_free_all(m_pHeaders);

	if (ret == CURLE_OK) {
		return response.GetString();
	}

	wxLogError(rheader.GetString());
	return wxEmptyString;
}


wxString Paste2Pastebin(const wxString& message)
{
	wxStringOutputStream response;
	wxStringOutputStream rheader;
	prDownloader().UpdateSettings();
	CurlWrapper cw;
	CURL* curl_handle = cw.GetHandle();
	struct curl_slist* m_pHeaders = NULL;
	struct curl_httppost* m_pPostHead = NULL;
	struct curl_httppost* m_pPostTail = NULL;
	static const char* url = "https://paste.springfiles.com/api/create";
	// these header lines will overwrite/add to cURL defaults
	m_pHeaders = curl_slist_append(m_pHeaders, "Expect:");

	//we need to keep these buffers around for curl op duration
	wxCharBuffer message_buffer = message.mb_str();
	wxCharBuffer nick_buffer = sett().GetServerAccountNick(sett().GetDefaultServer()).mb_str();

	curl_formadd(&m_pPostHead,
		     &m_pPostTail,
		     CURLFORM_COPYNAME, "text",
		     CURLFORM_COPYCONTENTS, (const char*)message_buffer,
		     CURLFORM_END);
	curl_formadd(&m_pPostHead,
		     &m_pPostTail,
		     CURLFORM_COPYNAME, "private",
		     CURLFORM_COPYCONTENTS, "1",
		     CURLFORM_END);
	curl_formadd(&m_pPostHead,
		     &m_pPostTail,
		     CURLFORM_COPYNAME, "name",
		     CURLFORM_COPYCONTENTS, (const char*)nick_buffer,
		     CURLFORM_END);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, m_pHeaders);
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	//	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&response);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void*)&rheader);
	curl_easy_setopt(curl_handle, CURLOPT_POST, TRUE);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, m_pPostHead);

	CURLcode ret = curl_easy_perform(curl_handle);

	/* cleanup curl stuff */
	curl_formfree(m_pPostHead);

	if (ret == CURLE_OK) {
		return response.GetString();
	}

	wxLogError(rheader.GetString());
	return wxEmptyString;
}
