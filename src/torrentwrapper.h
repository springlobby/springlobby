#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#ifndef NO_TORRENT_SYSTEM

#include <wx/string.h>
#include <wx/arrstr.h>

#include <map>

#include "inetclass.h"
#include "mutexwrapper.h"

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201

namespace libtorrent{ class session; };
class Socket;

enum MediaType
{
  map,
  mod
};

struct TorrentInfos
{
  float numcopies;
  wxString name;
  unsigned int downloaded;
  unsigned int uploaded;
  bool leeching;
  float progress;
  float inspeed;
  float outspeed;
  int filehash;
};

struct TorrentData
{
  wxString hash;
  wxString name;
  MediaType type;
  wxString infohash;
};


class TorrentWrapper : public iNetClass
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    /// gui interface

    void ConnectToP2PSystem();
    void DisconnectToP2PSystem();
    bool IsConnectedToP2PSystem();
    bool IsFileInSystem( const wxString& uhash );

    /// lobby interface
    void SetIngameStatus( bool status );
    void ReloadLocalFileList();
    bool RequestFile( const wxString& hash );
    void UpdateSettings();
    void UpdateFromTimer( int mselapsed );
    std::map<int,TorrentInfos> CollectGuiInfos();

  private:

    void CreateTorrent( const wxString& uhash, const wxString& name, MediaType type );
    bool JoinTorrent( const wxString& name );
    bool DownloadTorrentFileFromTracker( const wxString& shash );
    void FixTorrentList();

    void ReceiveandExecute( const wxString& msg );
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    bool m_connected;

    bool ingame;
    unsigned int m_seed_count;
    unsigned int m_leech_count;
    unsigned int m_timer_count;

    wxArrayString m_tracker_urls;

    typedef std::map<wxString,TorrentData> HashToTorrentData;/// shash -> torr data
    MutexWrapper<HashToTorrentData> m_torrents_infos;

    typedef std::map<wxString,wxString> SeedRequests;
    MutexWrapper<SeedRequests> m_seed_requests; ///name -> hash

    MutexWrapper<HashToTorrentData> m_local_files; /// shash -> torrent data


    typedef std::map<wxString,bool> OpenTorrents;
    MutexWrapper<OpenTorrents> m_open_torrents; /// name -> is seed


    libtorrent::session* m_torr;
    Socket* m_socket_class;
    unsigned int m_connected_tracker_index;
};


TorrentWrapper* torrent();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
