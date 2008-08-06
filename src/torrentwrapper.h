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
  /// Dont change values. Bit arithmetics is used in TorrentTable::Row
  not_stored=0, /// file is not on disk and not downloading
  queued=1, /// file is not on disk and queued for download
  leeching=2,/// file is being downloaded
  stored=128,/// file is on disk
  seeding=129/// file is on disk and being seeded
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
    bool HasFullFileLocal(){
      return status&stored;
    }
    void SetHasFullFileLocal(bool b=true){
      if(b){
        if(!status&stored)status=stored;
      }else{
        if(status&stored)status=not_stored;
      }
    }
  };
  typedef RefcountedPointer<Row> PRow;


  void InsertRow(PRow row);
  void RemoveRow(PRow row);

  /// row must be already inserted
  void AddSeedRequest(PRow row);
  void RemoveSeedRequest(PRow row);
  void SetRowHandle(PRow row, const libtorrent::torrent_handle &handle);

  bool IsSeedRequest(PRow row);

/// Following methods return NULL if not found!
  PRow RowByHash(const wxString &hash);
  PRow RowByName(const wxString &name);
  PRow RowByHandle(libtorrent::torrent_handle handle);
  std::map<wxString, TorrentTable::PRow> RowsByHash();

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
=======
>>>>>>> bd/master:src/torrentwrapper.h
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

    ///MutexWrapper<>
    TorrentTable m_torrent_table;

    wxArrayString m_queued_requests_hashes;/// hashes here.

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
