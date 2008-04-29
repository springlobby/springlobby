#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

class HttpDownloader;
#include <wx/intl.h>

class HttpDownloaderThread : public wxThread
{
  public:
    HttpDownloaderThread( HttpDownloader& CallingClass, const wxString& FileUrl, const wxString& DestPath,
        const bool notify = true, const wxString& noticeErr = wxEmptyString, const wxString& noticeOk = wxEmptyString);
    ~HttpDownloaderThread();
    void Init();
    void* Entry();
    void CloseThread();
    bool TestDestroy();
  private:
    HttpDownloader& m_calling_class;
    bool m_destroy;

    wxString m_destpath;
    wxString m_fileurl;

    bool m_notifyOnDownloadEvent;

    wxString m_noticeErr;
    wxString m_noticeOk;



};

class HttpDownloader
{
  public:
    HttpDownloader( const wxString& FileUrl, const wxString& DestPath, const bool notify = true,
        const wxString& noticeErr = wxEmptyString, const wxString& noticeOk  = wxEmptyString);
    ~HttpDownloader();
    void OnComplete(wxCommandEvent& event);

  protected:
    HttpDownloaderThread m_thread_updater;

};



#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
