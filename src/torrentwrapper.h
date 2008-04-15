#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#include <wx/string.h>
#include <wx/arrstr.h>

#include <vector>
#include <list>

#include "inetclass.h"

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201

namespace libtorrent{ class session; };
class Socket;

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

class TorrentWrapper : public iNetClass
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    /// gui interface

    void ConnectToP2PSystem();
    void DisconnectToP2PSystem();
    void ChangeListeningPort( unsigned int port );
    void ChangeUploadSpeedLimit( unsigned int speed );
    void ChangeDownloadSpeedLimit( unsigned int speed );

    /// lobby interface
    void SetIngameStatus( bool status );
    void ReloadLocalFileList();
    bool RequestFile( const wxString& hash );

  private:

    void CreateTorrent( const wxString& uhash, const wxString& name, MediaType type );
    void JoinTorrent( const wxString& name );
    bool DownloadTorrentFileFromTracker( const wxString& shash );

    void ReceiveandExecute( const wxString& msg );
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    bool m_connected;

    bool ingame;

    wxArrayString m_tracker_urls;

    std::map<wxString,TorrentData> m_torrents_infos;
    typedef std::map<wxString,TorrentData>::iterator TorrentsIter;
    std::list<wxString> m_seed_requests;
    typedef std::list<wxString>::iterator SeedReqIter;
    std::map<wxString,TorrentData> m_local_files;
    typedef std::map<wxString,TorrentData>::iterator LocalFilesIter;
    std::map<wxString,long> m_seed_joined;
    typedef std::map<wxString,bool>::iterator SeedOpenIter;
    std::map<wxString,long> m_leech_joined;
    typedef std::map<wxString,bool>::iterator LeechOpenIter;

    libtorrent::session* m_torr;
    Socket* m_socket_class;
};

TorrentWrapper* torrent();

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
