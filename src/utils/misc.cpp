/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */
#include "misc.h"

#include "math.h"
#include "../settings.h"

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/log.h>
#include <wx/tokenzr.h>

#include <vector>

#if defined(ENABLE_DEBUG_REPORT)
    #include "../curl/http.h"
#endif

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
            D(i,j) = min(D(i-1,j) + 1, // deletion
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



//typedef int (*func_T)(void);
//bool wxCurlBase::SetOpt(CURL* m_pCURL,CURLoption option, ...)
//{
//    va_list arg;
//
//    func_T param_func = (func_T)0;
//    long param_long = 0;
//    void *param_obj = NULL;
//    curl_off_t param_offset = 0;
//
//    va_start(arg, option);
//
//    CURLcode res = CURLE_OK;
//
//    // This code stolen from easy.c from LibCURL - It is needed to ensure that
//    // types are maintained.
//    if(option < CURLOPTTYPE_OBJECTPOINT) {
//        /* This is a LONG type */
//        param_long = va_arg(arg, long);
//        res = curl_easy_setopt(m_pCURL, option, param_long);
//    }
//    else if(option < CURLOPTTYPE_FUNCTIONPOINT) {
//        /* This is a object pointer type */
//        param_obj = va_arg(arg, void *);
//        res = curl_easy_setopt(m_pCURL, option, param_obj);
//    }
//    else if(option < CURLOPTTYPE_OFF_T) {
//        /* This is a function pointer type */
//        param_func = va_arg(arg, func_T );
//        res = curl_easy_setopt(m_pCURL, option, param_func);
//    } else {
//        /* This is a curl_off_t type */
//        param_offset = va_arg(arg, curl_off_t);
//        res = curl_easy_setopt(m_pCURL, option, param_offset);
//    }
//
//    va_end(arg);
//
//    DumpErrorIfNeed(res);
//    return (res == CURLE_OK);
//}

#if defined(ENABLE_DEBUG_REPORT)

wxString Paste2Pastebin( const wxString& message )
{
//    wxCurlHTTP http( _T("http://pastebin.ca/quiet-paste.php?api=rTsNvBZbuYULv8TXZKL2XZKUwh4bkU1t") );

    wxCurlHTTP http( _T("http://sl.pastebin.com/") );
    wxCharBuffer response;
    wxCharBuffer header;
    CURL * easyhandle = curl_easy_init();
    char *data="paste=Send&code2=curlMF";
    wxString fields = _T("paste=Send&code2=") + message ;
    curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, (const char*)fields.mb_str());
//    curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(easyhandle, CURLOPT_URL, "http://sl.pastebin.com/");
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, wxcurl_string_write);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(easyhandle, CURLOPT_HEADERFUNCTION, wxcurl_header_func);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEHEADER, &header);

    curl_easy_perform(easyhandle);

    wxStringTokenizer header_str( wxCURL_BUF2STRING( header ), _T("\n") );
    curl_easy_cleanup( easyhandle );

    wxLogMessage( wxCURL_BUF2STRING( header ) );
    wxLogMessage( wxCURL_BUF2STRING( response ) );

    while ( header_str.HasMoreTokens() ) {
        wxString line = header_str.GetNextToken();
        if ( line.StartsWith( _T("Location: ") ) )
            return line.Right( line.Len() - 10  );
    }

    return _T("");

}
#endif

