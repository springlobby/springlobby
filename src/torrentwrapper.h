#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTWRAPPER_H

#ifndef NO_TORRENT_SYSTEM

//#ifdef _MSC_VER
/// MSVC can not compile std::pair used in bimap with forward decl only.
/// GCC cant compile TorrentTable::Row either.
#include <libtorrent/torrent_handle.hpp>
//#endif

#include <wx/arrstr.h>
#include <wx/event.h>

#include <map>
#include <vector>
#include <queue>

#include "iunitsync.h"
#include "thread.h"
#include "mutexwrapper.h"
#include "utils/plasmaresourceinfo.h"

/*
namespace libtorrent{ class session; };
namespace libtorrent { struct torrent_handle; };
*/
class TorrentWrapper;
class PlasmaInterface;

namespace P2P {
enum FileStatus
{
	not_stored	= 0, /// file is not on disk and not downloading
	queued		= 1, /// file is not on disk and queued for download
	leeching	= 2, /// file is being downloaded
	paused		= 3, /// file currently not downloading but has valid handle
	complete	= 4, /// file is on disk / dl completed
};
}

struct TorrentInfos
{
    float numcopies;
    wxString name;
    unsigned int downloaded;
    P2P::FileStatus downloadstatus;
    float progress;
    float inspeed;
    unsigned int filesize;

    int eta;

	//default constructor
	TorrentInfos() : numcopies(-1.f), downloaded(0), downloadstatus(P2P::not_stored), progress(0.f), inspeed(0.f), filesize(0), eta(0) {}
};


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


class TorrentWrapper
{
public:

    TorrentWrapper();
    ~TorrentWrapper();

    enum DownloadRequestStatus
    {
		success					= 0,
//		paused_ingame			= 1,
		file_not_found			= 2,
		torrent_join_failed		= 3,
		remote_file_dl_failed	= 4,
		corrupt_torrent_file	= 5,
		no_seeds_found			= 6
    };

    /// gui interface
    bool IsFileInSystem( const wxString& name );
    bool RemoveTorrentByName( const wxString& name );
    int GetTorrentSystemStatus();

    ///HashToTorrentData& GetSystemFileList();

    /// lobby interface
    void SetIngameStatus( bool status );

	//! will add name to a queue that's processed from the maint. thread, to avoid GUI blocking (maybe)
    void RequestFileByName( const wxString& name );

    //!remove all torrents that have seed status
    void ClearFinishedTorrents();

    void UpdateSettings();
    std::map<wxString,TorrentInfos> CollectGuiInfos();

	//! threaded maintenance tasks
    void JoinRequestedTorrents();
	void RemoveInvalidTorrents();
	void HandleCompleted();
	void TryToJoinQueuedTorrents();
    void ResumeFromList();

private:
	P2P::FileStatus getP2PStatusFromHandle( const libtorrent::torrent_handle& handle );
    DownloadRequestStatus _RequestFileByName( const wxString& name );
	void DisplayError( const wxString& resourcename, DownloadRequestStatus );

    typedef std::vector<libtorrent::torrent_handle>
        TorrenthandleVector;
    //! internal
    typedef std::map<PlasmaResourceInfo,libtorrent::torrent_handle>
        TorrenthandleInfoMap;
	MutexWrapper<TorrenthandleInfoMap> m_handleInfo_map;

	TorrenthandleInfoMap &GetHandleInfoMap()
	{
		ScopedLocker<TorrenthandleInfoMap> l_torrent_table(m_handleInfo_map);
		return l_torrent_table.Get();
	}

	void SetHandleInfoMap( TorrenthandleInfoMap info )
	{
		ScopedLocker<TorrenthandleInfoMap> l_torrent_table(m_handleInfo_map);
		l_torrent_table.Get() = info;
	}

    DownloadRequestStatus AddTorrent( const PlasmaResourceInfo& info );

	typedef std::queue<wxString>
		JoinQueue;
	JoinQueue m_join_queue;

    wxString m_buffer;

    bool ingame;
    unsigned int m_timer_count;

    wxArrayString m_tracker_urls;

    TorrentMaintenanceThread m_maintenance_thread;

    libtorrent::session* m_torr;

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

