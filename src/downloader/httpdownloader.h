/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

#include <wx/event.h>
#include <wx/thread.h>

const wxEventType httpDownloadEvtComplete = wxNewEventType();
const wxEventType httpDownloadEvtFailed = wxNewEventType();

template < class ParentClass >
class HttpDownloaderThread : public wxThread
{
	public:
		HttpDownloaderThread(  const wxString& FileUrl, const wxString& DestPath, ParentClass& parent, int code = 0, const bool notify = true, const bool unzip = true, const wxString& noticeErr = wxEmptyString, const wxString& noticeOk = wxEmptyString );
		~HttpDownloaderThread();
		void Init();
		void* Entry();
		void CloseThread();
		bool TestDestroy();
	private:
		bool m_destroy;

		wxString m_destpath;
		wxString m_fileurl;
		bool Unzip();
		bool m_do_unzip;
		bool m_notifyOnDownloadEvent;

		wxString m_noticeErr;
		wxString m_noticeOk;

		int m_id_code;

		ParentClass& m_parent;

};

#include "httpdownloader.cpp"

#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

