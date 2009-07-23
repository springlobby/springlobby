/*
 *	base.cpp
 *	wxCURL
 *
 *	Created by Casey O'Donnell on Tue Jun 29 2004.
 *	Copyright (c) 2004 Casey O'Donnell. All rights reserved.
 *
 *
 *	Licence: wxWidgets Licence
 */

//////////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <stdio.h>
#include <stdarg.h>

#include <wx/curl/base.h>

//////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// C Functions for LibCURL
//////////////////////////////////////////////////////////////////////

extern "C"
{
	int wxcurl_evt_progress_func(void* ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow)
	{
		if(ptr)
		{
			wxCurlProgressEvent evt(rDlTotal, rDlNow, rUlTotal, rUlNow);

			wxPostEvent((wxEvtHandler*)ptr, evt);
		}

		return 0;
	}

    int wxcurl_verbose_stream_write(CURL * crlptr , curl_infotype info, char * cStrMessage, size_t msgSize, void * buffer)
    {
		wxString szMessage(cStrMessage, msgSize);
		wxString szVerboseMessage;

		wxOutputStream* pBuf = (wxOutputStream*)buffer;

		switch (info)
		{
			case CURLINFO_TEXT:
				szVerboseMessage = wxString(_T("Text: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_HEADER_IN:
				szVerboseMessage = wxString(_T("Header in: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_HEADER_OUT:
				szVerboseMessage = wxString(_T("Header out: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_DATA_IN:
				szVerboseMessage = wxString(_T("Data in: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_DATA_OUT:
				szVerboseMessage = wxString(_T("Data out: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_END:
				szVerboseMessage = wxString(_T("End: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_SSL_DATA_IN:
				szVerboseMessage = wxString(_T("SSL Data in: ")) + szMessage + _T("\n");
				break;
			case CURLINFO_SSL_DATA_OUT:
				szVerboseMessage = wxString(_T("SSL Data out: ")) + szMessage + _T("\n");
				break;
        }
        
        pBuf->Write(szVerboseMessage, szVerboseMessage.Len() * sizeof(wxChar));

        return 0;
    }
    
	size_t wxcurl_header_func(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		size_t iRealSize = size * nmemb;

		wxString* pStr = (wxString*) stream;

		if(pStr)
		{
			(*pStr) += ((char*)ptr);
		}

		return iRealSize;
	}

	size_t wxcurl_str_write(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t iRealSize = size * nmemb;

		wxString* pStr = (wxString*) stream;

		if(pStr)
		{
			*pStr += (char*)ptr;
		}

		return iRealSize;
	}

	size_t wxcurl_stream_write(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t iRealSize = size * nmemb;

		wxOutputStream* pBuf = (wxOutputStream*)stream;

		if(pBuf)
		{
			pBuf->Write(ptr, iRealSize);

			return pBuf->LastWrite();
		}

		return 0;
	}

	size_t wxcurl_str_read(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t iRealSize = size * nmemb;
		size_t iRetVal = 0;

		wxString* pStr = (wxString*) stream;

		if(pStr)
		{
			if(pStr->Len() >= iRealSize)
			{
				strncpy((char*)ptr, ((const char*)(*pStr)), iRealSize);
				iRetVal = iRealSize;
			}
			else
			{
				strncpy((char*)ptr, ((const char*)(*pStr)), pStr->Len());
				iRetVal = pStr->Len();
			}

			*pStr = pStr->Right(pStr->Len() - iRetVal);
		}

		return iRetVal;
	}

	size_t wxcurl_stream_read(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		size_t iRealSize = size * nmemb;

		wxInputStream* pBuf = (wxInputStream*)stream;

		if(pBuf)
		{
			pBuf->Read(ptr, iRealSize);

			return pBuf->LastRead();
		}

		return 0;
	}
}

// base.cpp: implementation of the wxCurlProgressEvent class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DEFINE_EVENT_TYPE(wxCURL_PROGRESS_EVENT);

IMPLEMENT_DYNAMIC_CLASS(wxCurlProgressEvent, wxEvent);

wxCurlProgressEvent::wxCurlProgressEvent()
: wxEvent(-1, wxCURL_PROGRESS_EVENT),
  m_rDownloadNow(0.0), m_rDownloadTotal(0.0),
  m_rUploadNow(0.0), m_rUploadTotal(0.0)
{
}

wxCurlProgressEvent::wxCurlProgressEvent(const double& rDownloadTotal, const double& rDownloadNow, const double& rUploadTotal, const double& rUploadNow, const wxString& szURL /*= wxEmptyString*/)
: wxEvent(-1, wxCURL_PROGRESS_EVENT),
  m_szURL(szURL),
  m_rDownloadTotal(rDownloadTotal), m_rDownloadNow(rDownloadNow),
  m_rUploadTotal(rUploadTotal), m_rUploadNow(rUploadNow)
{
}

wxCurlProgressEvent::wxCurlProgressEvent(const wxCurlProgressEvent& event)
: wxEvent(event)
{
	m_szURL = event.m_szURL;

	m_rDownloadNow = event.m_rDownloadNow;
	m_rDownloadTotal = event.m_rDownloadTotal;

	m_rUploadNow = event.m_rUploadNow;
	m_rUploadTotal = event.m_rUploadTotal;
}

// wxCurlBase.cpp: implementation of the wxCurlBeginPerformEvent class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DEFINE_EVENT_TYPE(wxCURL_BEGIN_PERFORM_EVENT);

IMPLEMENT_DYNAMIC_CLASS(wxCurlBeginPerformEvent, wxEvent);

wxCurlBeginPerformEvent::wxCurlBeginPerformEvent()
: wxEvent(-1, wxCURL_BEGIN_PERFORM_EVENT)
{
}

wxCurlBeginPerformEvent::wxCurlBeginPerformEvent(const wxString& szURL)
: wxEvent(-1, wxCURL_BEGIN_PERFORM_EVENT),
  m_szURL(szURL)
{
}

wxCurlBeginPerformEvent::wxCurlBeginPerformEvent(const wxCurlBeginPerformEvent& event)
: wxEvent(event)
{
	m_szURL = event.m_szURL;
}

// wxCurlBase.cpp: implementation of the wxCurlEndPerformEvent class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DEFINE_EVENT_TYPE(wxCURL_END_PERFORM_EVENT);

IMPLEMENT_DYNAMIC_CLASS(wxCurlEndPerformEvent, wxEvent);

wxCurlEndPerformEvent::wxCurlEndPerformEvent()
: wxEvent(-1, wxCURL_END_PERFORM_EVENT),
  m_iResponseCode(0)
{
}

wxCurlEndPerformEvent::wxCurlEndPerformEvent(const wxString& szURL, const long& iResponseCode)
: wxEvent(-1, wxCURL_END_PERFORM_EVENT),
  m_szURL(szURL),
  m_iResponseCode(iResponseCode)
{
}

wxCurlEndPerformEvent::wxCurlEndPerformEvent(const wxCurlEndPerformEvent& event)
: wxEvent(event)
{
	m_szURL = event.m_szURL;
	m_iResponseCode = event.m_iResponseCode;
}

// wxCurlBase.cpp: implementation of the wxCurlBase class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxCurlBase::wxCurlBase(const wxString& szURL /*= wxEmptyString*/, const wxString& szUserName /*= wxEmptyString*/, const wxString& szPassword /*= wxEmptyString*/, wxEvtHandler* pEvtHandler /*= NULL*/, const bool& bSendUpdateEvents /*= false*/, const bool& bSendBeginEndEvents /*= false*/)
: m_szBaseURL(szURL), m_szCurrFullURL(szURL), m_szUsername(szUserName), m_szPassword(szPassword),
  m_iHostPort(-1), m_iResponseCode(-1),
  m_bUseProxy(false), m_iProxyPort(-1),
  m_pCURL(NULL), m_pHeaders(NULL), m_pEvtHandler(pEvtHandler),
  m_bSendUpdateEvts(bSendUpdateEvents), m_bSendBeginEndEvts(bSendBeginEndEvents),
  m_bVerbose(false)
{
	m_szErrorBuffer[0] = '\0';

	InitHandle();
}

wxCurlBase::~wxCurlBase()
{
	CleanupHandle();
	ResetHeaders();
}

//////////////////////////////////////////////////////////////////////
// LibCURL Abstraction Methods
//////////////////////////////////////////////////////////////////////

typedef int (*func_T)(void);
bool wxCurlBase::SetOpt(CURLoption option, ...)
{
	va_list arg;

	func_T param_func = (func_T)0;
	long param_long = 0;
	void *param_obj = NULL;
	curl_off_t param_offset = 0;

	va_start(arg, option);

	CURLcode res = CURLE_OK;

	// This code stolen from easy.c from LibCURL - It is needed to ensure that
	// types are maintained.
	if(option < CURLOPTTYPE_OBJECTPOINT) {
		/* This is a LONG type */
		param_long = va_arg(arg, long);
		res = curl_easy_setopt(m_pCURL, option, param_long);
	}
	else if(option < CURLOPTTYPE_FUNCTIONPOINT) {
		/* This is a object pointer type */
		param_obj = va_arg(arg, void *);
		res = curl_easy_setopt(m_pCURL, option, param_obj);
	}
	else if(option < CURLOPTTYPE_OFF_T) {
		/* This is a function pointer type */
		param_func = va_arg(arg, func_T );
		res = curl_easy_setopt(m_pCURL, option, param_func);
	} else {
		/* This is a curl_off_t type */
		param_offset = va_arg(arg, curl_off_t);
		res = curl_easy_setopt(m_pCURL, option, param_offset);
	}

	va_end(arg);

	return (res == CURLE_OK);
}

bool wxCurlBase::GetInfo(CURLINFO info, ...)
{
	va_list arg;
	void* pParam;

	va_start(arg, info);
	pParam = va_arg(arg, void*);

	CURLcode res = CURLE_OK;

	res = curl_easy_getinfo(m_pCURL, info, pParam);

	return (res == CURLE_OK);
}

bool wxCurlBase::Perform()
{
	CURLcode res = CURLE_OK;

	if(m_bSendBeginEndEvts && m_pEvtHandler)
	{
		wxCurlBeginPerformEvent bgnEvent(m_szCurrFullURL);

		wxPostEvent(m_pEvtHandler, bgnEvent);
	}

	res = curl_easy_perform(m_pCURL);

	if(m_bSendBeginEndEvts && m_pEvtHandler)
	{
		wxCurlEndPerformEvent endEvent(m_szCurrFullURL, m_iResponseCode);

		wxPostEvent(m_pEvtHandler, endEvent);
	}

	return (res == CURLE_OK);
}

bool wxCurlBase::InitHandle()
{
	if(m_pCURL)
		return false;

	m_pCURL = curl_easy_init();

	return (m_pCURL != NULL);
}

bool wxCurlBase::ReInitHandle()
{
	CleanupHandle();
	return InitHandle();
}

bool wxCurlBase::CleanupHandle()
{
	if(m_pCURL)
	{
		curl_easy_cleanup(m_pCURL);
		m_pCURL = NULL;
	}

	return true;
}

bool wxCurlBase::ResetHandle()
{
	curl_easy_reset(m_pCURL);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Member Data Access Methods
//////////////////////////////////////////////////////////////////////

bool wxCurlBase::SetEvtHandler(wxEvtHandler* pEvtHandler)
{
	m_pEvtHandler = pEvtHandler;

	return true;
}

wxEvtHandler* wxCurlBase::GetEvtHandler() const
{
	return m_pEvtHandler;
}

void wxCurlBase::SendUpdateEvents(const bool& bSendEvts)
{
	m_bSendUpdateEvts = bSendEvts;
}

bool wxCurlBase::SendUpdateEvents() const
{
	return m_bSendUpdateEvts;
}

void wxCurlBase::SetBaseURL(const wxString& szBaseURL)
{
	m_szBaseURL = szBaseURL;
}

wxString wxCurlBase::GetBaseURL() const
{
	return m_szBaseURL;
}

void wxCurlBase::SetPort(const long& iPort)
{
	m_iHostPort = iPort;
}

long wxCurlBase::GetPort() const
{
	return m_iHostPort;
}

void wxCurlBase::SetUsername(const wxString& szUsername)
{
	m_szUsername = szUsername;
}

wxString wxCurlBase::GetUsername() const
{
	return m_szUsername;
}

void wxCurlBase::SetPassword(const wxString& szPassword)
{
	m_szPassword = szPassword;
}

wxString wxCurlBase::GetPassword() const
{
	return m_szPassword;
}

wxString wxCurlBase::GetResponseHeader() const
{
	return m_szResponseHeader;
}

wxString wxCurlBase::GetResponseBody() const
{
	return m_szResponseBody;
}

long wxCurlBase::GetResponseCode() const
{
	return m_iResponseCode;
}

wxString wxCurlBase::GetErrorString() const
{
	return wxString(m_szErrorBuffer);
}

void wxCurlBase::UseProxy(const bool& bUseProxy)
{
	m_bUseProxy = bUseProxy;
}

bool wxCurlBase::UseProxy() const
{
	return m_bUseProxy;
}

void wxCurlBase::SetProxyHost(const wxString& szProxyHost)
{
	m_szProxyHost = szProxyHost;
}

wxString wxCurlBase::GetProxyHost() const
{
	return m_szProxyHost;
}

void wxCurlBase::SetProxyUsername(const wxString& szProxyUsername)
{
	m_szProxyUsername = szProxyUsername;
}

wxString wxCurlBase::GetProxyUsername() const
{
	return m_szProxyUsername;
}

void wxCurlBase::SetProxyPassword(const wxString& szProxyPassword)
{
	m_szProxyPassword = szProxyPassword;
}

wxString wxCurlBase::GetProxyPassword() const
{
	return m_szProxyPassword;
}

void wxCurlBase::SetProxyPort(const long& iProxyPort)
{
	m_iProxyPort = iProxyPort;
}

long wxCurlBase::GetProxyPort() const
{
	return m_iProxyPort;
}

void wxCurlBase::Verbose(const bool& bVerbose)
{
	m_bVerbose = bVerbose;
}

bool wxCurlBase::IsVerbose() const
{
	return m_bVerbose;
}

bool wxCurlBase::GetVerboseStream(wxOutputStream& destStream)
{
	if(m_bVerbose)
	{
		if(m_mosVerbose.IsOk())
		{
			char* pBuffer = new char[m_mosVerbose.GetSize()];

			if(pBuffer)
			{
				m_mosVerbose.CopyTo(pBuffer, m_mosVerbose.GetSize());

				destStream.Write(pBuffer, m_mosVerbose.GetSize());

				return destStream.IsOk();
			}
		}
	}

	return false;
}

bool wxCurlBase::GetVerboseString(wxString& szStream)
{
	if(m_bVerbose)
	{
		char* pBuff;
		wxString szBuff;

		pBuff = new char[m_mosVerbose.GetSize()];

		if(pBuff)
		{
			m_mosVerbose.CopyTo(pBuff, m_mosVerbose.GetSize());
			szStream.Append(pBuff);

			delete[] pBuff;

			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// Helper Methods
//////////////////////////////////////////////////////////////////////

void wxCurlBase::SetCurlHandleToDefaults()
{
	if(m_pCURL && ResetHandle())
	{
		ResetResponseVars();

		SetOpt(CURLOPT_HEADERFUNCTION, wxcurl_header_func);
		SetOpt(CURLOPT_WRITEHEADER, &m_szResponseHeader);
		SetOpt(CURLOPT_ERRORBUFFER, m_szErrorBuffer);

		if(m_pEvtHandler && m_bSendUpdateEvts)
		{
			SetOpt(CURLOPT_NOPROGRESS, FALSE);
			SetOpt(CURLOPT_PROGRESSFUNCTION, wxcurl_evt_progress_func);
			SetOpt(CURLOPT_PROGRESSDATA, m_pEvtHandler);
		}

		if(!m_szUsername.IsEmpty() || !m_szPassword.IsEmpty())
		{
			m_szUserPass = m_szUsername + ":" + m_szPassword;

			SetOpt(CURLOPT_USERPWD, m_szUserPass.c_str());
			SetOpt(CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		}

		if(m_iHostPort != -1)
		{
			SetOpt(CURLOPT_PORT, m_iHostPort);
		}

		if(m_bUseProxy && !m_szProxyHost.IsEmpty())
		{
			SetOpt(CURLOPT_PROXY, m_szProxyHost.c_str());
		}

		if(m_bUseProxy && (m_iProxyPort != -1))
		{
			SetOpt(CURLOPT_PROXYPORT, m_iProxyPort);
		}

		if(m_bUseProxy && (!m_szProxyUsername.IsEmpty() || !m_szProxyPassword.IsEmpty()))
		{
			m_szProxyUserPass = m_szProxyUsername + ":" + m_szProxyPassword;

			SetOpt(CURLOPT_PROXYUSERPWD, m_szProxyUserPass.c_str());
		}

		if(m_bVerbose)
		{
			SetOpt(CURLOPT_VERBOSE, TRUE);
			SetOpt(CURLOPT_DEBUGFUNCTION, wxcurl_verbose_stream_write);
			SetOpt(CURLOPT_DEBUGDATA, (void*) &m_mosVerbose);
		}
	}
}

void wxCurlBase::SetHeaders()
{
	if(!m_arrHeaders.IsEmpty())
	{
		if(m_pHeaders)
		{
			curl_slist_free_all(m_pHeaders);

			m_pHeaders = NULL;

			SetOpt(CURLOPT_HTTPHEADER, NULL);
		}

		for(unsigned int i = 0; i < m_arrHeaders.Count(); i++)
		{
			m_pHeaders = curl_slist_append(m_pHeaders, m_arrHeaders[i]);
		}

		SetOpt(CURLOPT_HTTPHEADER, m_pHeaders);
	}
}

void wxCurlBase::ResetHeaders()
{
	m_arrHeaders.Clear();

	if(m_pHeaders)
	{
		curl_slist_free_all(m_pHeaders);

		m_pHeaders = NULL;

		SetOpt(CURLOPT_HTTPHEADER, NULL);
	}
}

void wxCurlBase::ResetResponseVars()
{
	m_szResponseHeader = wxEmptyString;
	m_szResponseBody = wxEmptyString;
	m_iResponseCode = -1;
	m_szCurrFullURL = wxEmptyString;
}

//////////////////////////////////////////////////////////////////////
// Static Initialization and Shutdown Methods
//////////////////////////////////////////////////////////////////////

void wxCurlBase::Init()
{
	// Initialize LibCURL
	curl_global_init(CURL_GLOBAL_ALL);
}

void wxCurlBase::Shutdown()
{
	// Shutdown CurlLib
	curl_global_cleanup();
}

//////////////////////////////////////////////////////////////////////
// Static Utility Methods
//////////////////////////////////////////////////////////////////////

wxDateTime wxCurlBase::GetDateFromString(const wxString& szDate)
{
	time_t now = wxDateTime::Now().GetTicks();

	return wxDateTime(curl_getdate(szDate, &now));
}

wxString wxCurlBase::GetURLEncodedString(const wxString& szData)
{
	char* pszRetVal = curl_escape(szData, szData.Len());

	if(pszRetVal)
	{
		wxString szRetVal = pszRetVal;

		curl_free(pszRetVal);

		return szRetVal;
	}

	return wxEmptyString;
}

wxString wxCurlBase::GetStringFromURLEncoded(const wxString& szData)
{
	char* pszRetVal = curl_unescape(szData, szData.Len());

	if(pszRetVal)
	{
		wxString szRetVal = pszRetVal;

		curl_free(pszRetVal);

		return szRetVal;
	}

	return wxEmptyString;
}
