#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#ifndef NO_TORRENT_SYSTEM

//#ifdef _MSC_VER
/// MSVC can not compile std::pair used in bimap with forward decl only.
/// GCC cant compile TorrentTable::Row either.
#include "libtorrent/torrent_handle.hpp"
//#endif

#include <wx/arrstr.h>

#include <map>

#include "inetclass.h"
#include "mutexwrapper.h"
#include "iunitsync.h"
#include "thread.h"

#include "autopointers.h"

#define DEFAULT_P2P_COORDINATOR_PORT 8202
#define DEFAULT_P2P_TRACKER_PORT 8201
/*
namespace libtorrent{ class session; };
namespace libtorrent { struct torrent_handle; };
*/
class TorrentWrapper;

namespace P2P {
enum FileStatus
{
    /// Dont change values. Bit arithmetics is used in TorrentTable::Row
    not_stored=0, /// file is not on disk and not downloading
    queued=1, /// file is not on disk and queued for download
    leeching=2,/// file is being downloaded
    stored=128,/// file is on disk
    seeding=129/// file is on disk and being seeded
};
}

struct TorrentInfos
{
    float numcopies;
    wxString name;
    unsigned int downloaded;
    unsigned int uploaded;
    P2P::FileStatus downloadstatus;
    float progress;
    float inspeed;
    float outspeed;
    unsigned int filesize;
    wxString hash;
    int eta; 

	//default constructor
	TorrentInfos() : numcopies(-1.f), downloaded(0), uploaded(0), downloadstatus(P2P::not_stored), progress(0.f), inspeed(0.f), outspeed(0.f), eta(0) {}
};


#define TorrentTable_validate

class TorrentMaintenanceThread : public Thread
{
	public:
		TorrentMaintenanceThread( TorrentWrapper* parent );
		void Init();
		void Stop();
		void* Entry();

	protected:
		bool TestDestroy();

		bool m_stop_thread;
		TorrentWrapper& m_parent;
};

class TorrentTable
{
public:

    bool IsConsistent();

    TorrentTable():
    m_seed_count(0),
    m_leech_count(0)
    {
    }

class Row: public RefcountedContainer
    {
        /// If you want to modify row's keys, you need to remove it from table first,
        /// then re-insert
    public:
        wxString hash;/// key, unitsync hash
        wxString name;/// key, unitsync name
        libtorrent::torrent_handle handle;/// key
        IUnitSync::MediaType type;
        wxString infohash; /// torrent sha1 infohash in b64
        //bool ondisk;

        P2P::FileStatus status;
        bool is_open;
        Row():
                type(IUnitSync::map),
                status(P2P::not_stored)
        {
        }
        bool HasFullFileLocal()
        {
            return status & P2P::stored;
        }
        void SetHasFullFileLocal(bool b=true)
        {
            if (b)
            {
                if ( !status & P2P::stored )
                    status=P2P::stored;
            }
            else
            {
                if ( status & P2P::stored )
                    status=P2P::not_stored;
            }
        }
    };
    typedef RefcountedPointer<Row> PRow;


		struct TransferredData
		{
			unsigned int failed_check_counts;
			unsigned int sentdata;
			TransferredData(): failed_check_counts(0), sentdata(0) {}
		};

		// deletes all stored infos
		void FlushData();

    void InsertRow(PRow row);
    void RemoveRow(PRow row);

    /// row must be already inserted
    void AddSeedRequest(PRow row);
    void RemoveSeedRequest(PRow row);
    void SetRowHandle(PRow row, const libtorrent::torrent_handle &handle);
    void AddRowToDependencyCheckQueue(PRow row);
    void RemoveRowFromDependencyCheckQueue(PRow row);
    void RemoveRowHandle( PRow row );
    void SetRowStatus( PRow row, P2P::FileStatus status );
    void SetRowTransferredData( PRow row, TransferredData data );

