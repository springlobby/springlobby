#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#ifndef NO_TORRENT_SYSTEM

//#ifdef _MSC_VER
/// MSVC can not compile std::pair used in bimap with forward decl only.
/// GCC cant compile TorrentTable::Row either.
#include <libtorrent/torrent_handle.hpp>
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
class PlasmaInterface;

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
	TorrentInfos() : numcopies(-1.f), downloaded(0), uploaded(0), downloadstatus(P2P::not_stored), progress(0.f), inspeed(0.f), outspeed(0.f), filesize(0), eta(0) {}
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
    m_leech_count(0)
    {
    }

    struct TransferredData
    {
        unsigned int failed_check_counts;
        unsigned int sentdata;
        TransferredData(): failed_check_counts(0), sentdata(0) {}
    };

    // deletes all stored infos
    void FlushData();

    unsigned int GetOpenLeechsCount();

private:
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
    std::map<wxString,TorrentInfos> CollectGuiInfos();

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

    bool GetTorrentFileAndInfos( const wxString& resourceName );

    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    virtual void OnDataReceived( Socket* ) {};

    wxString m_buffer;

    bool ingame;
    unsigned int m_timer_count;

    wxArrayString m_tracker_urls;

    MutexWrapper<TorrentTable> m_torrent_table;

    TorrentMaintenanceThread m_maintenance_thread;

    libtorrent::session* m_torr;

    bool m_started;

    PlasmaInterface* m_plasma_interface;
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

