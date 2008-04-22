#ifndef SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER

class wxProgressDialog;
class UpdateProgressbar;

class HttpDownloader
{
  public:
    HttpDownloader( const wxString& FileUrl, const wxString& DestPath, bool deflatezipstream = true );
    ~HttpDownloader();
  private:
    UpdateProgressbar* m_thread_updater;
};

class UpdateProgressbar : public wxThread
{
  public:
    UpdateProgressbar( HttpDownloader& CallingClass, const wxString& FileUrl, const wxString& DestPath, bool deflatezipstream = true );
    ~UpdateProgressbar();
    void Init();
    void* Entry();
    void CloseThread();
    bool TestDestroy();
  private:
    HttpDownloader& m_calling_class;
    bool m_destroy;

    void DeflateFiles();

    wxProgressDialog* m_dialog;
    size_t m_file_size;
    std::string m_stringbuffer;
    wxString m_destpath;
    wxInputStream* m_httpstream;
    unsigned int m_progress;
};

#endif // SPRINGLOBBY_HEADERGUARD_HTTPDOWNLOADER
