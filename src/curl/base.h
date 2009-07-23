/*
 *	base.h
 *	wxCURL
 *
 *	Created by Casey O'Donnell on Tue Jun 29 2004.
 *	Copyright (c) 2004 Casey O'Donnell. All rights reserved.
 *
 *  Contributions and Assistance:
 *  Ryan Wilcox - Verbose Options
 *
 *	Licence: wxWidgets Licence
 */

#ifndef _WXCURLBASE_H__INCLUDED_
#define _WXCURLBASE_H__INCLUDED_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "curl/base.h"
#endif

#ifdef WXMAKINGDLL_CURL
    #define WXDLLIMPEXP_CURL WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_CURL WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_CURL
#endif

#include <wx/event.h>
#include <wx/string.h>
#include <wx/datetime.h>
#include <wx/stream.h>
#include <wx/mstream.h>

#include <curl/curl.h>

// base.h: wxWidgets Declarations for Event Types.
//
//////////////////////////////////////////////////////////////////////

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxCURL_PROGRESS_EVENT, 6579)
	DECLARE_EVENT_TYPE(wxCURL_BEGIN_PERFORM_EVENT, 6580)
	DECLARE_EVENT_TYPE(wxCURL_END_PERFORM_EVENT, 6581)
END_DECLARE_EVENT_TYPES()

// base.h: interface for the wxCurlProgressEvent class.
//
//////////////////////////////////////////////////////////////////////

#define EVT_CURL_PROGRESS(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxCURL_PROGRESS_EVENT, wxID_ANY, wxID_ANY, \
		(wxObjectEventFunction) (wxEventFunction) &fn, \
		(wxObject *) NULL \
	),

class WXDLLIMPEXP_CURL wxCurlProgressEvent : public wxEvent
{
public:
	wxCurlProgressEvent();
	wxCurlProgressEvent(const double& rDownloadTotal, const double& rDownloadNow, const double& rUploadTotal, const double& rUploadNow, const wxString& szURL = wxEmptyString);
	wxCurlProgressEvent(const wxCurlProgressEvent& event);

	virtual wxEvent* Clone() const { return new wxCurlProgressEvent(*this); }

	double DownloadNow() const { return m_rDownloadNow; }
	double DownloadTotal() const { return m_rDownloadTotal; }
	double UploadNow() const { return m_rUploadNow; }
	double UploadTotal() const { return m_rUploadTotal; }

	wxString GetURL() { return m_szURL; }

	double DownloadPercent() const { return (100.0 * (m_rDownloadNow/m_rDownloadTotal)); }
	double UploadPercent() const { return (100.0 * (m_rUploadNow/m_rUploadTotal)); }

protected:
	wxString m_szURL;
	double m_rDownloadTotal, m_rDownloadNow;
	double m_rUploadTotal, m_rUploadNow;

private:
	DECLARE_DYNAMIC_CLASS(wxCurlProgressEvent);
};

// base.h: interface for the wxCurlBeginPerformEvent class.
//
//////////////////////////////////////////////////////////////////////

#define EVT_CURL_BEGIN_PERFORM(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxCURL_BEGIN_PERFORM_EVENT, wxID_ANY, wxID_ANY, \
		(wxObjectEventFunction) (wxEventFunction) &fn, \
		(wxObject *) NULL\
	),

class WXDLLIMPEXP_CURL wxCurlBeginPerformEvent : public wxEvent
{
public:
	wxCurlBeginPerformEvent();
	wxCurlBeginPerformEvent(const wxString& szURL);
	wxCurlBeginPerformEvent(const wxCurlBeginPerformEvent& event);

	virtual wxEvent* Clone() const { return new wxCurlBeginPerformEvent(*this); }

	wxString GetURL() { return m_szURL; }

protected:
	wxString m_szURL;

private:
	DECLARE_DYNAMIC_CLASS(wxCurlBeginPerformEvent);
};

// base.h: interface for the wxCurlEndPerformEvent class.
//
//////////////////////////////////////////////////////////////////////

#define EVT_CURL_END_PERFORM(fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxCURL_END_PERFORM_EVENT, wxID_ANY, wxID_ANY, \
		(wxObjectEventFunction) (wxEventFunction) &fn, \
		(wxObject *) NULL\
	),

class WXDLLIMPEXP_CURL wxCurlEndPerformEvent : public wxEvent
{
public:
	wxCurlEndPerformEvent();
	wxCurlEndPerformEvent(const wxString& szURL, const long& iResponseCode);
	wxCurlEndPerformEvent(const wxCurlEndPerformEvent& event);

	virtual wxEvent* Clone() const { return new wxCurlEndPerformEvent(*this); }

	wxString GetURL() { return m_szURL; }

protected:
	wxString	m_szURL;
	long		m_iResponseCode;

private:
	DECLARE_DYNAMIC_CLASS(wxCurlEndPerformEvent);
};

// C Function Declarations for LibCURL
//
//////////////////////////////////////////////////////////////////////

