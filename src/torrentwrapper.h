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

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201

namespace libtorrent{ class session; };
namespace libtorrent { struct torrent_handle; };

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
  bool seeding;
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

    void ConnectToP2PSystem();
    void DisconnectToP2PSystem();
    bool IsConnectedToP2PSystem();
    bool IsFileInSystem( const wxString& hash );
    void RemoveFile( const wxString& hash );
    int GetTorrentSystemStatus();
    HashToTorrentData GetSystemFileList();

    /// lobby interface
    void SetIngameStatus( bool status );
    void ReloadLocalFileList();
    bool RequestFileByHash( const wxString& hash );
    bool RequestFileByName( const wxString& name );
    void UpdateSettings();
    void UpdateFromTimer( int mselapsed );
    std::map<int,TorrentInfos> CollectGuiInfos();
    void SendMessageToCoordinator( const wxString& message );

  private:

    void CreateTorrent( const wxString& uhash, const wxString& name, MediaType type );
    bool JoinTorrent( const wxString& name );
    bool DownloadTorrentFileFromTracker( const wxString& hash );
    void FixTorrentList();

    void ReceiveandExecute( const wxString& msg );
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    bool m_connected;
    wxString m_buffer;

    bool ingame;
    unsigned int m_seed_count;
    unsigned int m_leech_count;
    unsigned int m_timer_count;

    wxArrayString m_tracker_urls;


/// Thread safety rules (all of them equally important. ) :
/// 1: Minimize duration of locks by putting ScopedLocker into its own {} block
///  Ideally, the only things that happen while locked are reading and writing (and iterator access).
///  (however, thats sometimes slightly painful to write, because that requirs lot of temp variables.)
/// 2: While locked:
/// 3:  -Try to avoid doing function calls while locked (only call simple functions like sqrt etc, but not calls to unitsync or libtorrent);
/// 4:  -Use temporary variables to do calls after unlock (outside of ScopedLocker's scope).
/// 5:  -*Never* call function that might want to lock those mutexes. Locking, then calling function which wants to lock again *will* cause simple deadlock.
/// 6:  -*Never* call functions that may take time to execute
/// 7: Try to avoid locking multiple things at once.
/// 8: When locking multiple things at once, *always* do locks in same order as declaration order here.
/// (to prevent deadlocks when multiple threads do that.
/// When one thread does lock a lock b and other lock b lock a, you get deadlock)

/// there probably are some more rules i dont know of, or which i forgot.

    MutexWrapper<HashToTorrentData> m_torrents_infos;

    MutexWrapper<SeedRequests> m_seed_requests;

    MutexWrapper<OpenTorrents> m_open_torrents;

    MutexWrapper<TorrentHandleToHash> m_torrent_handles;

    MutexWrapper<NameToHash> m_name_to_hash;

    libtorrent::session* m_torr;
    Socket* m_socket_class;
    unsigned int m_connected_tracker_index;
};


TorrentWrapper* torrent();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
