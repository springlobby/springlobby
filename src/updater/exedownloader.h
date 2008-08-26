#ifndef SPRINGLOBBY_HEADERGUARD_EXEDOWNLOADER
#define SPRINGLOBBY_HEADERGUARD_EXEPDOWNLOADER

class ExeDownloader;
#include <wx/string.h>


class ExeDownloader
{
public:
    ExeDownloader( const wxString& FileUrl, const wxString& DestPath );
    ~ExeDownloader();
    void OnComplete(wxCommandEvent& event);

protected:

    class ExeDownloaderThread : public wxThread
        {
        public:
            ExeDownloaderThread(  const wxString& FileUrl, const wxString& DestPath );
            ~ExeDownloaderThread();
            void Init();
            void* Entry();
            void CloseThread();
            bool TestDestroy();
        private:
            bool m_destroy;

            wxString m_destpath;
            wxString m_fileurl;
            bool Unzip();
        };

    ExeDownloaderThread* m_thread_updater;

};



#endif // SPRINGLOBBY_HEADERGUARD_ExeDOWNLOADER
