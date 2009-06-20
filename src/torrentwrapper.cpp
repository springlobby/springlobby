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
#include "utils.h"
#include "socket.h"
#include "base64.h"
#include "globalevents.h"

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
#include "settings++/custom_dialogs.h"
#include "globalsmanager.h"


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
		if ( m_parent.IsConnectedToP2PSystem() )
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

/** Get the wxFileName of a wxTorrentFile given its wxHash (where a
 * wxHash is a wxString).
 *
 * This is a wxConvienceFunction. wxEnjoy!
 */
inline wxFileName
torrentFileName(const wxString& hash)
{
    return sett().GetTorrentDir().GetPathWithSep() + MakeHashSigned(hash) + _T(".torrent");
}

bool TorrentTable::IsConsistent()
{
#ifdef TorrentTable_validate
    for (std::set<TorrentTable::PRow>::iterator i=all_torrents.begin();i!=all_torrents.end();++i)
    {
        if (hash_index.count((*i)->hash)==0)return false;
        if (name_index.count((*i)->name)==0)return false;
        // handle_index might not contain the torrent with invalid/null handle
    }
#endif
    return true;
}

void TorrentTable::FlushData()
{
#ifdef TorrentTable_validate
    all_torrents.clear();
#endif
    hash_index.clear();
    name_index.clear();
    handle_index.clear();
    seed_requests.clear();
    queued_torrents.clear();
    seed_sent_data.clear();
		dep_check_queue.clear();

    m_seed_count = 0;
    m_leech_count = 0;
}

void TorrentTable::InsertRow(TorrentTable::PRow row)
{
    if (!row.ok())return;

#ifdef TorrentTable_validate
    if (all_torrents.count(row))
    {
        wxLogWarning(_T("TorrentTable: inserting the row twice!"));
    }
    all_torrents.insert(row);
#endif
    int duplicates=0;
    if (hash_index.count(row->hash))
    {
        wxLogWarning(_T("TorrentTable: inserting row with duplicate hash!"));
        duplicates++;
    }
    if (name_index.count(row->name))
    {
        wxLogWarning(_T("TorrentTable: inserting row with duplicate name!"));
        duplicates++;
    }
    // enforce all duplicates or no duplicates for now.
    if (duplicates!=0 && duplicates!=2)
    {
        wxLogWarning(_T("TorrentTable: insert would cause inconsistency, not all keys are duplicated!"));
        wxLogWarning(_T("TorrentTable: insert not done!"));
        return;
    }

    // duplicate handles are not so bad, but a message may be useful
    if (handle_index.count(row->handle))
    {
        wxLogMessage(_T("TorrentTable: inserting row with duplicate handle."));
    }

    hash_index[row->hash]=row;
    name_index[row->name]=row;

    if (row->handle.is_valid())handle_index[row->handle]=row;
}

void TorrentTable::RemoveRow(TorrentTable::PRow row)
{
    if (!row.ok())return;
    hash_index.erase(row->hash);
    name_index.erase(row->name);
    handle_index.erase(row->handle);
    seed_requests.erase(row);
    queued_torrents.erase(row);
}

void TorrentTable::SetRowHandle(TorrentTable::PRow row, const libtorrent::torrent_handle &handle)
{
    handle_index.erase(row->handle);
    row->handle=handle;
    if (row->handle!=libtorrent::torrent_handle())handle_index[row->handle]=row;
}

void TorrentTable::AddRowToDependencyCheckQueue(PRow row)
{
	if (!row.ok())return;
	dep_check_queue.insert( row );
}

void TorrentTable::RemoveRowFromDependencyCheckQueue( PRow row )
{
	if (!row.ok())return;
	dep_check_queue.erase( row );
}

void TorrentTable::RemoveRowHandle( PRow row )
{
		if (!row.ok())return;
    handle_index.erase(row->handle);
    row->handle= libtorrent::torrent_handle();
}

void TorrentTable::SetRowStatus( TorrentTable::PRow row, P2P::FileStatus status )
{
		if (!row.ok())return;
		if ( row->status != P2P::seeding && status == P2P::seeding ) m_seed_count++;
		if ( row->status != P2P::leeching && status == P2P::leeching ) m_leech_count++;
		if ( row->status == P2P::seeding && status != P2P::seeding ) m_seed_count--;
		if ( row->status == P2P::leeching && status != P2P::leeching ) m_leech_count--;
    if ( row->status != P2P::queued && status == P2P::queued ) queued_torrents.insert( row );
		if ( row->status == P2P::queued && status != P2P::queued ) queued_torrents.erase( row );
    if ( row->status == P2P::seeding || row->status == P2P::leeching )
    {
        if ( status != P2P::seeding && status != P2P::leeching ) RemoveRowHandle( row );
    }
    row->status = status;
}

void TorrentTable::SetRowTransferredData( PRow row, TransferredData data )
{
	if (!row.ok())return;
	seed_sent_data[row] = data;
}

void TorrentTable::AddSeedRequest(TorrentTable::PRow row)
{
		if (!row.ok())return;
    seed_requests.insert(row);
}
void TorrentTable::RemoveSeedRequest(TorrentTable::PRow row)
{
		if (!row.ok())return;
    seed_requests.erase(row);
}

