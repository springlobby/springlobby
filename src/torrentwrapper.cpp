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

bool TorrentTable::IsConsistent()
{
#ifdef TorrentTable_validate
#endif
    return true;
}

void TorrentTable::FlushData()
{
#ifdef TorrentTable_validate

#endif
    m_leech_count = 0;
}

//
//void TorrentTable::SetRowStatus( TorrentTable::PRow row, P2P::FileStatus status )
//{
//		if (!row.ok())return;
//		if ( row->status != P2P::seeding && status == P2P::seeding ) m_seed_count++;
//		if ( row->status != P2P::leeching && status == P2P::leeching ) m_leech_count++;
//		if ( row->status == P2P::seeding && status != P2P::seeding ) m_seed_count--;
//		if ( row->status == P2P::leeching && status != P2P::leeching ) m_leech_count--;
//    if ( row->status != P2P::queued && status == P2P::queued ) queued_torrents.insert( row );
//		if ( row->status == P2P::queued && status != P2P::queued ) queued_torrents.erase( row );
//    if ( row->status == P2P::seeding || row->status == P2P::leeching )
//    {
//        if ( status != P2P::seeding && status != P2P::leeching ) RemoveRowHandle( row );
//    }
//    row->status = status;
//}

//void TorrentTable::SetRowTransferredData( PRow row, TransferredData data )
//{
//	if (!row.ok())return;
//	seed_sent_data[row] = data;
//}


