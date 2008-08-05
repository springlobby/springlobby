#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

class HttpDownloader;
#include <wx/string.h>


class HttpDownloader
{
public:
    HttpDownloader( const wxString& FileUrl, const wxString& DestPath, const bool notify = true,
                    const wxString& noticeErr = wxEmptyString, const wxString& noticeOk  = wxEmptyString);
    ~HttpDownloader();
    void OnComplete(wxCommandEvent& event);

protected:

    class HttpDownloaderThread : public wxThread
        {
        public:
            HttpDownloaderThread(  const wxString& FileUrl, const wxString& DestPath,
                                  const bool notify = true, const wxString& noticeErr = wxEmptyString, const wxString& noticeOk = wxEmptyString);
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
            bool m_notifyOnDownloadEvent;

            wxString m_noticeErr;
            wxString m_noticeOk;

        };

    HttpDownloaderThread m_thread_updater;

};



#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
