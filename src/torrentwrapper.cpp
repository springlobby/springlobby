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
#include "utils/uievents.h"
#include "globalsmanager.h"
#include "iunitsync.h"
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

//! translate handle status to P2Pstatus
P2P::FileStatus TorrentWrapper::getP2PStatusFromHandle( const libtorrent::torrent_handle& handle ) const
{
	if ( !handle.is_valid() )
		return P2P::not_stored;
	if ( handle.is_seed() )
		return P2P::complete;
	if ( handle.is_paused() )
		return P2P::paused;
	if ( handle.queue_position() > m_torr->settings().active_downloads )
		return P2P::queued;
	return P2P::leeching;
}

TorrentMaintenanceThread::TorrentMaintenanceThread( TorrentWrapper* parent )
	: m_stop_thread( false ),
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
	m_parent.ResumeFromList();
	while ( !TestDestroy() )
	{
		if( !Sleep( 2000 ) ) break;
		{
			//just to be on the safe sade in case anything slipped thru
			try {
				//  DON'T alter function call order here or bad things may happend like locust, earthquakes or raptor attack
				m_parent.RemoveInvalidTorrents();
				m_parent.HandleCompleted();
			}
			catch ( ... )
			{}
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
	m_info_download_thread.Create();
	m_info_download_thread.SetPriority( WXTHREAD_MIN_PRIORITY );
	m_info_download_thread.Run();
}


TorrentWrapper::~TorrentWrapper()
{
    wxLogDebugFunc( wxEmptyString );
    m_maintenance_thread.Stop();
	m_info_download_thread.Wait();
	m_torr->pause();
	ClearFinishedTorrents();
	RemoveInvalidTorrents();
	/* the use of Settings seems to a problem (destruction order) therefore disabled until further notice
	//save torrents to resume
	std::vector<wxString> toResume;
	TorrenthandleInfoMap infomap = GetHandleInfoMap();
	TorrenthandleInfoMap::iterator it = infomap.begin();
	for ( ; it != infomap.end(); ++it )
	{
		PlasmaResourceInfo info = it->first;
		toResume.push_back( info.m_name );
	}
	std::vector<wxString> currentResumes = sett().GetTorrentListToResume();
	for ( std::vector<wxString>::const_iterator it = currentResumes.begin();
		it != currentResumes.end(); ++it )
	{
		toResume.push_back( *it );
	}
	//save new list
	sett().SetTorrentListToResume( toResume );
	*/

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
	const TorrenthandleInfoMap& infomap = GetHandleInfoMap();
	TorrenthandleInfoMap::const_iterator it = infomap.begin();
	for ( ; it != infomap.end(); ++it )
	{
		PlasmaResourceInfo info = it->first;
		if ( info.m_name == name )
		{
			return true;
		}
	}
	return false;
}

bool TorrentWrapper::RemoveTorrentByName( const wxString& name )
{
	TorrenthandleInfoMap& infomap = GetHandleInfoMap();
	TorrenthandleInfoMap::iterator it = infomap.begin();
	for ( ; it != infomap.end(); ++it )
    {
        PlasmaResourceInfo info = it->first;
        libtorrent::torrent_handle handle = it->second;
        if ( info.m_name == name )
        {
            m_torr->remove_torrent( handle );
			infomap.erase( it++ );
            return true;
        }
    }
    return false;
}

int TorrentWrapper::GetTorrentSystemStatus() const
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
	ResourceInfoWorkItem* item = new ResourceInfoWorkItem( name, this );
	m_info_download_thread.DoWork( item );
}

