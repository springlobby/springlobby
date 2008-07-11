#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#ifndef NO_TORRENT_SYSTEM

//#ifdef _MSC_VER
/// MSVC can not compile std::pair used in bimap with forward decl only.
/// GCC cant compile TorrentTable::Row either.
#include "libtorrent/torrent_handle.hpp"
//#endif

#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/event.h>

#include <map>
#include "bimap.h"

#include "inetclass.h"
#include "mutexwrapper.h"

#include "autopointers.h"

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201
/*
namespace libtorrent{ class session; };
namespace libtorrent { struct torrent_handle; };
*/
enum MediaType
{
  map,
  mod
};

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
    not_stored=0, /// file is not on disk and not downloading
    queued, /// file is not on disk and queued for download
    leeching,/// file is being downloaded
    stored,/// file is on disk
    seeding/// file is on disk and being seeded
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

/*
struct TorrentData
{
  wxString hash;
  wxString name;
  MediaType type;
  wxString infohash;
  bool ondisk;
};
*/

#define TorrentTable_validate

class TorrentTable{
  public:

  bool IsConsistent();


  class Row: public RefcountedContainer{
    /// If you want to modify row's keys, you need to remove it from table first,
    /// then re-insert
    public:
    wxString hash;/// key
    wxString name;/// key
    libtorrent::torrent_handle handle;/// key
    MediaType type;
    wxString infohash;
    //bool ondisk;

    FileStatus status;
    bool is_open;
    Row():
    type(map),
    status(not_stored)
    {
    }
  };
  typedef RefcountedPointer<Row> PRow;


  void InsertRow(PRow row);
  void RemoveRow(PRow row);
  void AddSeedRequest(PRow row);

  bool IsSeedRequest(PRow row);

/// Following methods return NULL if not found!
  PRow RowByHash(const wxString &hash);
  PRow RowByName(const wxString &name);
  PRow RowByHandle(libtorrent::torrent_handle handle);

  private:

  wxCriticalSection mutex;

#ifdef TorrentTable_validate
  std::set<PRow> all_torrents;
#endif
  std::map<wxString, PRow> hash_index;
  std::map<wxString, PRow> name_index;
  std::map<libtorrent::torrent_handle, PRow> handle_index;
  std::set<PRow> seed_requests;

};

/*
typedef std::map<wxString,TorrentData> HashToTorrentData;/// hash -> torr data
typedef codeproject::bimap<wxString,wxString> SeedRequests; ///name -> hash
typedef std::map<wxString,bool> OpenTorrents;/// name -> is seed
typedef codeproject::bimap<libtorrent::torrent_handle,wxString> TorrentHandleToHash; /// torrent handle -> hash
typedef codeproject::bimap<wxString,wxString> NameToHash;///name -> hash
*/

class TorrentWrapper : public iNetClass
{
  public:

    TorrentWrapper();
    ~TorrentWrapper();

    /// gui interface

    bool ConnectToP2PSystem();
    void DisconnectToP2PSystem();
    bool IsConnectedToP2PSystem();
    bool IsFileInSystem( const wxString& hash );
    void RemoveFile( const wxString& hash );
    int GetTorrentSystemStatus();

    ///HashToTorrentData& GetSystemFileList();

    /// lobby interface
    void SetIngameStatus( bool status );
    void ReloadLocalFileList();
    DownloadRequestStatus RequestFileByHash( const wxString& hash );
    DownloadRequestStatus RequestFileByName( const wxString& name );
    void UpdateSettings();
    void UpdateFromTimer( int mselapsed );
    std::map<int,TorrentInfos> CollectGuiInfos();
    void SendMessageToCoordinator( const wxString& message );

    TorrentTable &GetTorrentTable(){return m_torrent_table;}

  private:

    void CreateTorrent( const wxString& uhash, const wxString& name, MediaType type );
    bool JoinTorrent( const wxString& name );
    bool DownloadTorrentFileFromTracker( const wxString& hash );
    void FixTorrentList();
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


    ///MutexWrapper<>
    TorrentTable m_torrent_table;

/*

    MutexWrapper<HashToTorrentData> m_torrents_infos;
    MutexWrapper<SeedRequests> m_seed_requests;
    MutexWrapper<OpenTorrents> m_open_torrents;
    MutexWrapper<TorrentHandleToHash> m_torrent_handles;
    MutexWrapper<NameToHash> m_name_to_hash;
    MutexWrapper<wxArrayString> m_queued_requests; /// contains hashes of pending requested files when already leeching > 4 files
*/

    libtorrent::session* m_torr;
    Socket* m_socket_class;
    unsigned int m_connected_tracker_index;
};


TorrentWrapper* torrent();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
