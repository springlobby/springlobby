#ifndef DOWNLOADER_HH_INCLUDED
#define DOWNLOADER_HH_INCLUDED

#ifndef NO_TORRENT_SYSTEM

#include <wx/string.h>
#include <wx/event.h>
#include <wx/event.h>
#include <map>
#include "../inetclass.h"
#include "plasmaresourceinfo.h"
#include "../thread.h"

template <class PB, class I >
class GlobalObjectHolder;

class wxCurlHTTP;
class wxCurlEndPerformEvent;

const wxEventType DoFetchResourcesEvt = wxNewEventType();

class PlasmaInterface : public iNetClass, public wxEvtHandler {
    public:
		~PlasmaInterface();
        PlasmaResourceInfo GetResourceInfo( const wxString& name ) ;

        //! post: added full path to dl'ed torrent fiel to info
        bool DownloadTorrentFile( PlasmaResourceInfo& info, const wxString& destination_directory ) const;

        typedef std::vector<PlasmaResourceInfo>
            ResourceList;

        const ResourceList& GetResourceList() {  return m_resource_list; }
        //! start retrieving the newest resourcelist info
        void FetchResourceList();
        //! fill resourcelist with cached data
        void InitResourceList();
        void ParseResourceListData( const wxString& buffer );


    protected:
        PlasmaInterface();

        template <class PB, class I >
        friend class GlobalObjectHolder;

		friend class FetchResourceListWorkItem;

        //!TODO doesn't really need to be here
        void downloadFile( const wxString& host, const wxString& remote_path, const wxString& local_dest ) const;
        PlasmaResourceInfo ParseResourceInfoData( const int buffer_index );

        void OnConnected( Socket* ){}
        void OnDisconnected( Socket* ){}
		virtual void OnDataReceived( Socket* socket ){}

        //! fqdn of the host where plasma service runs
        wxString m_host;
        //! remote path to service aspx, starting at web root
        wxString m_remote_path;

        //! a vector holding (after the initial request) all the available resources on the remote
        ResourceList m_resource_list;

        std::map<Socket*, int> m_socket_index;
        std::map<int,wxString> m_buffers;

        static const int m_list_socket_index = 0;
        static const int m_info_socket_index = 1;

		WorkerThread m_worker_thread;
};

PlasmaInterface& plasmaInterface();

class FetchResourceListWorkItem : public WorkItem
{
protected:
	PlasmaInterface* m_interface;
public:
	FetchResourceListWorkItem(  )
	:m_interface( 0 ){}
	void SetInterface( PlasmaInterface* m_interface );
	void Run();

};


#endif //NO_TORRENT_SYSTEM

#endif