TorrentWrapper::DownloadRequestStatus TorrentWrapper::AddTorrent( const PlasmaResourceInfo& info )
{
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
		GetHandleInfoMap()[info] = tor;
        for ( size_t i = 0; i < num_webseeds; ++ i )
            tor.add_url_seed( STD_STRING( info.m_webseeds[i] ) );
        return success;
    }
    catch (std::exception& e) {
       wxLogError(_T("%s"),TowxString( e.what()).c_str()); // TODO (BrainDamage#1#): add message to user on failure
	   return torrent_join_failed;
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
//	std::vector<wxString> torrentsToResume = sett().GetTorrentListToResume();
//	std::vector<wxString> resumeFailures;
//	for ( std::vector<wxString>::const_iterator it = torrentsToResume.begin();
//		it != torrentsToResume.end(); ++it )
//	{
//		if ( _RequestFileByName( *it ) != success )
//			resumeFailures.push_back( *it );
//	}
//	//save new list (hopefully empty)
//	sett().SetTorrentListToResume( resumeFailures );
}

////////////////////////////////////////////////////////
//// private functions to interface with the system ////
////////////////////////////////////////////////////////
void TorrentWrapper::HandleCompleted()
{
	int num_completed = 0;
	{
	TorrenthandleInfoMap infohandle = GetHandleInfoMap();
	TorrenthandleInfoMap::iterator it = infohandle.begin();

	for ( ; it != infohandle.end(); ++it )
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
					wxMutexGuiLocker locker;
					customMessageBoxNoModal( SL_MAIN_ICON, wxString::Format( _("File copy from %s to %s failed.\nPlease copy manually and reload maps/mods afterwards"),
																			 source_path.c_str(), dest_filename.c_str() ),
											 _("Copy failed") );
					//this basically invalidates the handle for further use
					m_torr->remove_torrent( handle );
				}
				else
				{
					wxRemoveFile( source_path );
					wxMutexGuiLocker locker;
					UiEvents::StatusData data( wxString::Format( _("Download completed: %s"), info.m_name.c_str() ), 1 );
					UiEvents::GetStatusEventSender( UiEvents::addStatusMessage ).SendEvent( data );
					num_completed++;
				}
			}
		}
	}
	}
	if ( num_completed > 0 )
	{
		usync().AddReloadEvent();
	}
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
        globalinfos.inspeed = session_status.download_rate;
        globalinfos.numcopies = 0.0f;
        globalinfos.filesize = 0;
        ret[wxString(_T("global"))] = globalinfos;

		const TorrenthandleVector torrentList = m_torr->get_torrents();
		const TorrenthandleInfoMap& infomap = GetHandleInfoMap();
		for ( TorrenthandleInfoMap::const_iterator i = infomap.begin(); i != infomap.end(); ++i )
        {
			try {
				TorrentInfos CurrentTorrent;
				libtorrent::torrent_handle handle = i->second;
				libtorrent::torrent_status torrent_status = handle.status();
				CurrentTorrent.name = i->first.m_name;
				CurrentTorrent.progress = torrent_status.progress;
				CurrentTorrent.downloaded = torrent_status.total_payload_download;
				CurrentTorrent.inspeed = torrent_status.download_payload_rate;
				CurrentTorrent.numcopies = torrent_status.distributed_copies;
				CurrentTorrent.filesize = handle.get_torrent_info().total_size();

				int eta_seconds = -1;
				if ( CurrentTorrent.progress > 0 && CurrentTorrent.inspeed > 0)
					eta_seconds = int (  (CurrentTorrent.filesize - CurrentTorrent.downloaded ) / CurrentTorrent.inspeed );

				CurrentTorrent.eta = eta_seconds;
				CurrentTorrent.downloadstatus = getP2PStatusFromHandle( handle );
				ret[CurrentTorrent.name] = CurrentTorrent;
			}
			catch ( libtorrent::invalid_handle& h )
			{}
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"), TowxString(e.what()).c_str());
        return ret;
    }

    return ret;
}

void TorrentWrapper::RemoveInvalidTorrents()
{
	TorrenthandleInfoMap& infomap = GetHandleInfoMap();
	TorrenthandleInfoMap::iterator it = infomap.begin();
	for ( ; it != infomap.end(); )
    {
        PlasmaResourceInfo info = it->first;
        libtorrent::torrent_handle handle = it->second;
        if ( !handle.is_valid() )
        {
            m_torr->remove_torrent( handle );
			//! TODO: should this remove all files as well??
			infomap.erase( it++ );
        }
        else
            ++it;
    }
}

