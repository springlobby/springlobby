/* Copyright (C) 2007-2011 The SpringLobby Team. All rights reserved. */
#include "misc.h"

#include <lslutils/misc.h>
#include "../settings.h"

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/log.h>
#include <wx/tokenzr.h>
#include <wx/sstream.h>
#include <wx/filename.h>
#include <vector>
#include "curlhelper.h"

double LevenshteinDistance(wxString s, wxString t)
{
    s.MakeLower(); // case insensitive edit distance
    t.MakeLower();

    const int m = s.length(), n = t.length(), _w = m + 1;
    std::vector<unsigned char> _d((m + 1) * (n + 1));
#define D(x, y) _d[(y) * _w + (x)]

    for (int i = 0; i <= m; ++i) D(i,0) = i;
    for (int j = 0; j <= n; ++j) D(0,j) = j;

    for (int i = 1; i <= m; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            const int cost = (s[i - 1] != t[j - 1]);
            D(i,j) = LSL::Util::Min(D(i-1,j) + 1, // deletion
                         D(i,j-1) + 1, // insertion
                         D(i-1,j-1) + cost); // substitution
        }
    }
    double d = (double) D(m,n) / std::max(m, n);
    wxLogMessage( _T("LevenshteinDistance('%s', '%s') = %g"), s.c_str(), t.c_str(), d );
    return d;
#undef D
}

wxString GetBestMatch(const wxArrayString& a, const wxString& s, double* distance )
{
    const unsigned int count = a.GetCount();
    double minDistance = 1.0;
    int minDistanceIndex = -1;
    for (unsigned int i = 0; i < count; ++i)
    {
        const double distance_ = LevenshteinDistance(a[i], s);
        if (distance_ < minDistance)
        {
            minDistance = distance_;
            minDistanceIndex = i;
        }
    }
    if (distance != NULL) *distance = minDistance;
    if (minDistanceIndex == -1) return _T("");
    return a[minDistanceIndex];
}

wxString Paste2Pastebin( const wxString& message )
{
	#ifndef SL_QT_MODE
	#ifndef __WXMAC__
	wxStringOutputStream response;
	wxStringOutputStream rheader;
	CURL *curl_handle;
	curl_handle = curl_easy_init();
	struct curl_slist* m_pHeaders = NULL;
	struct curl_httppost*   m_pPostHead = NULL;
	struct curl_httppost*   m_pPostTail = NULL;
    static const char* url = "http://paste.springfiles.com/api/create";
	// these header lines will overwrite/add to cURL defaults
	m_pHeaders = curl_slist_append(m_pHeaders, "Expect:") ;

	//we need to keep these buffers around for curl op duration
	wxCharBuffer message_buffer = message.mb_str();
	wxCharBuffer nick_buffer = sett().GetServerAccountNick( sett().GetDefaultServer() ).mb_str();

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
	curl_easy_setopt(curl_handle, CURLOPT_URL, url );
//	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
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

	if(ret == CURLE_OK)
		return response.GetString();
	else
	#endif
	#endif

	return wxEmptyString;
}

/** Try to create the named directory, if it doesn't exist.
 *
 * @param name Path to directory that should exist or be created.
 *
 * @param perm Value of @p perm parameter for wxFileName::Mkdir.
 *
 * @param flags Value of @p flags parameter for wxFileName::Mkdir.
 *
 * @return @c true if the directory already exists, or the return
 * value of wxFileName::Mkdir if it does not.
 */
bool tryCreateDirectory( const wxString& name, int perm, int flags )
{
    if ( wxFileName::DirExists( name ) )
        return true;
    else
        return wxFileName::Mkdir( name, perm, flags );
}
