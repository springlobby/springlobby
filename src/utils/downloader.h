#ifndef DOWNLOADER_HH_INCLUDED
#define DOWNLOADER_HH_INCLUDED

#include <wx/string.h>
#include <wx/arrstr.h>

struct PlasmaResourceInfo {
    wxString m_name;//file name
    wxString m_torrent_filename;//fn on remote
    wxArrayString m_webseeds;//full urls
    wxArrayString m_dependencies;//list of resource names

    enum ResourceType {
        mod = 0,
        map = 1,
        unknwon = 2
    };

    ResourceType m_type;

};

class PlasmaInterface {
    public:
        PlasmaInterface();

        PlasmaResourceInfo GetResourceInfo( const wxString& name );
        bool DownloadTorrentFile( const PlasmaResourceInfo& info, const wxString& destination_directory );

    protected:
        void downloadFile( const wxString& host, const wxString& remote_path, const wxString& local_dest );

        wxString m_host;
};

#endif
