/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#ifndef NO_TORRENT_SYSTEM

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include "settings.h"
#include "utils/conversion.h"
#include "utils/debug.h"
#include "socket.h"
#include "base64.h"

#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/storage.hpp>
#include <libtorrent/hasher.hpp>
#include <libtorrent/file_pool.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/file.hpp>
#if LIBTORRENT_VERSION_MINOR >= 14
	#include <libtorrent/create_torrent.hpp>
#endif
#include <libtorrent/extensions/metadata_transfer.hpp>
#include <libtorrent/extensions/ut_pex.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>

#include <fstream>

#include <wx/tokenzr.h>
#include <wx/protocol/http.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/wfstream.h>
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <wx/event.h>

#include "torrentwrapper.h"
#include "utils/customdialogs.h"
#include "utils/downloader.h"
#include "globalsmanager.h"
#include "utils/globalevents.h"

#ifdef __WXMSW__
    #include "utils/platform.h"
#endif

/** Get the name of the Spring data subdirectory that corresponds to a
 * given IUnitSync::MediaType value.
 */
inline wxString
getDataSubdirForType(const IUnitSync::MediaType type)
{
    switch ( type)
    {
    case IUnitSync::map:
        return _T("maps");
    case IUnitSync::mod:
        return _T("mods");
    default:
        ASSERT_EXCEPTION(false, _T("Unhandled IUnitSync::MediaType value"));
    }
}

TorrentMaintenanceThread::TorrentMaintenanceThread( TorrentWrapper* parent ):
m_stop_thread( false ),
m_parent( *parent )
{
}

void TorrentMaintenanceThread::Init()
{
	Create();
	SetPriority( WXTHREAD_MIN_PRIORITY );
	Run();
}

void TorrentMaintenanceThread::Stop()
{
	m_stop_thread = true;
}

void* TorrentMaintenanceThread::Entry()
{
	while ( !TestDestroy() )
	{
		if( !Sleep( 2000 ) ) break;
//		if ( m_parent.IsConnectedToP2PSystem() )
		{
				//  DON'T alter function call order here or bad things may happend like locust, earthquakes or raptor attack
				m_parent.JoinRequestedTorrents();
				m_parent.RemoveUnneededTorrents();
				m_parent.HandleCompleted();
				m_parent.TryToJoinQueuedTorrents();
				m_parent.SearchAndGetQueuedDependencies();
		}
	}
	return 0;
}

bool TorrentMaintenanceThread::TestDestroy()
{
	return Thread::TestDestroy() || m_stop_thread;
}

TorrentWrapper& torrent()
{
    static LineInfo<TorrentWrapper> m( AT );
    static GlobalObjectHolder<TorrentWrapper,LineInfo<TorrentWrapper> > m_torr_wrap( m );
    return m_torr_wrap;
}


TorrentWrapper::TorrentWrapper():
        ingame(false),
        m_timer_count(0),
        m_maintenance_thread(this),
        m_started(false)
{
    wxLogMessage(_T("TorrentWrapper::TorrentWrapper()"));
    m_torr = new libtorrent::session( libtorrent::fingerprint("SL", 0, 0, 0, 0), 0 );
    try
    {
        m_torr->add_extension(&libtorrent::create_metadata_plugin);
    }
    catch (std::exception& e)
    {
        wxLogError( TowxString( e.what() ) );
    }
    try
    {
        m_torr->add_extension(&libtorrent::create_ut_pex_plugin);
    }
    catch (std::exception& e)
    {
        wxLogError( TowxString( e.what() ) );
    }
    //these extensions proved o be too problematic on win so i flat out disable them
    #ifndef __WXMSW__
        try
        {
            m_torr->start_upnp();
        }
        catch (std::exception& e)
        {
            wxLogError( TowxString( e.what() ) );
        }
        try
        {
            m_torr->start_natpmp();
        }
        catch (std::exception& e)
        {
            wxLogError( TowxString( e.what() ) );
        }
        try
        {
            m_torr->start_lsd();
        }
        catch (std::exception& e)
        {
            wxLogError( TowxString( e.what() ) );
        }
    #endif
    UpdateSettings();
    m_maintenance_thread.Init();
}


