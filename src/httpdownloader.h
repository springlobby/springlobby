#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

class wxProgressDialog;
class UpdateProgressbar;

class HttpDownloader
{
  public:
    HttpDownloader( const wxString& FileUrl, const wxString& DestPath, bool deflatezipstream = true );
    ~HttpDownloader();

    void OnThreadUpdate();

  private:
    void DeflateFiles();

    wxProgressDialog* m_dialog;
    UpdateProgressbar* m_thread_updater;
    size_t m_file_size;
    wxString m_stringbuffer;
    wxString m_destpath;
    wxInputStream* m_httpstream;
    unsigned int m_progress;
};

class UpdateProgressbar : public wxThread
{
  public:
    UpdateProgressbar( HttpDownloader& CallingClass );
    void Init();
    void* Entry();
    void CloseThread();
    bool TestDestroy();
  private:
    HttpDownloader& m_calling_class;
    bool m_destroy;
};

#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
