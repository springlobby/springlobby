#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

#include <wx/event.h>

class wxProgressDialog;
class UpdateProgressbar;
class wxCommandEvent;

class HttpDownloader : public wxEvtHandler
{
  public:
    HttpDownloader( const wxString& FileUrl, const wxString& DestPath );
    ~HttpDownloader();
    void OnComplete(wxCommandEvent& event);

  protected:
    UpdateProgressbar* m_thread_updater;

    DECLARE_EVENT_TABLE()
};

class UpdateProgressbar : public wxThread
{
  public:
    UpdateProgressbar( HttpDownloader& CallingClass, const wxString& FileUrl, const wxString& DestPath );
    ~UpdateProgressbar();
    void Init();
    void* Entry();
    void CloseThread();
    bool TestDestroy();
  private:
    HttpDownloader& m_calling_class;
    bool m_destroy;

    wxString m_destpath;
    wxString m_fileurl;
};

#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