unsigned int TorrentTable::GetOpenLeechsCount()
{
	return m_leech_count;
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
    m_tracker_urls.Add( _T("tracker.caspring.org"));
    m_tracker_urls.Add( _T("tracker2.caspring.org"));
    m_tracker_urls.Add( _T("backup-tracker.licho.eu"));
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
    int uploadLimit, downloadLimit;

    try
    {
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


bool TorrentWrapper::IsFileInSystem( const wxString& hash )
{
    return false;//GetTorrentTable().RowByHash(hash).ok();
}


bool TorrentWrapper::RemoveTorrentByHash( const wxString& hash )
{
    return false;
}


P2P::FileStatus TorrentWrapper::GetTorrentStatusByHash(const wxString &hash)
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

TorrentWrapper::DownloadRequestStatus TorrentWrapper::RequestFileByName( const wxString& name )
{
    return missing_in_table;
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
        std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
        if ( ingame ) // going ingame, pause all torrents (or throttle speeds) and disable dht
        {
            if ( sett().GetTorrentSystemSuspendMode() == 0 ) for ( unsigned int i = 0; i < TorrentList.size(); i++) TorrentList[i].pause();
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
            if ( sett().GetTorrentSystemSuspendMode() == 0 ) for ( unsigned int i = 0; i < TorrentList.size(); i++) TorrentList[i].resume();
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
    wxArrayString TorrentsToResume = sett().GetTorrentListToResume();
    unsigned int ResumeCount = TorrentsToResume.GetCount();
    if ( ResumeCount > 0 )
    {
        //request all hashes in list, remember successes
        std::vector<int> successfulIndices;
        for ( unsigned int i = 0; i < ResumeCount; i++ )
        {
//            if (scheduled_in_cue == RequestFileByHash( TorrentsToResume[i] ) ) // resume all open leeched files when system as disconnected last time
//                successfulIndices.push_back(i);
            assert( false );
        }

        //remove successfully resumed torrents from list
        std::vector<int>::const_iterator it = successfulIndices.begin();
        for ( ; it != successfulIndices.end(); ++it )
            TorrentsToResume.RemoveAt( *it );
        //save new list (hopefully empty)
        sett().SetTorrentListToResume( TorrentsToResume );
    }
}

////////////////////////////////////////////////////////
//// private functions to interface with the system ////
////////////////////////////////////////////////////////
void TorrentWrapper::JoinRequestedTorrents()
{

}

std::map<wxString,TorrentInfos> TorrentWrapper::CollectGuiInfos()
{
    std::map<wxString,TorrentInfos> ret;
    try
    {
        TorrentInfos globalinfos;
        libtorrent::session_status session_status = m_torr->status();
        globalinfos.downloadstatus = P2P::leeching;
        globalinfos.progress = 0.0f;
        globalinfos.downloaded = session_status.total_download;
        globalinfos.uploaded = session_status.total_upload;
        globalinfos.outspeed = session_status.upload_rate;
        globalinfos.inspeed = session_status.download_rate;
        globalinfos.numcopies = 0.0f;
        globalinfos.filesize = 0;
        ret[wxString(_T("global"))] = globalinfos;

        std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
        for ( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
        {
            TorrentInfos CurrentTorrent;
            libtorrent::torrent_status torrent_status = i->status();
            CurrentTorrent.name = TowxString(i->name()).BeforeFirst(_T('|'));
            CurrentTorrent.progress = torrent_status.progress;
            CurrentTorrent.downloaded = torrent_status.total_payload_download;
            CurrentTorrent.uploaded = torrent_status.total_payload_upload;
            CurrentTorrent.inspeed = torrent_status.download_payload_rate;
            CurrentTorrent.outspeed = torrent_status.upload_payload_rate;
            CurrentTorrent.numcopies = torrent_status.distributed_copies;
            CurrentTorrent.filesize = i->get_torrent_info().total_size();

			int eta_seconds = -1;
			if ( CurrentTorrent.progress > 0 && CurrentTorrent.inspeed > 0)
				eta_seconds = int (  (CurrentTorrent.filesize - CurrentTorrent.downloaded ) / CurrentTorrent.inspeed );

			CurrentTorrent.eta = eta_seconds;

////            TorrentTable::PRow row=GetTorrentTable().RowByHandle(*i);
//            if (!row.ok()) continue;
//            CurrentTorrent.hash=row->hash;
//            CurrentTorrent.downloadstatus = row->status;
//
//            ret[CurrentTorrent.hash] = CurrentTorrent;
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"), TowxString(e.what()).c_str());
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


bool TorrentWrapper::GetTorrentFileAndInfos( const wxString& resourceName )
{
    if ( sett().GetCurrentUsedDataDir().IsEmpty() ) return false; // no good things can happend if you don't know which folder to r/w files from

    return true;
}

void TorrentWrapper::RemoveUnneededTorrents()
{


}


void TorrentWrapper::TryToJoinQueuedTorrents()
{

    if ( GetTorrentTable().GetOpenLeechsCount() < 5 )
    {
        // join queued files if there are available slots
//        std::set<TorrentTable::PRow> queuedrequests = GetTorrentTable().QueuedTorrentsByRow();
//        for ( std::set<TorrentTable::PRow>::iterator it = queuedrequests.begin(); ( it != queuedrequests.end() ) && ( GetTorrentTable().GetOpenLeechsCount() < 4 ); it++ )
//        {
//            if ( !it->ok() ) continue;
//            RequestFileByRow( *it );
//        }
    }

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

void TorrentWrapper::OnConnected( Socket* /*unused*/ )
{
    wxLogMessage(_T("torrent system connected") );
    m_started = true;

    try
    {
        m_torr->start_dht();
    }
    catch (std::exception& e)
    {
        wxLogError( TowxString( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }

    GetTorrentTable().FlushData(); // flush the torrent data
		m_maintenance_thread.Init();

}


void TorrentWrapper::OnDisconnected( Socket* /*unused*/ )
{
    wxLogMessage(_T("torrent system disconnected") );

//    std::set<TorrentTable::PRow> queued =  GetTorrentTable().QueuedTorrentsByRow();
//    wxArrayString TorrentsToResume;
//    for ( std::set<TorrentTable::PRow>::iterator it = queued.begin(); it != queued.end(); it++ ) TorrentsToResume.Add( (*it)->hash );
//
//    std::map<libtorrent::torrent_handle, TorrentTable::PRow> handles =  GetTorrentTable().RowByTorrentHandles();
//		for ( std::map<libtorrent::torrent_handle, TorrentTable::PRow>::iterator it = handles.begin(); it != handles.end(); it++ )
//    {
//        if ( !it->first.is_seed() ) TorrentsToResume.Add( it->second->hash ); // save leeching torrents for resume on next connection
//
//        try
//        {
//            m_torr->remove_torrent(it->first); //remove all torrents upon disconnect
//        }
//        catch (std::exception& e)
//        {
//            wxLogError( TowxString( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
//        }
//    }
//
//    try
//    {
//        if (m_started) m_torr->stop_dht();
//    }
//    catch (std::exception& e)
//    {
//        wxLogError( TowxString( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
//    }
//
//
//    GetTorrentTable().FlushData(); // flush the torrent data
//
//    try
//    {
//        sett().SetTorrentListToResume( TorrentsToResume );
//    }
//    catch (GlobalDestroyedError)
//    {
//    }
    m_started = false;
    m_maintenance_thread.Stop();
}

#endif
