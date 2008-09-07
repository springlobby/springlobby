#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#ifndef NO_TORRENT_SYSTEM

#ifdef _MSC_VER
// MSVC can not compile std::pair used in bimap with forward decl only.
#include "libtorrent/torrent_handle.hpp"
#endif

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/event.h>

#include <map>
#include "bimap.h"

#include "inetclass.h"
#include "mutexwrapper.h"
#include "iunitsync.h"

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201

namespace libtorrent{ class session; };
namespace libtorrent { struct torrent_handle; };

enum DownloadRequestStatus
{
  success,
  not_connected,
  paused_ingame,
  duplicate_request,
  file_not_found,
  torrent_join_failed,
  scheduled_in_cue
};

enum FileStatus
{
  leeching,
  seeding,
  queued
};

struct TorrentInfos
{
  float numcopies;
  wxString name;
  unsigned int downloaded;
  unsigned int uploaded;
  FileStatus downloadstatus;
  float progress;
  float inspeed;
  float outspeed;
  unsigned int filesize;
  wxString hash;
  int eta; //is set in update function of maintorrenttab
};

struct TorrentData
{
  wxString hash;
  wxString name;
  MediaType type;
  wxString infohash;
  bool ondisk;
};


typedef std::map<wxString,TorrentData> HashToTorrentData;/// hash -> torr data
typedef codeproject::bimap<wxString,wxString> SeedRequests; ///name -> hash
typedef std::map<wxString,bool> OpenTorrents;/// name -> is seed
typedef codeproject::bimap<libtorrent::torrent_handle,wxString> TorrentHandleToHash; /// torrent handle -> hash
typedef codeproject::bimap<wxString,wxString> NameToHash;///name -> hash

class TorrentWrapper : public iNetClass
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    /// gui interface

    bool ConnectToP2PSystem(const unsigned int tracker_no = 0);
    void DisconnectToP2PSystem();
    bool IsConnectedToP2PSystem();
    bool IsFileInSystem( const wxString& hash );
    void RemoveFile( const wxString& hash );
    int GetTorrentSystemStatus();
    HashToTorrentData& GetSystemFileList();

    /// lobby interface
    void SetIngameStatus( bool status );
    DownloadRequestStatus RequestFileByHash( const wxString& hash );
    DownloadRequestStatus RequestFileByName( const wxString& name );
    void UpdateSettings();
    void UpdateFromTimer( int mselapsed );
    std::map<int,TorrentInfos> CollectGuiInfos();
    void SendMessageToCoordinator( const wxString& message );

  private:

    void CreateTorrent( const wxString& uhash, const wxString& name, MediaType type );
    bool JoinTorrent( const wxString& name );
    bool DownloadTorrentFileFromTracker( const wxString& hash );
    void JoinRequestedTorrents();
    void RemoveUnneededTorrents();
    void TryToJoinQueuedTorrents();
    void ResumeFromList();

    void ReceiveandExecute( const wxString& msg );
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    wxString m_buffer;

    bool ingame;
    unsigned int m_seed_count;
    unsigned int m_leech_count;
    unsigned int m_timer_count;

    wxArrayString m_tracker_urls;

    HashToTorrentData m_torrent_infos; /// hash -> torr data

    SeedRequests m_seed_requests; ///name -> hash

    OpenTorrents m_open_torrents; /// name -> is seed

    TorrentHandleToHash m_torrent_handles;  /// torrent handle -> hash

    NameToHash m_name_to_hash; ///name -> hash

    wxArrayString m_queued_requests; /// contains hashes of pending requested files when already leeching > 4 files

    libtorrent::session* m_torr;
    Socket* m_socket_class;

    //!we set this when trying a tracker and waiting for connection to be established
    bool m_is_connecting;

    unsigned int m_connected_tracker_index;
};


TorrentWrapper* torrent();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
