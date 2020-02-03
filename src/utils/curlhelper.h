/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CURLHELPER_H
#define CURLHELPER_H

#include <wx/sstream.h>
#include <wx/string.h>
#define wxCURL_BUF2STRING(x) wxString((const char*)(x), wxConvLibc)
#if wxUSE_UNICODE
#define wxCURL_STRING2BUF(x) ((x).ToAscii())
#else
#define wxCURL_STRING2BUF(x) wxCharBuffer((x).c_str())
#endif


extern "C" {
size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream);
}

wxString Paste2Pastebin(const wxString& message);
wxString Paste2Logs(const wxString& body);

#endif // CURLHELPER_H
