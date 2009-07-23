/*
 *	http.cpp
 *	wxCURL
 *
 *	Created by Casey O'Donnell on Tue Jun 29 2004.
 *	Copyright (c) 2004 Casey O'Donnell. All rights reserved.
 *
 *
 *	Licence: wxWidgets Licence
 */

// http.cpp: implementation of the wxCurlHTTP class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include <wx/mstream.h>
#include <wx/wfstream.h>

#include <wx/curl/http.h>

//////////////////////////////////////////////////////////////////////
// C Functions for LibCURL
//////////////////////////////////////////////////////////////////////
extern "C"
{
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxCurlHTTP::wxCurlHTTP(const wxString& szURL /*= wxEmptyString*/, const wxString& szUserName /*= wxEmptyString*/, const wxString& szPassword /*= wxEmptyString*/, wxEvtHandler* pEvtHandler /*= NULL*/, const bool& bSendUpdateEvents /*= false*/, const bool& bSendBeginEndEvents /*= false*/)
: wxCurlBase(szURL, szUserName, szPassword, pEvtHandler, bSendUpdateEvents, bSendBeginEndEvents),
  m_pPostHead(NULL), m_pPostTail(NULL), m_bUseCookies(false), m_szCookieFile("-"),
  m_pszPostFieldsData(NULL), m_iPostDataSize(0)
{
}

wxCurlHTTP::~wxCurlHTTP()
{
	ResetPostData();
}

//////////////////////////////////////////////////////////////////////
// Member Data Access Methods
//////////////////////////////////////////////////////////////////////

void wxCurlHTTP::UseCookies(const bool& bUseCookies)
{
	m_bUseCookies = bUseCookies;
}

bool wxCurlHTTP::UseCookies() const
{
	return m_bUseCookies;
}

void wxCurlHTTP::SetCookieFile(const wxString& szFilePath)
{
	m_szCookieFile = szFilePath;
}

wxString wxCurlHTTP::GetCookieFile() const
{
	return m_szCookieFile;
}

//////////////////////////////////////////////////////////////////////
// Post Data Methods
//////////////////////////////////////////////////////////////////////

bool wxCurlHTTP::AddForm(const bool& bClear, const wxString& szName, struct curl_forms* pForms)
{
	if(bClear)
		ResetPostData();

	CURLFORMcode res = CURL_FORMADD_OK;

	//FormAdd
	res = curl_formadd(&m_pPostHead, &m_pPostTail, CURLFORM_COPYNAME, szName.c_str(), CURLFORM_ARRAY, pForms, CURLFORM_END);

	return (res == CURL_FORMADD_OK);
}

//////////////////////////////////////////////////////////////////////
// Action Methods
//////////////////////////////////////////////////////////////////////

bool wxCurlHTTP::Options(const wxString& szRemoteFile /*= wxEmptyString*/)
{
	if(m_pCURL)
	{
		SetCurlHandleToDefaults();

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_CUSTOMREQUEST, "OPTIONS");
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_str_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&m_szResponseBody);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}

	return false;
}

bool wxCurlHTTP::Head(const wxString& szRemoteFile /*= wxEmptyString*/)
{
	if(m_pCURL)
	{
		SetCurlHandleToDefaults();

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_HTTPGET, TRUE);
		SetOpt(CURLOPT_NOBODY, TRUE);
		
		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}

	return false;
}

bool wxCurlHTTP::Post(const char* buffer, size_t size, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	wxMemoryInputStream inStream(buffer, size);

	return Post(inStream, szRemoteFile);
}

bool wxCurlHTTP::Post(wxInputStream& buffer, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	curl_off_t iSize = 0;

	if(m_pCURL && buffer.IsOk())
	{
		SetCurlHandleToDefaults();

		iSize = buffer.GetSize();

		if(iSize == (~(size_t)0))	// wxCurlHTTP does not know how to upload unknown length streams.
			return false;

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_POST, TRUE);
		SetOpt(CURLOPT_POSTFIELDSIZE_LARGE, iSize);
		SetOpt(CURLOPT_READFUNCTION, wxcurl_stream_read);
		SetOpt(CURLOPT_READDATA, (void*)&buffer);
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_str_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&m_szResponseBody);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}

	return false;
}

bool wxCurlHTTP::Post(const wxString& szRemoteFile /*= wxEmptyString*/)
{
	if(m_pCURL && m_pPostHead && m_pPostTail)
	{
		SetCurlHandleToDefaults();

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_POST, TRUE);
		SetOpt(CURLOPT_HTTPPOST, m_pPostHead);
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_str_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&m_szResponseBody);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}

	return false;
}