void TorrentWrapper::ClearFinishedTorrents()
{
	TorrenthandleInfoMap& infomap = GetHandleInfoMap();
	TorrenthandleInfoMap::iterator it = infomap.begin();
	for ( ; it != infomap.end();  )
    {
        PlasmaResourceInfo info = it->first;
        libtorrent::torrent_handle handle = it->second;
		if ( handle.is_valid() && handle.is_seed() )
		{
			//since the handle is still valid, this file has already been copied to the proper destination in datadir
			//so we pass the second arg to delete all associated files but the torrent file itself
			m_torr->remove_torrent( handle, libtorrent::session::delete_files );
			wxRemoveFile( info.m_local_torrent_filepath );
			infomap.erase( it++ );
        }
        else
            ++it;
    }
}

void DisplayError( const wxString& resourcename, TorrentWrapper::DownloadRequestStatus status )
{
	wxString msg;
	switch ( status )
	{
		case TorrentWrapper::no_seeds_found: msg = _("The remote server supplied no sources to download the file from."); break;
		case TorrentWrapper::remote_file_dl_failed: msg = _("The file was not found on the remote server"); break;
		case TorrentWrapper::corrupt_torrent_file: msg = _("The downloaded file was corrupted. You should retry the download."); break;
		case TorrentWrapper::torrent_join_failed: msg = _("The downloaded .torrent file was unusable."); break;
		default: msg = _("Unknown"); break;
	}
	msg = wxString::Format(_("Downloading %s failed with reason:\n%s"), resourcename.c_str(), msg.c_str() );
	wxString title = _("Download failure");
#ifdef __WXMSW__
	UiEvents::StatusData data( wxString::Format(_("Downloading %s failed"), resourcename.c_str() ), 1 );
	UiEvents::GetStatusEventSender( UiEvents::addStatusMessage ).SendEvent( data );
#else
	wxMutexGuiLocker locker;
	customMessageBoxNoModal( SL_MAIN_ICON, msg, title );
#endif
}

ResourceInfoWorkItem::ResourceInfoWorkItem( const  wxString& name, TorrentWrapper* tor )
	: m_name( name ), m_TorrentWrapper( tor )
{}

void ResourceInfoWorkItem::Run()
{
	TorrentWrapper::DownloadRequestStatus stat;
	PlasmaResourceInfo info = plasmaInterface().GetResourceInfo( m_name );
	info.m_name = m_name;
	if( info.m_type == PlasmaResourceInfo::unknown )
	{
		stat = TorrentWrapper::remote_file_dl_failed; //!TODO use ebtter code
	}
	else if ( plasmaInterface().DownloadTorrentFile( info, sett().GetTorrentDir().GetFullPath() ) )
	{
		TorrentWrapper::DownloadRequestStatus dep_status = m_TorrentWrapper->AddTorrent( info );
		if ( dep_status == TorrentWrapper::success ) {
			for ( size_t i = 0; i < info.m_dependencies.Count(); ++i ) {
				wxString dependency_name = info.m_dependencies[i];
				PlasmaResourceInfo dependency_info = plasmaInterface().GetResourceInfo( dependency_name );
				dependency_info.m_name = dependency_name;
				if ( dependency_info.m_type == PlasmaResourceInfo::unknown )
					continue;
				if ( plasmaInterface().DownloadTorrentFile( dependency_info, sett().GetTorrentDataDir().GetFullPath() ) )
					m_TorrentWrapper->AddTorrent( dependency_info );
			}
		}
		stat = dep_status;
	}
	else
	{
		stat = TorrentWrapper::remote_file_dl_failed;
	}
	if ( stat != TorrentWrapper::success )
			DisplayError( m_name, stat );
}

#endif
