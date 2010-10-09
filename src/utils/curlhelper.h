#ifndef CURLHELPER_H
#define CURLHELPER_H

#include <wx/string.h>
#include <wx/sstream.h>

#define wxCURL_BUF2STRING(x)         wxString((const char*)(x), wxConvLibc)
#if wxUSE_UNICODE
	#define wxCURL_STRING2BUF(x)         ((x).ToAscii())
#else
	#define wxCURL_STRING2BUF(x)         wxCharBuffer((x).c_str())
#endif

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

extern "C"
{
	int wxcurl_evt_progress_func(void* ptr, double rDlTotal, double rDlNow,
								 double rUlTotal, double rUlNow);
	int wxcurl_verbose_stream_write (CURL * crlptr , curl_infotype info, char * cStrMessage,
									 size_t msgSize, void * buffer);
	size_t wxcurl_header_func(void *ptr, size_t size, size_t nmemb, void *stream);

	size_t wxcurl_string_write(void* ptr, size_t size, size_t nmemb, void* stream);
	size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream);

	size_t wxcurl_string_read(void* ptr, size_t size, size_t nmemb, void* stream);
	size_t wxcurl_stream_read(void* ptr, size_t size, size_t nmemb, void* stream);
}

#endif // CURLHELPER_H