extern "C"
{
	int wxcurl_evt_progress_func(void* ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow);
    int wxcurl_verbose_stream_write (CURL * crlptr , curl_infotype info, char * cStrMessage, size_t msgSize, void * buffer);
	size_t wxcurl_header_func(void *ptr, size_t size, size_t nmemb, void *stream);
	size_t wxcurl_str_write(void* ptr, size_t size, size_t nmemb, void* stream);
	size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream);
	size_t wxcurl_str_read(void* ptr, size_t size, size_t nmemb, void* stream);
	size_t wxcurl_stream_read(void* ptr, size_t size, size_t nmemb, void* stream);
}

// base.h: interface for the wxCurlBase class.
//
//////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_CURL wxCurlBase  
{
public:
	wxCurlBase(const wxString& szURL = wxEmptyString, const wxString& szUserName = wxEmptyString, const wxString& szPassword = wxEmptyString, wxEvtHandler* pEvtHandler = NULL, const bool& bSendUpdateEvents = false, const bool& bSendBeginEndEvents = false);
	virtual ~wxCurlBase();

	// LibCURL Abstraction Methods - Wrapping curl_easy calls...
	bool SetOpt(CURLoption option, ...);
	bool GetInfo(CURLINFO info, ...);
	bool Perform();

	bool InitHandle();
	bool ReInitHandle();
	bool CleanupHandle();
	bool ResetHandle();

	// Member Data Access Methods (MDA)

	// MDA - Get/Set Parent Window.  This is necessary to use progress events.
	bool			SetEvtHandler(wxEvtHandler* pParent);
	wxEvtHandler*	GetEvtHandler() const;

	// MDA - Get/Set Boolean for Sending Update Events.  This is necessary to use progress events.
	void		SendUpdateEvents(const bool& bSendEvts);
	bool		SendUpdateEvents() const;

	// MDA - Get/Set Base URL.  This allows you to specify a 'base' URL if you
	// are performing multiple actions.
	void		SetBaseURL(const wxString& szBaseURL);
	wxString	GetBaseURL() const;

	// MDA - Get/Set Host Port.  This allows you to specify a specific (non-
	// default port) if you like.  The value -1 means that the default port
	// will be used.
	void		SetPort(const long& iPort);
	long		GetPort() const;

	// MDA - Get/Set Username/Password Methods.  If no username or password is
	// needed, simply assign an empty string ("") for each.
	void		SetUsername(const wxString& szUsername);
	wxString	GetUsername() const;

	void		SetPassword(const wxString& szPassword);
	wxString	GetPassword() const;

	// MDA - Response/Error Access
	wxString	GetResponseHeader() const;
	wxString	GetResponseBody() const;		// May only contain data on NON-GET calls.
	long		GetResponseCode() const;
	wxString	GetErrorString() const;

	// MDA - Get/Set Proxy Information.
	void		UseProxy(const bool& bUseProxy);
	bool		UseProxy() const;

	void		SetProxyHost(const wxString& szProxyHost);
	wxString	GetProxyHost() const;

	void		SetProxyUsername(const wxString& szProxyUsername);
	wxString	GetProxyUsername() const;

	void		SetProxyPassword(const wxString& szProxyPassword);
	wxString	GetProxyPassword() const;

	void		SetProxyPort(const long& iProxyPort);
	long		GetProxyPort() const;

	// MDA - Get/Set Verbose Option and Data Stream
	void		Verbose(const bool& bVerbose);
	bool		IsVerbose() const;

    bool		GetVerboseStream(wxOutputStream& destStream);
	bool		GetVerboseString(wxString& szStream);
    
	// Static LibCURL Initialization Methods - Call At Program Init and Close...
	static void Init();
	static void Shutdown();

	// Static LibCURL Utility Methods
	static wxDateTime GetDateFromString(const wxString& szDate);
	static wxString GetURLEncodedString(const wxString& szData);
	static wxString GetStringFromURLEncoded(const wxString& szData);

protected:
	wxString				m_szBaseURL;
	wxString				m_szCurrFullURL;
	wxString				m_szUsername;
	wxString				m_szPassword;
	wxString				m_szUserPass;

	long					m_iHostPort;

	wxString				m_szResponseHeader;
	wxString				m_szResponseBody;
	long					m_iResponseCode;

	bool					m_bUseProxy;
	wxString				m_szProxyHost;
	wxString				m_szProxyUsername;
	wxString				m_szProxyPassword;
	wxString				m_szProxyUserPass;
	long					m_iProxyPort;

	bool					m_bVerbose;
	wxMemoryOutputStream	m_mosVerbose;

	char					m_szErrorBuffer[CURL_ERROR_SIZE];

	wxArrayString			m_arrHeaders;

	CURL*					m_pCURL;

	struct curl_slist*		m_pHeaders;

	wxEvtHandler*			m_pEvtHandler;

	bool					m_bSendUpdateEvts;
	bool					m_bSendBeginEndEvts;

	// CURL Handle Initialization Helper Method
	virtual void	SetCurlHandleToDefaults();
	virtual void	SetHeaders();
	virtual void	ResetHeaders();
	virtual void	ResetResponseVars();

private:

};

#endif // _WXCURLBASE_H__INCLUDED_
