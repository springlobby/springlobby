/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

#include <wx/event.h>
class wxThread;
class wxEvtHandler;

class HttpDownloaderThread : public wxThread
{
	public:
		static const wxEventType httpDownloadEvtComplete;

		HttpDownloaderThread(const wxString& FileUrl, const wxString& DestPath, const bool unzip, wxObjectEventFunction func, wxEvtHandler* evt);
		~HttpDownloaderThread();
		void Init();
		void* Entry();
		void CloseThread();
		bool TestDestroy();
	private:
		bool Unzip();
		bool m_do_unzip;

		bool m_destroy;
		wxString m_destpath;
		wxString m_fileurl;

		wxEvtHandler* m_evt;
};

#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