bool wxCurlHTTP::Trace(const wxString& szRemoteFile /*= wxEmptyString*/)
{
	if(m_pCURL)
	{
		SetCurlHandleToDefaults();

		m_arrHeaders.Add("Content-type: message/http");

		SetHeaders();

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_CUSTOMREQUEST, "TRACE");
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_str_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&m_szResponseBody);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			ResetHeaders();

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}

		ResetHeaders();
	}

	return false;
}

bool wxCurlHTTP::Get(const wxString& szFilePath, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	wxFFileOutputStream outStream(szFilePath);

	return Get(outStream, szRemoteFile);
}

size_t wxCurlHTTP::Get(char*& buffer, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	wxMemoryOutputStream outStream;

	buffer = NULL;
	size_t iRetVal = 0;

	if(Get(outStream, szRemoteFile))
	{
		iRetVal = outStream.GetSize();
		buffer = (char*)malloc(iRetVal + 1);

		if(buffer)
		{
			outStream.CopyTo(buffer, outStream.GetSize());
			buffer[iRetVal] = '\0';
			return iRetVal;
		}
		
		free(buffer);
		buffer = NULL;
	}

	return 0;
}

bool wxCurlHTTP::Get(wxOutputStream& buffer, const wxString& szRemoteFile /*=wxEmptyString*/)
{
	if(m_pCURL && buffer.IsOk())
	{
		SetCurlHandleToDefaults();

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_HTTPGET, TRUE);
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_stream_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&buffer);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}
	
	return false;
}

bool wxCurlHTTP::Put(const wxString& szFilePath, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	wxFFileInputStream inStream(szFilePath);

	return Put(inStream, szRemoteFile);
}

bool wxCurlHTTP::Put(const char* buffer, size_t size, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	wxMemoryInputStream inStream(buffer, size);

	return Put(inStream, szRemoteFile);
}

bool wxCurlHTTP::Put(wxInputStream& buffer, const wxString& szRemoteFile /*= wxEmptyString*/)
{
	curl_off_t iSize = 0;

	if(m_pCURL && buffer.IsOk())
	{
		SetCurlHandleToDefaults();

		iSize = buffer.GetSize();

		if(iSize == (~(size_t)0))	// wxCurlHTTP does not know how to upload unknown length streams.
			return false;

		m_szCurrFullURL = m_szBaseURL + szRemoteFile;

		SetOpt(CURLOPT_UPLOAD, TRUE);
		SetOpt(CURLOPT_PUT, TRUE);
		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_READFUNCTION, wxcurl_stream_read);
		SetOpt(CURLOPT_READDATA, (void*)&buffer);
		SetOpt(CURLOPT_INFILESIZE_LARGE, (curl_off_t)iSize);
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_str_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&m_szResponseBody);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}

	return false;
}

bool wxCurlHTTP::Delete(const wxString& szRemoteLoc /*= wxEmptyString*/)
{
	if(m_pCURL)
	{
		SetCurlHandleToDefaults();

		m_szCurrFullURL = m_szBaseURL + szRemoteLoc;

		SetOpt(CURLOPT_URL, m_szCurrFullURL.c_str());
		SetOpt(CURLOPT_CUSTOMREQUEST, "DELETE");
		SetOpt(CURLOPT_WRITEFUNCTION, wxcurl_str_write);
		SetOpt(CURLOPT_WRITEDATA, (void*)&m_szResponseBody);

		if(Perform())
		{
			GetInfo(CURLINFO_RESPONSE_CODE, &m_iResponseCode);

			return ((m_iResponseCode > 199) && (m_iResponseCode < 300));
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
// Helper Methods
//////////////////////////////////////////////////////////////////////

void wxCurlHTTP::ResetPostData()
{
	m_pszPostFieldsData = NULL;
	m_iPostDataSize = 0;

	if(m_pPostHead && m_pPostTail)
	{
		curl_formfree(m_pPostHead);

		m_pPostHead = NULL;
		m_pPostTail = NULL;
	}
}

void wxCurlHTTP::SetCurlHandleToDefaults()
{
	wxCurlBase::SetCurlHandleToDefaults();

	if(m_bUseCookies)
	{
		SetOpt(CURLOPT_COOKIEJAR, m_szCookieFile.c_str());
	}
}