    bool IsSeedRequest(PRow row);

/// Following methods return NULL if not found!
    PRow RowByHash(const wxString &hash);
    PRow RowByName(const wxString &name);
    PRow RowByHandle(libtorrent::torrent_handle handle);
    std::map<wxString, TorrentTable::PRow> RowsByHash();
    std::set<PRow> SeedRequestsByRow();
    std::map<libtorrent::torrent_handle, PRow> RowByTorrentHandles();
    std::set<PRow> QueuedTorrentsByRow();
    std::map<TorrentTable::PRow, TransferredData> TransferredDataByRow();
    std::set<TorrentTable::PRow> DependencyCheckQueuebyRow();

    unsigned int GetOpenSeedsCount();
    unsigned int GetOpenLeechsCount();

private:


#ifdef TorrentTable_validate
    std::set<PRow> all_torrents;
#endif
    std::map<wxString, PRow> hash_index;
    std::map<wxString, PRow> name_index;
    std::map<libtorrent::torrent_handle, PRow> handle_index;
    std::set<PRow> seed_requests;
    std::set<PRow> queued_torrents;
    std::map<TorrentTable::PRow, TorrentTable::TransferredData>  seed_sent_data;
		std::set<TorrentTable::PRow> dep_check_queue;

    unsigned int m_seed_count;
    unsigned int m_leech_count;
};

class TorrentWrapper : public iNetClass
{
public:

    TorrentWrapper();
    ~TorrentWrapper();

    enum DownloadRequestStatus
    {
        success,
        not_connected,
        paused_ingame,
        duplicate_request,
        file_not_found,
        torrent_join_failed,
        scheduled_in_cue,
        missing_in_table
    };

    /// gui interface

    bool ConnectToP2PSystem(const unsigned int tracker_no = 0);
    void DisconnectFromP2PSystem();
    bool IsConnectedToP2PSystem();
    bool IsFileInSystem( const wxString& hash );
    bool RemoveTorrentByHash( const wxString& hash );
	P2P::FileStatus GetTorrentStatusByHash(const wxString& hash);
    int GetTorrentSystemStatus();

    ///HashToTorrentData& GetSystemFileList();

    /// lobby interface
    void SetIngameStatus( bool status );
    DownloadRequestStatus RequestFileByHash( const wxString& hash );
    DownloadRequestStatus RequestFileByName( const wxString& name );
    void UpdateSettings();
    void UpdateFromTimer( int mselapsed );
    std::map<wxString,TorrentInfos> CollectGuiInfos();
    void SendMessageToCoordinator( const wxString& message );

    /// threaded maintenance tasks
    void JoinRequestedTorrents();
    void RemoveUnneededTorrents();
    void TryToJoinQueuedTorrents();
    void SearchAndGetQueuedDependencies();
		void ResumeFromList();

    TorrentTable &GetTorrentTable()
    {
				ScopedLocker<TorrentTable> l_torrent_table(m_torrent_table);
        return l_torrent_table.Get();
    }

private:

    void CreateTorrent( const wxString& uhash, const wxString& name, IUnitSync::MediaType type );
    DownloadRequestStatus RequestFileByRow( const TorrentTable::PRow& row );
    DownloadRequestStatus QueueFileByRow( const TorrentTable::PRow& row );
    bool RemoveTorrentByRow( const TorrentTable::PRow& row );
    bool JoinTorrent( const TorrentTable::PRow& row, bool IsSeed );
    bool DownloadTorrentFileFromTracker( const wxString& hash );

    void ReceiveandExecute( const wxString& msg );
    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    wxString m_buffer;

    bool ingame;
    unsigned int m_timer_count;

    wxArrayString m_tracker_urls;

    MutexWrapper<TorrentTable> m_torrent_table;

    TorrentMaintenanceThread m_maintenance_thread;

    libtorrent::session* m_torr;
    Socket* m_socket_class;


    //!we set this when trying a tracker and waiting for connection to be established
    bool m_is_connecting;

    unsigned int m_connected_tracker_index;

    bool m_started;

};


TorrentWrapper& torrent();

#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