bool TorrentTable::IsSeedRequest(TorrentTable::PRow row)
{
		if (!row.ok()) return false;
    return seed_requests.count(row)>0;
}

TorrentTable::PRow TorrentTable::RowByHash(const wxString &hash)
{
    std::map<wxString,PRow>::iterator i=hash_index.find(hash);
    return i!=hash_index.end() ? i->second : PRow(NULL);
}

TorrentTable::PRow TorrentTable::RowByName(const wxString &name)
{
    std::map<wxString,PRow>::iterator i=name_index.find(name);
    return i!=name_index.end() ? i->second : PRow(NULL);
}

TorrentTable::PRow TorrentTable::RowByHandle(libtorrent::torrent_handle handle)
{
    std::map<libtorrent::torrent_handle,PRow>::iterator i=handle_index.find(handle);
    return i!=handle_index.end() ? i->second : PRow(NULL);
}

std::map<wxString, TorrentTable::PRow> TorrentTable::RowsByHash()
{
    return hash_index;
}


std::set<TorrentTable::PRow> TorrentTable::SeedRequestsByRow()
{
    return seed_requests;
}

std::map<libtorrent::torrent_handle, TorrentTable::PRow> TorrentTable::RowByTorrentHandles()
{
    return handle_index;
}

std::set<TorrentTable::PRow> TorrentTable::QueuedTorrentsByRow()
{
    return queued_torrents;
}

std::map<TorrentTable::PRow, TorrentTable::TransferredData> TorrentTable::TransferredDataByRow()
{
	return seed_sent_data;
}

std::set<TorrentTable::PRow> TorrentTable::DependencyCheckQueuebyRow()
{
	return dep_check_queue;
}

unsigned int TorrentTable::GetOpenSeedsCount()
{
	return m_seed_count;
}

unsigned int TorrentTable::GetOpenLeechsCount()
{
	return m_leech_count;
}


TorrentWrapper& torrent()
{
    static GlobalObjectHolder<TorrentWrapper> m_torr_wrap;
    return m_torr_wrap;
}


