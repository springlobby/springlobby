#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#include <wx/string.h>
#include <wx/arrstr.h>

#include <vector>
#include <list>

#include "inetclass.h"

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201

enum MediaType
{
  map,
  mod
};

struct TorrentData
{
  wxString hash;
  wxString name;
  MediaType type;
  wxArrayString seedurls;
};

namespace libtorrent{ class session; };
class Socket;

class TorrentWrapper : public iNetClass
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    void ConnectToP2PSystem();
    void DisconnectToP2PSystem();
    void ChangeListeningPort( unsigned int port );

    void ReloadLocalFileList();

    bool RequestFile( const wxString& hash );
  private:

    void CreateTorrent( const wxString& uhash, const wxString& name, MediaType type );
    void JoinTorrent( const wxString& name );

    void ReceiveandExecute( const wxString& msg );
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    wxString m_buffer;
    bool m_connected;

    wxArrayString m_tracker_urls;
    unsigned int m_open_torrents_number;

    std::vector<TorrentData> m_torrents_infos;
    typedef std::vector<TorrentData>::iterator TorrentsIter;
    std::list<wxString> m_seed_requests;
    typedef std::list<wxString>::iterator SeedReqIter;
    std::vector<TorrentData> m_local_files;
    typedef std::vector<TorrentData>::iterator LocalFilesIter;

    libtorrent::session* m_torr;
    Socket* m_socket_class;
};

TorrentWrapper* torrent();

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