TorrentWrapper::~TorrentWrapper()
{
    wxLogDebugFunc( wxEmptyString );
    m_maintenance_thread.Stop();
    #ifndef __WXMSW__
        try
        {
            m_torr->stop_upnp();
        }
        catch (std::exception& e)
        {
            wxLogError( TowxString( e.what() ) );
        }
        try
        {
            m_torr->stop_natpmp();
        }
        catch (std::exception& e)
        {
            wxLogError( TowxString( e.what() ) );
        }
        try
        {
            m_torr->stop_lsd();
        }
        catch (std::exception& e)
        {
            wxLogError( TowxString( e.what() ) );
        }
    #endif
    delete m_torr;
}


////////////////////////////////////////////////////////
////                gui interface                   ////
////////////////////////////////////////////////////////

void TorrentWrapper::UpdateSettings()
{
    try
    {
        int uploadLimit, downloadLimit;
        if ( !ingame || sett().GetTorrentSystemSuspendMode() == 0 )
        {
            uploadLimit = sett().GetTorrentUploadRate();
            downloadLimit = sett().GetTorrentDownloadRate();
        }
        else
        {
            uploadLimit = sett().GetTorrentThrottledUploadRate();
            downloadLimit = sett().GetTorrentThrottledDownloadRate();
        }

        uploadLimit = uploadLimit < 0 ? -1 : uploadLimit * 1024;
        downloadLimit = downloadLimit < 0 ? -1 : downloadLimit * 1024;

        m_torr->set_upload_rate_limit(uploadLimit);
        m_torr->set_download_rate_limit(downloadLimit);

        m_torr->set_max_connections(sett().GetTorrentMaxConnections());

        m_torr->listen_on(std::make_pair(sett().GetTorrentPort(), sett().GetTorrentPort()));

    }
    catch (std::exception& e)
    {
        wxLogError( TowxString( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }
}


bool TorrentWrapper::IsFileInSystem( const wxString& name )
{
    return false;//GetTorrentTable().RowByHash(hash).ok();
}


bool TorrentWrapper::RemoveTorrentByName( const wxString& name )
{
    TorrenthandleInfoMap::iterator it = m_handleInfo_map.begin();
    for ( ; it != m_handleInfo_map.end(); ++it )
    {
        PlasmaResourceInfo info = it->first;
        libtorrent::torrent_handle handle = it->second;
        if ( info.m_name == name )
        {
            m_torr->remove_torrent( handle );
            m_handleInfo_map.erase( it++ );
            return true;
        }
    }
    return false;
}


P2P::FileStatus TorrentWrapper::GetTorrentStatusByName(const wxString& name )
{
//	TorrentTable::PRow row=GetTorrentTable().RowByHash(hash);
//    if (!row.ok())
//		return P2P::not_stored;
//
//	libtorrent::torrent_handle handle = row->handle;
//	return row->status;
    return P2P::not_stored;
}


int TorrentWrapper::GetTorrentSystemStatus()
{
    if (ingame) return 2;
    return 1;
}


////////////////////////////////////////////////////////
////               lobby interface                  ////
////////////////////////////////////////////////////////

IUnitSync::MediaType convertMediaType( const PlasmaResourceInfo::ResourceType& t ) {
    switch ( t ) {
        case PlasmaResourceInfo::map: return IUnitSync::map;
        default: return IUnitSync::mod;
    }

}

void TorrentWrapper::RequestFileByName( const wxString& name )
{
	m_join_queue.push( name );
}

TorrentWrapper::DownloadRequestStatus TorrentWrapper::_RequestFileByName( const wxString& name )
{
    PlasmaResourceInfo info = plasmaInterface().GetResourceInfo( name );
	info.m_name = name;
    if( info.m_type == PlasmaResourceInfo::unknwon )
        return remote_file_dl_failed; //!TODO use ebtter code

	if ( plasmaInterface().DownloadTorrentFile( info, sett().GetTorrentDir().GetFullPath() ) )
    {
        if ( AddTorrent( info ) == success ) {
            for ( size_t i = 0; i < info.m_dependencies.Count(); ++i ) {
                wxString dependency_name = info.m_dependencies[i];
                PlasmaResourceInfo dependency_info = plasmaInterface().GetResourceInfo( dependency_name );
                if ( dependency_info.m_type != PlasmaResourceInfo::unknwon )
                    continue;
                if ( plasmaInterface().DownloadTorrentFile( dependency_info, sett().GetTorrentDataDir().GetFullPath() ) )
                    AddTorrent( dependency_info );
            }
            return success;
        }
    }
    return remote_file_dl_failed;
}

TorrentWrapper::DownloadRequestStatus TorrentWrapper::AddTorrent( const PlasmaResourceInfo& info )
{
    wxMutexLocker lock( m_info_map_mutex );
    libtorrent::add_torrent_params p;

    try {
        // the torrent_info is stored in an intrusive_ptr
		boost::filesystem::path torfile_b_path( info.m_local_torrent_filepath.mb_str() );
		p.ti = new libtorrent::torrent_info( torfile_b_path );
		wxString dl_dir_path = sett().GetTorrentDataDir().GetFullPath() + wxFileName::GetPathSeparator();
        p.save_path = boost::filesystem::path( dl_dir_path.mb_str() );
    }
    catch ( std::exception& exc ) {
            wxLogMessage( _T("torrent has invalid encoding") );
            return corrupt_torrent_file;
    }
    try {
        //if we have no seeds the torrent is useless
        size_t num_webseeds = info.m_webseeds.Count();
        if ( num_webseeds < 1 )
            return no_seeds_found;

        libtorrent::torrent_handle tor = m_torr->add_torrent(p);
        m_handleInfo_map[info] = tor;
        for ( size_t i = 0; i < num_webseeds; ++ i )
            tor.add_url_seed( STD_STRING( info.m_webseeds[i] ) );
        return success;
    }
    catch (std::exception& e) {
       wxLogError(_T("%s"),TowxString( e.what()).c_str()); // TODO (BrainDamage#1#): add message to user on failure
       return missing_in_table;
    }
}

void TorrentWrapper::SetIngameStatus( bool status )
{
    if ( status == ingame ) return; // no change needed
    ingame = status;
    if ( ingame )
        m_maintenance_thread.Pause();
    else
        m_maintenance_thread.Resume();

    try
    {
        TorrenthandleVector torrentList = m_torr->get_torrents();
        if ( ingame ) // going ingame, pause all torrents (or throttle speeds) and disable dht
        {
            if ( sett().GetTorrentSystemSuspendMode() == 0 ) for ( unsigned int i = 0; i < torrentList.size(); i++)
                torrentList[i].pause();
            else
            {
                m_torr->set_upload_rate_limit(sett().GetTorrentThrottledUploadRate() * 1024);
                m_torr->set_download_rate_limit(sett().GetTorrentThrottledDownloadRate() *1024 );
            }
            m_torr->stop_dht();
        }
        else// game closed, resume all torrents (or reset normal speed) and reactivate dht
        {
            m_torr->start_dht();
            if ( sett().GetTorrentSystemSuspendMode() == 0 ) for ( unsigned int i = 0; i < torrentList.size(); i++) torrentList[i].resume();
            else
            {
                m_torr->set_upload_rate_limit(sett().GetTorrentUploadRate() * 1024);
                m_torr->set_download_rate_limit(sett().GetTorrentDownloadRate() *1024 );
            }
        }
    }
    catch (std::exception& e)
    {
        wxLogError( TowxString( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }
}

void TorrentWrapper::ResumeFromList()
{
//    wxArrayString TorrentsToResume = sett().GetTorrenthandleVectorToResume();
//    unsigned int ResumeCount = TorrentsToResume.GetCount();
//    if ( ResumeCount > 0 )
//    {
//        //request all hashes in list, remember successes
//        std::vector<int> successfulIndices;
//        for ( unsigned int i = 0; i < ResumeCount; i++ )
//        {
////            if (scheduled_in_cue == RequestFileByHash( TorrentsToResume[i] ) ) // resume all open leeched files when system as disconnected last time
////                successfulIndices.push_back(i);
//            assert( false );
//        }
//
//        //remove successfully resumed torrents from list
//        std::vector<int>::const_iterator it = successfulIndices.begin();
//        for ( ; it != successfulIndices.end(); ++it )
//            TorrentsToResume.RemoveAt( *it );
//        //save new list (hopefully empty)
//        sett().SetTorrenthandleVectorToResume( TorrentsToResume );
//    }
}

////////////////////////////////////////////////////////
//// private functions to interface with the system ////
////////////////////////////////////////////////////////
void TorrentWrapper::JoinRequestedTorrents()
{
	while ( m_join_queue.size() > 0 )
	{
		wxString resourcename = m_join_queue.front();
		DownloadRequestStatus stat = _RequestFileByName( resourcename );
		if ( stat != TorrentWrapper::success )
			customMessageBoxNoModal( SL_MAIN_ICON, _("dl failed"), _("dl failed") );
		m_join_queue.pop();
	}
}

void TorrentWrapper::HandleCompleted()
{
	wxMutexLocker lock( m_info_map_mutex );
	TorrenthandleInfoMap::iterator it = m_handleInfo_map.begin();
	int num_completed = 0;
	for ( ; it != m_handleInfo_map.end(); ++it )
	{
		PlasmaResourceInfo info = it->first;
		libtorrent::torrent_handle handle = it->second;
		if ( handle.is_valid() && handle.is_seed() )
		{
			wxString dest_filename = sett().GetCurrentUsedDataDir() +
									 getDataSubdirForType( convertMediaType( info.m_type ) ) +
									 wxFileName::GetPathSeparator() +
									 TowxString( handle.get_torrent_info().file_at( 0 ).path.string() );
			if ( !wxFileExists( dest_filename ) )
			{
				wxString source_path = TowxString( handle.save_path().string() )  +
									   wxFileName::GetPathSeparator() +
									   TowxString( handle.get_torrent_info().file_at( 0 ).path.string() );
				bool ok = wxCopyFile( source_path, dest_filename );
				if ( !ok )
				{
//					customMessageBoxNoModal( SL_MAIN_ICON, wxString::Format( _("File copy from %s to %s failed.\nPlease copy manually and reload maps/mods afterwards"),
//																			 source_path.c_str(), dest_filename.c_str() ),
//											 _("Copy failed") );
					wxLogError( wxString::Format( _("File copy from %s to %s failed.\nPlease copy manually and reload maps/mods afterwards"),
																			 source_path.c_str(), dest_filename.c_str() ) );
				}
				else
				{
					num_completed++;
				}
			}
//			else	wxLogError( wxString::Format( _("File exists at %s"), dest_filename.c_str() ) );

		}
	}
	if ( num_completed > 0 )
	{
		GlobalEvents::GetGlobalEventSender( GlobalEvents::UnitSyncReloadRequest ).SendEvent( 0 );
		wxLogError( _("USYNC RELOAD"));
	}
}

std::map<wxString,TorrentInfos> TorrentWrapper::CollectGuiInfos()
{
    wxMutexLocker lock( m_info_map_mutex );
    std::map<wxString,TorrentInfos> ret;
    try
    {
        TorrentInfos globalinfos;
        libtorrent::session_status session_status = m_torr->status();
        globalinfos.downloadstatus = P2P::leeching;
        globalinfos.progress = 0.0f;
        globalinfos.downloaded = session_status.total_download;
        globalinfos.inspeed = session_status.download_rate;
        globalinfos.numcopies = 0.0f;
        globalinfos.filesize = 0;
        ret[wxString(_T("global"))] = globalinfos;

        TorrenthandleVector torrentList = m_torr->get_torrents();
        for ( TorrenthandleVector::const_iterator i = torrentList.begin(); i != torrentList.end(); ++i )
        {
            TorrentInfos CurrentTorrent;
            libtorrent::torrent_status torrent_status = i->status();
            CurrentTorrent.name = TowxString(i->name()).BeforeFirst(_T('|'));
            CurrentTorrent.progress = torrent_status.progress;
            CurrentTorrent.downloaded = torrent_status.total_payload_download;
            CurrentTorrent.inspeed = torrent_status.download_payload_rate;
            CurrentTorrent.numcopies = torrent_status.distributed_copies;
            CurrentTorrent.filesize = i->get_torrent_info().total_size();

			int eta_seconds = -1;
			if ( CurrentTorrent.progress > 0 && CurrentTorrent.inspeed > 0)
				eta_seconds = int (  (CurrentTorrent.filesize - CurrentTorrent.downloaded ) / CurrentTorrent.inspeed );

			CurrentTorrent.eta = eta_seconds;
            CurrentTorrent.downloadstatus = P2P::leeching;
//
            ret[CurrentTorrent.name] = CurrentTorrent;
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"), TowxString(e.what()).c_str());
        return ret;
    }

    // display infos about queued torrents

//    std::set<TorrentTable::PRow> queuedrequests = GetTorrentTable().QueuedTorrentsByRow();
//    for ( std::set<TorrentTable::PRow>::iterator it = queuedrequests.begin(); ( it != queuedrequests.end() ) && ( GetTorrentTable().GetOpenLeechsCount() < 4 ); it++ )
//    {
//        TorrentInfos QueuedTorrent;
//        QueuedTorrent.numcopies = -1;
//        QueuedTorrent.hash = (*it)->hash;
//        QueuedTorrent.downloadstatus = P2P::queued;
//        QueuedTorrent.name=(*it)->name;
//		QueuedTorrent.eta = -1;
//        ret[QueuedTorrent.hash] = QueuedTorrent;
//    }

    return ret;
}

void TorrentWrapper::RemoveUnneededTorrents()
{
    wxMutexLocker lock( m_info_map_mutex );
    TorrenthandleInfoMap::iterator it = m_handleInfo_map.begin();
    for ( ; it != m_handleInfo_map.end(); )
    {
        PlasmaResourceInfo info = it->first;
        libtorrent::torrent_handle handle = it->second;
        if ( !handle.is_valid() )
        {
            m_torr->remove_torrent( handle );
            m_handleInfo_map.erase( it++ );
        }
        else
            ++it;
    }
}

void TorrentWrapper::ClearFinishedTorrents()
{
    wxMutexLocker lock( m_info_map_mutex );
    TorrenthandleInfoMap::iterator it = m_handleInfo_map.begin();
    for ( ; it != m_handleInfo_map.end();  )
    {
        PlasmaResourceInfo info = it->first;
        libtorrent::torrent_handle handle = it->second;
        if ( !handle.is_valid() || handle.is_seed() )
        {
            m_torr->remove_torrent( handle );
            m_handleInfo_map.erase( it++ );
        }
        else
            ++it;
    }
}


void TorrentWrapper::TryToJoinQueuedTorrents()
{
    wxMutexLocker lock( m_info_map_mutex );
}

void TorrentWrapper::SearchAndGetQueuedDependencies()
{
//	std::set<TorrentTable::PRow> listcopy = GetTorrentTable().DependencyCheckQueuebyRow();
//	for ( std::set<TorrentTable::PRow>::iterator itor = listcopy.begin(); itor != listcopy.end(); itor++ )
//	{
//		TorrentTable::PRow row = *itor;
//		if ( row->type == IUnitSync::map )
//		{
//			if ( usync().MapExists( row->name, row->hash ) )
//			{
//				wxArrayString deps = usync().GetMapDeps( row->name );
//				int count = deps.GetCount();
//				for ( int i = 0; i < count; i++ )
//				{
//					RequestFileByName( deps[i] );
//				}
//				GetTorrentTable().RemoveRowFromDependencyCheckQueue( row );
//			}
//		}
//		else if ( row->type == IUnitSync::mod )
//		{
//			if ( usync().ModExists( row->name, row->hash ) )
//			{
//				wxArrayString deps = usync().GetModDeps( row->name );
//				int count = deps.GetCount();
//				for ( int i = 0; i < count; i++ )
//				{
//					RequestFileByName( deps[i] );
//				}
//				GetTorrentTable().RemoveRowFromDependencyCheckQueue( row );
//			}
//		}
//	}

}


#endif
