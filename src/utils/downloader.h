#ifndef DOWNLOADER_HH_INCLUDED
#define DOWNLOADER_HH_INCLUDED

#include <wx/string.h>
#include <wx/arrstr.h>
#include <vector>
#include <map>
#include "inetclass.h"

struct PlasmaResourceInfo {
    //!file name
    wxString m_name;
    //!fn on remote
    wxString m_torrent_filename;
    //!full urls
    wxArrayString m_webseeds;
    //!list of resource names
    wxArrayString m_dependencies;
    //! will only be non-empty when actual download of torrent file succeeded
    wxString m_local_torrent_filepath;

    enum ResourceType {
        mod = 0,
        map = 1,
        unknwon = 2
    };

    ResourceType m_type;

};

template <class PB, class I >
class GlobalObjectHolder;

class PlasmaInterface : public iNetClass {
    public:

        PlasmaResourceInfo GetResourceInfo( const wxString& name ) ;

        //! post: added full path to dl'ed torrent fiel to info
        bool DownloadTorrentFile( PlasmaResourceInfo& info, const wxString& destination_directory ) const;

        typedef std::vector<PlasmaResourceInfo>
            ResourceList;

        const ResourceList& GetResourceList() {  return m_resource_list; }
        void InitResourceList();

    protected:
        PlasmaInterface();

        template <class PB, class I >
        friend class GlobalObjectHolder;

        //!TODO doesn't really need to be here
        void downloadFile( const wxString& host, const wxString& remote_path, const wxString& local_dest ) const;
        void ParseResourceListData( const int buffer_index );

        void OnConnected( Socket* ){}
        void OnDisconnected( Socket* ){}
        virtual void OnDataReceived( Socket* socket );

        //! fqdn of the host where plasma service runs
        wxString m_host;
        //! remote path to service aspx, starting at web root
        wxString m_remote_path;

        //! a vector holding (after the initial request) all the available resources on the remote
        ResourceList m_resource_list;

        std::map<Socket*, int> m_socket_index;
        std::vector<wxString> m_buffers; // why not using map here directly? because i can write directly into the vector this way

        static const int m_list_socket_index = 0;
        static const int m_info_socket_index = 1;
};

PlasmaInterface& plasmaInterface();

#endif