TorrentWrapper::TorrentWrapper():
        ingame(false),
        m_timer_count(0),
        m_maintenance_thread(this),
        m_is_connecting(false),
        m_started(false)
{
    wxLogMessage(_T("TorrentWrapper::TorrentWrapper()"));
    m_tracker_urls.Add( _T("tracker.caspring.org"));
    m_tracker_urls.Add( _T("tracker2.caspring.org"));
    m_tracker_urls.Add( _T("backup-tracker.licho.eu"));
    m_torr = new libtorrent::session();
    try
    {
        m_torr->add_extension(&libtorrent::create_metadata_plugin);
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    try
    {
        m_torr->add_extension(&libtorrent::create_ut_pex_plugin);
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    try
    {
        m_torr->start_upnp();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    try
    {
        m_torr->start_natpmp();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    try
    {
        m_torr->start_lsd();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    m_socket_class = new Socket( *this );
    UpdateSettings();
}


TorrentWrapper::~TorrentWrapper()
{
    wxLogMessage(_T("TorrentWrapper::~TorrentWrapper()"));
    m_maintenance_thread.Stop();
    try
    {
        m_torr->stop_upnp();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    try
    {
        m_torr->stop_natpmp();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    try
    {
        m_torr->stop_lsd();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
    }
    m_socket_class->SetTimeout( 1 );
    DisconnectFromP2PSystem();
    delete m_torr;
    delete m_socket_class;
}


////////////////////////////////////////////////////////
////                gui interface                   ////
////////////////////////////////////////////////////////

bool TorrentWrapper::ConnectToP2PSystem( const unsigned int tracker_no )
{
    if ( IsConnectedToP2PSystem() ) return true;
    if ( tracker_no >= m_tracker_urls.GetCount() )
    {
        m_is_connecting = false;
        m_connected_tracker_index = 0;
        customMessageBoxNoModal( SL_MAIN_ICON, _("Tried all known trackers for torrent system. No connection could be established"),
                                 _("Torrent system failure") );
        return false;
    }
    m_socket_class->Connect( m_tracker_urls[tracker_no], DEFAULT_P2P_COORDINATOR_PORT );
    m_connected_tracker_index= tracker_no;
    m_is_connecting = true;

    return IsConnectedToP2PSystem();
}


void TorrentWrapper::DisconnectFromP2PSystem()
{
    if ( IsConnectedToP2PSystem() )
        m_socket_class->Disconnect();
}


bool TorrentWrapper::IsConnectedToP2PSystem()
{
    if ( m_socket_class == 0 ) return false;
    return  (m_socket_class->State() == SS_Open);
}


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
        wxLogError( WX_STRINGC( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }
}


bool TorrentWrapper::IsFileInSystem( const wxString& hash )
{
    return GetTorrentTable().RowByHash(hash).ok();
}


bool TorrentWrapper::RemoveTorrentByHash( const wxString& hash )
{
    TorrentTable::PRow row=GetTorrentTable().RowByHash(hash);
    if (!row.ok())return false;
    return RemoveTorrentByRow( row );
}



int TorrentWrapper::GetTorrentSystemStatus()
{
    if (!IsConnectedToP2PSystem()) return 0;
    if (ingame) return 2;
    return 1;
}


/*
HashToTorrentData& TorrentWrapper::GetSystemFileList()
{
    return m_torrent_infos;
}
*/

////////////////////////////////////////////////////////
////               lobby interface                  ////
////////////////////////////////////////////////////////


TorrentWrapper::DownloadRequestStatus TorrentWrapper::RequestFileByHash( const wxString& hash )
{
    TorrentTable::PRow row=GetTorrentTable().RowByHash(hash);
    if ( !row.ok() ) return missing_in_table;
    return QueueFileByRow( row );
}


TorrentWrapper::DownloadRequestStatus TorrentWrapper::RequestFileByName( const wxString& name )
{
    TorrentTable::PRow row=GetTorrentTable().RowByName(name);
    if ( !row.ok() ) return missing_in_table;
    return QueueFileByRow( row );
}


void TorrentWrapper::SetIngameStatus( bool status )
{
    if ( status == ingame ) return; // no change needed
    ingame = status;
    if ( ingame ) m_maintenance_thread.Pause();
    else m_maintenance_thread.Resume();
    if ( !IsConnectedToP2PSystem() ) return;
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
        wxLogError( WX_STRINGC( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }
}


void TorrentWrapper::UpdateFromTimer( int mselapsed )
{
    m_timer_count++;
    if ( m_timer_count < 20 ) return;//update every 2 sec
    m_timer_count = 0;
    if ( m_is_connecting )
    {
        if ( IsConnectedToP2PSystem() )
            m_is_connecting = false;
        else
            ConnectToP2PSystem( m_connected_tracker_index +1 );
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
            if (scheduled_in_cue == RequestFileByHash( TorrentsToResume[i] ) ) // resume all open leeched files when system as disconnected last time
                successfulIndices.push_back(i);
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

TorrentWrapper::DownloadRequestStatus TorrentWrapper::QueueFileByRow( const TorrentTable::PRow& row )
{
    if (ingame) return paused_ingame;
    if ( !IsConnectedToP2PSystem()  ) return not_connected;

    if (!row.ok())return file_not_found;

    if (row->status==P2P::leeching||(row->status&P2P::stored) || (row->status == P2P::queued) ) return duplicate_request;

		GetTorrentTable().SetRowStatus( row, P2P::queued );
		return scheduled_in_cue;
}

TorrentWrapper::DownloadRequestStatus TorrentWrapper::RequestFileByRow( const TorrentTable::PRow& row )
{
    if (ingame) return paused_ingame;
    if ( !IsConnectedToP2PSystem()  ) return not_connected;

    if (!row.ok())return file_not_found;

    if (row->status==P2P::leeching||(row->status&P2P::stored) ) return duplicate_request;

    if ( GetTorrentTable().GetOpenLeechsCount() > 4 )
    {
    	 GetTorrentTable().SetRowStatus( row, P2P::queued );
    	 return scheduled_in_cue;
    }

    if ( !JoinTorrent( row, false ) )
    {
    	 GetTorrentTable().SetRowStatus( row, P2P::not_stored ); // remove from queue list or it will keep failing
    	 return torrent_join_failed;
    }
    return success;
}


bool TorrentWrapper::RemoveTorrentByRow( const TorrentTable::PRow& row )
{
    if (!row.ok())return false;
    wxLogDebugFunc( row->name );
    if ( row->status==P2P::queued )
    {
    	TorrentTable().SetRowStatus( row, P2P::not_stored );
    	return true;
    }
    try
    {
        bool filecompleted = row->handle.is_seed();
        m_torr->remove_torrent( row->handle );

        if ( filecompleted ) GetTorrentTable().SetRowStatus( row, P2P::stored ); // fix the file status and automatically remove row handle
        else
        {
        	 GetTorrentTable().SetRowStatus( row, P2P::not_stored );
        	 SendMessageToCoordinator( _T("N-|")  + MakeHashSigned(row->hash) + _T("\n") );  //notify the system we don't need the file anymore
        }
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) );
        return false;
    }
    return true;
}


std::map<int,TorrentInfos> TorrentWrapper::CollectGuiInfos()
{
    std::map<int,TorrentInfos> ret;
    try
    {
        TorrentInfos globalinfos;
        libtorrent::session_status s = m_torr->status();
        globalinfos.downloadstatus = P2P::leeching;
        globalinfos.progress = 0.0f;
        globalinfos.downloaded = s.total_download;
        globalinfos.uploaded = s.total_upload;
        globalinfos.outspeed = s.upload_rate;
        globalinfos.inspeed = s.download_rate;
        globalinfos.numcopies = 0.0f;
        globalinfos.filesize = 0;
        ret[0] = globalinfos;

        if ( ingame || !IsConnectedToP2PSystem()  ) return ret; // stop updating the gui if disconneted

        std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
        for ( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
        {
            TorrentInfos CurrentTorrent;
            libtorrent::torrent_status s = i->status();
            CurrentTorrent.name = WX_STRING(i->name()).BeforeFirst(_T('|'));
            CurrentTorrent.progress = s.progress;
            CurrentTorrent.downloaded = s.total_payload_download;
            CurrentTorrent.uploaded = s.total_payload_upload;
            CurrentTorrent.inspeed = s.download_payload_rate;
            CurrentTorrent.outspeed = s.upload_payload_rate;
            CurrentTorrent.numcopies = s.distributed_copies;
            CurrentTorrent.filesize = i->get_torrent_info().total_size();

            TorrentTable::PRow row=GetTorrentTable().RowByHandle(*i);
            if (!row.ok()) continue;
            CurrentTorrent.hash=row->hash;
            CurrentTorrent.downloadstatus = row->status;

            ret[s2l(CurrentTorrent.hash)] = CurrentTorrent;
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"), WX_STRINGC(e.what()).c_str());
    }

    // display infos about queued torrents

    std::set<TorrentTable::PRow> queuedrequests = GetTorrentTable().QueuedTorrentsByRow();
    for ( std::set<TorrentTable::PRow>::iterator it = queuedrequests.begin(); ( it != queuedrequests.end() ) && ( GetTorrentTable().GetOpenLeechsCount() < 4 ); it++ )
    {
        TorrentInfos QueuedTorrent;
        QueuedTorrent.numcopies = -1;
        QueuedTorrent.hash = (*it)->hash;
        QueuedTorrent.downloadstatus = P2P::queued;
        QueuedTorrent.name=(*it)->name;
        ret[s2l(QueuedTorrent.hash)] = QueuedTorrent;
    }

    return ret;
}


void TorrentWrapper::SendMessageToCoordinator( const wxString& message )
{
    if ( IsConnectedToP2PSystem()  )
    {
			 wxLogMessage( _T("T send: %s"), message.c_str() );
    	 m_socket_class->Send( message + _T("\n") );
    }
}


bool TorrentWrapper::JoinTorrent( const TorrentTable::PRow& row, bool IsSeed )
{
    if ( !row.ok() ) return false;
    wxLogMessage(_T("(1) Joining torrent, name=%s"),row->name.c_str());
    if (ingame) return false;

    wxLogMessage(_T("(2) Joining torrent. IsSeed: ") + TowxString(IsSeed) + _T(" status: ") + TowxString(row->status) );

    if ( IsSeed && ( row->status != P2P::stored ) ) return false;
    if ( !IsSeed && ( row->status != P2P::queued ) && ( row->status != P2P::not_stored ) ) return false;

    wxString torrent_name=row->name;
    wxString torrent_infohash_b64=row->infohash;
    wxFileName path;

    switch (row->type)
    {
    case IUnitSync::map:
    {
        torrent_name = torrent_name + _T("|MAP");
        break;
    }
    case IUnitSync::mod:
    {
        torrent_name = torrent_name + _T("|MOD");
        break;
    }
    }

    if ( IsSeed )
    {
        wxString archivename;
        switch ( row->type ) // if file is not present locally you can't seed it
        {
        case IUnitSync::map:
        {
            if ( !usync().MapExists( row->name, row->hash ) ) return false;
            int index = usync().GetMapIndex( row->name );
            if ( index == -1 ) return false;
            archivename = usync().GetMapArchive( index );
            break;
        }
        case IUnitSync::mod:
        {
            if ( !usync().ModExists( row->name, row->hash ) ) return false;
            int index = usync().GetModIndex( row->name );
            if ( index == -1 ) return false;
            archivename = usync().GetModArchive( index );
            break;
        }
        }

        try
        {
            // dizekat> i'm getting archivename == /home/dmytry/.spring/maps/Whatever.sdf and getting archivepath == /home/dmytry/.spring/maps/
            // dizekat> so i changed it to prepend path only if path isnt found here.
            wxLogMessage( _T("seeding from archive name: %s"), archivename.c_str() );
            wxFileName archivepath(usync().GetArchivePath( archivename ));
            int i = archivename.Find( archivepath.GetFullPath() );
            if (i<0)
            {
                path = archivepath.GetPathWithSep() + archivename;
            }
            else
            {
                path = wxFileName(archivename);
            }

        }
        catch (std::exception& e)
        {
            wxLogError( WX_STRINGC( e.what() ) );
            wxLogMessage( _T("Local filepath couldn't be determined") );
            return false;
        }
        wxLogMessage( _T("seeding from local filename: %s"), path.GetFullPath().c_str() );
    }
    else			/* if(IsSeed) */
    {
        path = sett().GetTorrentDataDir();
        path.AppendDir( getDataSubdirForType(row->type) );
        if ( !path.DirExists() ) path.Mkdir(0755);
            wxLogMessage(_T("downloading to path: =%s"), path.GetFullPath().c_str());
    }


    wxLogMessage(_T("(3) Joining torrent: downloading info file"));
    if (!DownloadTorrentFileFromTracker( MakeHashSigned( row->hash ) ))
    {
    	 wxLogError(_T("(3) info file download failed"));
    	 return false;
    }

		#if LIBTORRENT_VERSION_MINOR < 14
			// read torrent from file
			std::ifstream in( torrentFileName(row->hash).GetFullPath().mb_str(), std::ios_base::binary);
			in.unsetf(std::ios_base::skipws);
			libtorrent::entry e;
			try
			{
					// decode the torrent infos from the file
					e = libtorrent::bdecode(std::istream_iterator<char>(in), std::istream_iterator<char>());

			}
			catch ( std::exception& exc )
			{
					wxLogMessage( _T("torrent has invalid encoding") );
					return false;
			}

			libtorrent::torrent_info t_info (e);

			if ( t_info.num_files() != 1 )
			{
					wxLogMessage( _T("torrent contains an invalid number of files") );
					return false;
			}

			wxString torrentfilename = WX_STRING( t_info.begin_files()->path.string() ); // get the file name in the torrent infos

    #else
			libtorrent::add_torrent_params p;

			try
			{
					// the torrent_info is stored in an intrusive_ptr
					p.ti = new libtorrent::torrent_info(boost::filesystem::path(torrentFileName(row->hash).GetFullPath().mb_str()));

			}
			catch ( std::exception& exc )
			{
					wxLogMessage( _T("torrent has invalid encoding") );
					return false;
			}
			//decode success

			boost::intrusive_ptr<libtorrent::torrent_info> t_info = p.ti;

			if ( t_info->num_files() != 1 )
			{
					wxLogMessage( _T("torrent contains an invalid number of files") );
					return false;
			}

			wxString torrentfilename = WX_STRING( t_info->file_at(0).path.string() ); // get the file name in the torrent infos
    #endif


    wxLogMessage( _T("requested filename: %s"), torrentfilename.c_str() );

    wxFileName archive_filename(path);
    wxFileName torrent_filename(torrentfilename);

    if ( IsSeed && ( torrent_filename.GetFullName() != archive_filename.GetFullName() ) )
    {
        wxLogMessage(_T("filename differs from torrent, renaming file in torrent info"));
        if ( !( torrent_filename.GetExt() == archive_filename.GetExt() ) ) // different extension, won't work
        {
           wxLogMessage( _T("file extension locally differs, not joining torrent") );
           return false;
        }
        wxLogMessage(_T("New filename in torrent: %s"), archive_filename.GetFullName().c_str());
        #if LIBTORRENT_VERSION_MINOR < 14
					std::vector<libtorrent::file_entry> map;
					libtorrent::file_entry foo = t_info.file_at(0);
					map.push_back( foo );
					map.front().path = boost::filesystem::path(STD_STRING( archive_filename.GetFullName() ) );
					wxLogMessage(_T("New filename in torrent: %s"), archive_filename.GetFullName().c_str() );
					if ( !t_info.remap_files(map) )
					{
					 wxLogMessage(_T("Cannot remap filenames in the torrent, aborting seed"));
					 return false;
					}
				#else
					t_info->files().rename_file(0, std::string(archive_filename.GetFullName().mb_str()));
				#endif
    }
    wxLogMessage(_T("(4) Joining torrent: add_torrent(%s,[%s],%s,[%s])"),m_tracker_urls[m_connected_tracker_index].c_str(),torrent_infohash_b64.c_str(),row->name.c_str(),path.GetFullPath().c_str());

    try
    {
				#if LIBTORRENT_VERSION_MINOR < 14
					GetTorrentTable().SetRowHandle(row, m_torr->add_torrent( t_info, boost::filesystem::path(path.GetFullPath().mb_str())));
				#else
					p.save_path = path.GetFullPath().mb_str();
					GetTorrentTable().SetRowHandle(row, m_torr->add_torrent(p));
				#endif

    }
    catch (std::exception& e)
    {
       wxLogError(_T("%s"),WX_STRINGC( e.what()).c_str()); // TODO (BrainDamage#1#): add message to user on failure
       return false;
    }
     try
    {
        if (IsSeed)
        {
            if (row->handle.is_valid())
            {
								#if LIBTORRENT_VERSION_MINOR < 14
									std::vector<bool> tmp(1,true);
									row->handle.filter_files(tmp);
								#else
									// there's only one file in the torrent, set its priority to 0
									row->handle.file_priority(0, 0);
                #endif
            }
            else
            {
                wxLogMessage(_T("Cant set seed not to download"));
            }
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"),WX_STRINGC( e.what()).c_str()); // TODO (BrainDamage#1#): add message to user on failure
    }

    if ( IsSeed ) GetTorrentTable().SetRowStatus( row, P2P::seeding );
    else
    {
        GetTorrentTable().SetRowStatus( row, P2P::leeching );
				SendMessageToCoordinator( wxString::Format( _T("N+|%s\n"), MakeHashSigned(row->hash).c_str() ) ); // request for seeders for the file
    }

    wxLogMessage(_T("(5) Joining torrent: done"));
    return true;
}


void TorrentWrapper::CreateTorrent( const wxString& hash, const wxString& name, IUnitSync::MediaType type )
{
    if (ingame) return;


    if ( sett().GetCurrentUsedDataDir().IsEmpty() ) return; // no good things can happend if you don't know which folder to r/w files from

	# if LIBTORRENT_VERSION_MINOR >= 14

    libtorrent::file_storage files;

    wxString archivename;

    switch ( type )
    {
    case IUnitSync::map :
    {
        if ( !usync().MapExists( name, hash ) ) return;
        int index = usync().GetMapIndex( name );
        if ( index == -1 ) return;
        archivename = usync().GetMapArchive( index );
        break;
    }
    case IUnitSync::mod :
    {
        if ( !usync().ModExists( name, hash ) ) return;
        int index = usync().GetModIndex( name );
        if ( index == -1 ) return;
        archivename = usync().GetModArchive( index );
        break;
    }
    }

    wxString archivepath = usync().GetArchivePath( archivename );
    int i = archivename.Find( archivepath );
    wxString path;
    if (i<0)
    {
        path = archivepath + archivename;
    }
    else
    {
        path = archivename;
    }

    boost::filesystem::path InputFilePath = complete(boost::filesystem::path( STD_STRING( path ) ) );

    files.add_file( InputFilePath.branch_path(), boost::filesystem::file_size( InputFilePath ) );

    libtorrent::create_torrent newtorrent(files);

    for ( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
    {
        newtorrent.add_tracker( STD_STRING(m_tracker_urls[i] +  _T(":DEFAULT_P2P_TRACKER_PORT/announce") ) );
    }

    // calculate the hash for all pieces
    set_piece_hashes(newtorrent, InputFilePath.branch_path());

    switch (type)
    {
    case IUnitSync::map:
        newtorrent.set_comment( wxString( name + _T("|MAP") ).mb_str() );
    case IUnitSync::mod:
        newtorrent.set_comment( wxString( name + _T("|MOD") ).mb_str() );
    }

    libtorrent::entry e = newtorrent.generate();
    // TODO: e needs to be encoded and saved to a .torrent file
    // or added to m_torr

	#endif
}


bool TorrentWrapper::DownloadTorrentFileFromTracker( const wxString& hash )
{
    if ( sett().GetCurrentUsedDataDir().IsEmpty() ) return false; // no good things can happend if you don't know which folder to r/w files from

    wxFileName filename( torrentFileName(hash) );

    bool readable(filename.IsFileReadable());

    if ( readable  ) return true; //file already present locally

    wxLogMessage(_T("torrent system downloading torrent info %s"), hash.c_str() );

    wxHTTP fileRequest;
    //versionRequest.SetHeader(_T("Content-type"), _T(""));
    // normal timeout is 10 minutes.. set to 2 secs.
    fileRequest.SetTimeout(2);
    fileRequest.Connect( m_tracker_urls[m_connected_tracker_index], 80);
    wxInputStream *stream = fileRequest.GetInputStream(  _T("/torrents/") + hash + _T(".torrent") );
    bool ret = false;
    if (fileRequest.GetError() == wxPROTO_NOERR)
    {

        wxFileOutputStream output( filename.GetFullPath() );
        if ( output.Ok() )
        {
            stream->Read(output);
            ret = true;
        }

    }

    wxDELETE(stream);
    fileRequest.Close();
    if (ret) wxLogMessage(_T("torrent system downloading torrent info %s successful"), hash.c_str() );
    else wxLogMessage(_T("torrent system downloading torrent info %s failed"), hash.c_str() );
    return ret;
}


void TorrentWrapper::JoinRequestedTorrents()
{
    std::set<TorrentTable::PRow> seedrequests= GetTorrentTable().SeedRequestsByRow();
    for (std::set<TorrentTable::PRow>::iterator  it = seedrequests.begin(); it != seedrequests.end(); ++it)
    {
        if (!it->ok())continue;

        if ( GetTorrentTable().GetOpenSeedsCount() > 9 ) break; // too many seeds open

        if ( (*it)->status != P2P::stored ) continue; // torrent must be present locally and not seeding/leeching

        if ( !JoinTorrent( *it, true ) ) GetTorrentTable().RemoveSeedRequest( *it );

    }

}

void TorrentWrapper::RemoveUnneededTorrents()
{
    std::map<libtorrent::torrent_handle, TorrentTable::PRow> torrenthandles = GetTorrentTable().RowByTorrentHandles();
    std::map<TorrentTable::PRow, TorrentTable::TransferredData> transfer_data_map = GetTorrentTable().TransferredDataByRow();
    for (std::map<libtorrent::torrent_handle, TorrentTable::PRow>::iterator  it = torrenthandles.begin(); it != torrenthandles.end(); ++it)
    {
				if ( !it->first.is_valid() ) continue;
        if ( !it->first.is_seed() ) continue;

				// save how much the torrent has seeded
				unsigned int payload_upload = it->first.status().total_payload_upload;

				TorrentTable::TransferredData old_data = transfer_data_map[it->second];
				if ( payload_upload == old_data.sentdata )
				{
					old_data.failed_check_counts = old_data.failed_check_counts + 1;
				}
				else old_data.sentdata = payload_upload;
				GetTorrentTable().SetRowTransferredData( it->second, old_data );
				// if the torrent didn't seed any data in the last 2 minutes, remove it from the request list
				if ( old_data.failed_check_counts > 60 ) GetTorrentTable().RemoveSeedRequest( it->second );

        if ( it->second->status == P2P::leeching ) // if torrent was opened in leech mode but now it's seeding it means it was requested from the user but now it's completed
        {
            //torrent has finished download, refresh unitsync and remove file from list
            try
            {
                /* Grab the source (temporary) and destination (final) filenames BEFORE we remove the download. */
                wxString sourceName( TowxString( ( it->first.save_path() / it->first.name() ).file_string() ) );
                wxString basepath = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + getDataSubdirForType(it->second->type);
                wxString targetName = basepath + wxFileName::GetPathSeparator() + TowxString(it->first.name());

                if ( !wxFileName::DirExists( basepath ) ) wxFileName::Mkdir( basepath, 0755 );
                ASSERT_EXCEPTION( RemoveTorrentByRow( it->second ), _T("failed to remove torrent: ")+ it->second->hash );

                /* Move file from temporary download directory to final destination.  `false' for
                 * parameter 3 means don't overwrite the file if it already exists.
                 */
                if ( ! wxRenameFile(sourceName, targetName, false) ) wxLogError(wxString::Format(_T("torrent: Failed to move \"%s\" to \"%s\""), sourceName.c_str(), targetName.c_str()));
                else wxLogMessage(wxString::Format(_T("torrent: Moved \"%s\" to \"%s\""), sourceName.c_str(), targetName.c_str()));

                GetTorrentTable().AddRowToDependencyCheckQueue( it->second );

                wxCommandEvent refreshevt(UnitSyncReloadRequest); // request an unitsync reload
                wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), refreshevt );
            }
            catch (std::exception& e)
            {
                wxLogError( WX_STRINGC( e.what() ) );
            }
        }

        if ( !GetTorrentTable().IsSeedRequest( it->second ) )// if torrent not in request list but still seeding then remove
        {
            try
            {
                ASSERT_EXCEPTION( RemoveTorrentByRow( it->second ), _T("failed to remove torrent: ")+ it->second->hash );
            }
            catch (std::exception& e)
            {
                wxLogError( WX_STRINGC( e.what() ) );
            }
        }
    }

}


void TorrentWrapper::TryToJoinQueuedTorrents()
{

    if ( GetTorrentTable().GetOpenLeechsCount() < 5 )
    {
        // join queued files if there are available slots
        std::set<TorrentTable::PRow> queuedrequests = GetTorrentTable().QueuedTorrentsByRow();
        for ( std::set<TorrentTable::PRow>::iterator it = queuedrequests.begin(); ( it != queuedrequests.end() ) && ( GetTorrentTable().GetOpenLeechsCount() < 4 ); it++ )
        {
            if ( !it->ok() ) continue;
            RequestFileByRow( *it );
        }
    }

}

void TorrentWrapper::SearchAndGetQueuedDependencies()
{
	std::set<TorrentTable::PRow> listcopy = GetTorrentTable().DependencyCheckQueuebyRow();
	for ( std::set<TorrentTable::PRow>::iterator itor = listcopy.begin(); itor != listcopy.end(); itor++ )
	{
		TorrentTable::PRow row = *itor;
		if ( row->type == IUnitSync::map )
		{
			if ( usync().MapExists( row->name, row->hash ) )
			{
				wxArrayString deps = usync().GetMapDeps( row->name );
				int count = deps.GetCount();
				for ( int i = 0; i < count; i++ )
				{
					RequestFileByName( deps[i] );
				}
				GetTorrentTable().RemoveRowFromDependencyCheckQueue( row );
			}
		}
		else if ( row->type == IUnitSync::mod )
		{
			if ( usync().ModExists( row->name, row->hash ) )
			{
				wxArrayString deps = usync().GetModDeps( row->name );
				int count = deps.GetCount();
				for ( int i = 0; i < count; i++ )
				{
					RequestFileByName( deps[i] );
				}
				GetTorrentTable().RemoveRowFromDependencyCheckQueue( row );
			}
		}
	}

}

void TorrentWrapper::ReceiveandExecute( const wxString& msg )
{
    wxLogMessage(_T("T recive: %s"), msg.c_str() );

    wxArrayString data = wxStringTokenize( msg, _T('|') );

    if ( data.GetCount() == 0 ) return;
    // T+|hash|name|type 	 informs client that new torrent was added to server (type is either MOD or MAP)
    else if ( data.GetCount() > 3 && data[0] == _T("T+") )
    {

        TorrentTable::PRow newtorrent = new TorrentTable::Row;

        newtorrent->hash = MakeHashUnsigned(data[1]);
        newtorrent->name = data[2];
        if ( data[3] == _T("MAP") )
        {
            newtorrent->type = IUnitSync::map;
            if ( usync().MapExists( newtorrent->name, newtorrent->hash ) ) newtorrent->status = P2P::stored;
        }
        else if ( data[3] == _T("MOD") )
        {
            newtorrent->type = IUnitSync::mod;
            if ( usync().ModExists( newtorrent->name, newtorrent->hash ) ) newtorrent->status = P2P::stored;
        }

        GetTorrentTable().InsertRow( newtorrent );

        // SendMessageToCoordinator(  _T("IH|") + data[1] + _T("\n") );

        // T-|hash 	 informs client that torrent was removed from server
    }
    else if ( data[0] == _T("T-") && data.GetCount() > 1 )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( MakeHashUnsigned(data[1]) );
        if ( !row.ok() ) return;
        GetTorrentTable().RemoveRow( row );

        // S+|hash|seeders|leechers 	 tells client that seed is needed for this torrent
    }
    else if ( data.GetCount() > 1 && data[0] == _T("S+") )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( MakeHashUnsigned(data[1]) );
        if ( !row.ok() ) return;
        GetTorrentTable().AddSeedRequest( row );

        unsigned long seeders=0;
        unsigned long leechers=0;
        if (data.GetCount() > 2)data[2].ToULong(&seeders);
        if (data.GetCount() > 3)data[3].ToULong(&leechers);

        // S-|hash 	 tells client that seed is no longer neede for this torrent
    }
    else if ( data.GetCount() > 1 && data[0] == _T("S-") )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( MakeHashUnsigned(data[1]) );
        if ( !row.ok() ) return;
        GetTorrentTable().RemoveSeedRequest( row );

        // M+|hash|url 	 It tells the client if url is given that http mirror exists for given hash, else there are no mirrors.
    }
    else if ( data[0] == _T("M+") && data.GetCount() > 2 )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( MakeHashUnsigned(data[1]) );
        if ( !row.ok() ) return;

        for ( unsigned int index = 2; index < data.GetCount(); index++ )
        {
						try
						{
							row->handle.add_url_seed( STD_STRING( data[index] ) );
						} catch ( std::exception& e )
						{
						}
        }

        // PING 	 every minute - client must respond with its own "PING"
    }
    else if ( data[0] == _T("PING") )
    {

        SendMessageToCoordinator( _T("PING\n") );

        //IH|hash|infohash infos the client about torrent's infohash b64 encoded
    }
    else if ( data.GetCount() > 2 && data[0] == _T("IH") )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( MakeHashUnsigned(data[1]) );
        if ( !row.ok() ) return;

        row->infohash = data[2];
				// QH|query tag|type|name 	 queries clients for spring hashes of given map/mod
    }
    else if ( data[0] == _T("QH") && data.GetCount() > 3  )
    {
				wxString query_tag = data[1];
				wxString file_type = data[2];
				wxString unitsync_name = data[3];
				wxString hash;
				if ( file_type == _T("MAP") )
				{
					hash = usync().GetMap( unitsync_name ).hash;
				}
				else if ( file_type == _T("MOD") )
				{
					hash = usync().GetMod( unitsync_name ).hash;
				}
				if ( !hash.IsEmpty() )
				{
					hash = MakeHashSigned( hash );
					SendMessageToCoordinator( _T("QH|") + query_tag + _T("|") + hash );
				}
    }
    else if ( data[0] == _T("TLISTDONE") )
    {
        ResumeFromList(); // resume download of files
    }
}


void TorrentWrapper::OnConnected( Socket* sock )
{
    wxLogMessage(_T("torrent system connected") );
    m_started = true;

    try
    {
        m_torr->start_dht();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }

    GetTorrentTable().FlushData(); // flush the torrent data
		m_maintenance_thread.Init();

}


void TorrentWrapper::OnDisconnected( Socket* sock )
{
    wxLogMessage(_T("torrent system disconnected") );

    std::set<TorrentTable::PRow> queued =  GetTorrentTable().QueuedTorrentsByRow();
    wxArrayString TorrentsToResume;
    for ( std::set<TorrentTable::PRow>::iterator it = queued.begin(); it != queued.end(); it++ ) TorrentsToResume.Add( (*it)->hash );

    std::map<libtorrent::torrent_handle, TorrentTable::PRow> handles =  GetTorrentTable().RowByTorrentHandles();
		for ( std::map<libtorrent::torrent_handle, TorrentTable::PRow>::iterator it = handles.begin(); it != handles.end(); it++ )
    {
        if ( !it->first.is_seed() ) TorrentsToResume.Add( it->second->hash ); // save leeching torrents for resume on next connection

        try
        {
            m_torr->remove_torrent(it->first); //remove all torrents upon disconnect
        }
        catch (std::exception& e)
        {
            wxLogError( WX_STRINGC( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
        }
    }

    try
    {
        if (m_started) m_torr->stop_dht();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) ); // TODO (BrainDamage#1#): add message to user on failure
    }


    GetTorrentTable().FlushData(); // flush the torrent data

    try
    {
        sett().SetTorrentListToResume( TorrentsToResume );
    }
    catch (GlobalDestroyedError)
    {
    }
    m_started = false;
    m_maintenance_thread.Stop();
}


void TorrentWrapper::OnDataReceived( Socket* sock )
{
		if ( sock == 0 ) return;

    wxString data = sock->Receive();
		m_buffer << data;
		int returnpos = m_buffer.Find( _T("\n") );
		while ( returnpos != -1 )
		{
			wxString cmd = m_buffer.Left( returnpos );
			m_buffer = m_buffer.Mid( returnpos + 1 );
			ReceiveandExecute( cmd );
			returnpos = m_buffer.Find( _T("\n") );
		}
}

#endif
