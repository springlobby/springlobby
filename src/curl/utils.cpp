/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:     wxCurlSizeQueryThread
// Author:      Francesco Montorsi
// Created:     2005/04/23
// RCS-ID:      $Id: utils.cpp 636 2007-07-08 21:23:35Z frm $
// Copyright:   (c) 2007 Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// includes
#ifndef WX_PRECOMP
    #include <wx/log.h>
#endif

#include "utils.h"
#include "http.h"
#include "ftp.h"


DEFINE_EVENT_TYPE(wxCURL_SIZE_QUERY_EVENT)



// -----------------------------------
// wxCurlSizeQueryOutput
// -----------------------------------

extern "C"
{
    int wxcurl_size_query_progress_func(void* ptr, double rDlTotal, double WXUNUSED(rDlNow),
                                        double WXUNUSED(rUlTotal), double WXUNUSED(rUlNow))
    {
        unsigned long *p = (unsigned long *)ptr;
        if (p) *p = (unsigned long)rDlTotal;

        // a non-zero value means to abort the transfer
        // and that's what we want as we don't want to download the
        // entire file but just get the rDlTotal info from the server
        return 1;
    }
}

// ---------------------
// wxCurlSizeQueryThread
// ---------------------

void *wxCurlSizeQueryThread::Entry()
{
    wxLogDebug(wxT("wxSizeCacherThread::Entry - caching file sizes"));

    wxMemoryOutputStream os;
    bool allok = true;
    wxCurlHTTP http;
    wxCurlFTP ftp;

    m_urlSize.Clear();
    for (size_t i=0; i<m_urls.GetCount() && !TestDestroy(); i++)
    {
        unsigned long sz;

        wxCurlProtocol prot = GetProtocolFromURL(m_urls[i]);
        switch (prot)
        {
            case wxCP_HTTP:
            {
                http.OverrideProgressCallback(wxcurl_size_query_progress_func, &sz);
                allok &= http.Get(os, m_urls[i]);
            }
            break;

            case wxCP_FTP:
            {
                ftp.OverrideProgressCallback(wxcurl_size_query_progress_func, &sz);
                allok &= ftp.Get(os, m_urls[i]);
            }
            break;

            default:
                wxFAIL;
        }

        m_urlSize.Add(sz);

        // send the event
        wxCurlSizeEvent ev(GetId(), m_urls[i], sz);
        wxPostEvent(GetEvtHandler(), ev);
    }

    wxLogDebug(wxT("wxSizeCacherThread::Entry - caching of file sizes completed"));
    return (void *)allok;
}

void wxCurlSizeQueryThread::OnExit()
{/*
    // we'll use wxPostEvent to post this event since this is the
    // only thread-safe way to post events !
    wxCommandEvent updatevent(wxEVT_COMMAND_CACHESIZE_COMPLETE);

    // the event handler must delete the wxArrayLong which we pass to it in the event !
    updatevent.SetClientData(m_urlSizes);
    if (m_pHandler)
        wxPostEvent(m_pHandler, updatevent);*/
}

