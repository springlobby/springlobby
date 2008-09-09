/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#ifndef NO_TORRENT_SYSTEM

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
#include <wx/wfstream.h>
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <wx/event.h>

#include "torrentwrapper.h"
#include "settings++/custom_dialogs.h"


bool TorrentTable::IsConsistent()
{
#ifdef TorrentTable_validate
    for (std::set<TorrentTable::PRow>::iterator i=all_torrents.begin();i!=all_torrents.end();++i)
    {
        if (hash_index.count((*i)->hash)==0)return false;
        if (name_index.count((*i)->name)==0)return false;
        /// handle_index might not contain the torrent with invalid/null handle
    }
#endif
    return true;
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
    /// enforce all duplicates or no duplicates for now.
    if (duplicates!=0 && duplicates!=2)
    {
        wxLogWarning(_T("TorrentTable: insert would cause inconsistency, not all keys are duplicated!"));
        wxLogWarning(_T("TorrentTable: insert not done!"));
        return;
    }

    /// duplicate handles are not so bad, but a message may be useful
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

void TorrentTable::RemoveRowHandle( PRow row )
{
  handle_index.erase(row->handle);
  row->handle= libtorrent::torrent_handle();
}

void TorrentTable::SetRowStatus( TorrentTable::PRow row, FileStatus status )
{
  if ( row->status == seeding || row->status == leeching )
  {
    if ( status != seeding && status != leeching ) RemoveRowHandle( row );
  }
  if ( row->status == queued && status != queued ) queued_torrents.erase( row );
  if ( status == queued ) queued_torrents.insert( row );
  row->status = status;
}

void TorrentTable::AddSeedRequest(TorrentTable::PRow row)
{
    seed_requests.insert(row);
}
void TorrentTable::RemoveSeedRequest(TorrentTable::PRow row)
{
    seed_requests.erase(row);
}

bool TorrentTable::IsSeedRequest(TorrentTable::PRow row)
{
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



TorrentWrapper& torrent()
{
    static TorrentWrapper m_torr_wrap;
    return m_torr_wrap;
}


TorrentWrapper::TorrentWrapper():
        ingame(false),
        m_seed_count(0),
        m_leech_count(0),
        m_timer_count(0),
        m_is_connecting(false)
{
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
    DisconnectToP2PSystem();
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


void TorrentWrapper::DisconnectToP2PSystem()
{
    if ( IsConnectedToP2PSystem() ) m_socket_class->Disconnect();
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
        wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
    }
}


bool TorrentWrapper::IsFileInSystem( const wxString& hash )
{
    return m_torrent_table.RowByHash(hash).ok();
}


bool TorrentWrapper::RemoveTorrentByHash( const wxString& hash )
{
    TorrentTable::PRow row=m_torrent_table.RowByHash(hash);
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


DownloadRequestStatus TorrentWrapper::RequestFileByHash( const wxString& hash )
{
    TorrentTable::PRow row=m_torrent_table.RowByHash(hash);
    if ( !row.ok() ) return file_not_found;
    return RequestFileByRow( row );
}


DownloadRequestStatus TorrentWrapper::RequestFileByName( const wxString& name )
{
    TorrentTable::PRow row=m_torrent_table.RowByName(name);
    if ( !row.ok() ) return file_not_found;
    return RequestFileByRow( row );
}


void TorrentWrapper::SetIngameStatus( bool status )
{
    if ( status == ingame ) return; /// no change needed
    ingame = status;
    if ( !IsConnectedToP2PSystem() ) return;
    try
    {
        std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
        if ( ingame ) /// going ingame, pause all torrents (or throttle speeds) and disable dht
        {
            if ( sett().GetTorrentSystemSuspendMode() == 0 ) for ( unsigned int i = 0; i < TorrentList.size(); i++) TorrentList[i].pause();
            else
            {
                m_torr->set_upload_rate_limit(sett().GetTorrentThrottledUploadRate() * 1024);
                m_torr->set_download_rate_limit(sett().GetTorrentThrottledDownloadRate() *1024 );
            }
            m_torr->stop_dht();
        }
        else/// game closed, resume all torrents (or reset normal speed) and reactivate dht
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
        wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
    }
}


void TorrentWrapper::UpdateFromTimer( int mselapsed )
{
    m_timer_count++;
    if ( m_timer_count < 20 ) return;///update every 2 sec
    m_timer_count = 0;
    if ( m_is_connecting )
    {
        if ( IsConnectedToP2PSystem() )
            m_is_connecting = false;
        else
            ConnectToP2PSystem( m_connected_tracker_index +1 );
    }

    if (!ingame && IsConnectedToP2PSystem() )
    {
        ///  DON'T alter function call order here or bad things may happend like locust, earthquakes or raptor attack
        JoinRequestedTorrents();
        RemoveUnneededTorrents();
        TryToJoinQueuedTorrents();
        ResumeFromList();
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
            if (success == RequestFileByHash( TorrentsToResume[i] ) ) /// resume all open leeched files when system as disconnected last time
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


DownloadRequestStatus TorrentWrapper::RequestFileByRow( const TorrentTable::PRow& row )
{
    if (ingame) return paused_ingame;
    if ( !IsConnectedToP2PSystem()  ) return not_connected;

    if (!row.ok())return file_not_found;

    if (row->status==leeching||(row->status&stored) || (row->status == queued) ) return duplicate_request;

    if ( m_leech_count > 4 )
    {
        GetTorrentTable().SetRowStatus( row, queued );
        return scheduled_in_cue;
    }

    if ( !JoinTorrent( row, false ) ) return torrent_join_failed;

    m_socket_class->Send( wxString::Format( _T("N+|%s\n"), row->hash.c_str() ) ); /// request for seeders for the file
    return success;
}


bool TorrentWrapper::RemoveTorrentByRow( const TorrentTable::PRow& row )
{
    if (!row.ok())return false;
    try
    {
        bool filecompleted = row->handle.is_seed();
        m_torr->remove_torrent( row->handle );

        if (row->status==seeding) m_seed_count--;
        else if (row->status==leeching) m_leech_count--;

        if ( filecompleted ) GetTorrentTable().SetRowStatus( row, stored ); /// fix the file status and automatically remove row handle
        else GetTorrentTable().SetRowStatus( row, not_stored );
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
        globalinfos.downloadstatus = leeching;
        globalinfos.progress = 0.0f;
        globalinfos.downloaded = 0;
        globalinfos.uploaded = 0;
        globalinfos.outspeed = m_torr->status().upload_rate;
        globalinfos.inspeed = m_torr->status().download_rate;
        globalinfos.numcopies = 0.0f;
        globalinfos.filesize = 0;
        ret[0] = globalinfos;

        if ( ingame || !IsConnectedToP2PSystem()  ) return ret; /// stop updating the gui if disconneted

        std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
        for ( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
        {
            TorrentInfos CurrentTorrent;
            CurrentTorrent.name = WX_STRING(i->name()).BeforeFirst(_T('|'));
            if ( i->is_seed() ) CurrentTorrent.downloadstatus = seeding;
            else CurrentTorrent.downloadstatus = leeching;
            CurrentTorrent.progress = i->status().progress;
            CurrentTorrent.downloaded = i->status().total_payload_download;
            CurrentTorrent.uploaded = i->status().total_payload_upload;
            CurrentTorrent.inspeed = i->status().download_payload_rate;;
            CurrentTorrent.outspeed = i->status().upload_payload_rate;;
            CurrentTorrent.numcopies = i->status().distributed_copies;
            CurrentTorrent.filesize = i->get_torrent_info().total_size();

            TorrentTable::PRow row=m_torrent_table.RowByHandle(*i);
            if (!row.ok()) continue;
            CurrentTorrent.hash=row->hash;

            ret[s2l(CurrentTorrent.hash)] = CurrentTorrent;
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"), WX_STRINGC(e.what()).c_str());
    }

    /// display infos about queued torrents

    std::set<TorrentTable::PRow> queuedrequests = GetTorrentTable().QueuedTorrentsByRow();
    for ( std::set<TorrentTable::PRow>::iterator it = queuedrequests.begin(); ( it != queuedrequests.end() ) && ( m_leech_count < 4 ); it++ )
    {
        TorrentInfos QueuedTorrent;
        QueuedTorrent.numcopies = -1;
        QueuedTorrent.hash = (*it)->hash;
        QueuedTorrent.downloadstatus = queued;
        QueuedTorrent.name=(*it)->name;
        ret[s2l(QueuedTorrent.hash)] = QueuedTorrent;
    }

    return ret;
}


void TorrentWrapper::SendMessageToCoordinator( const wxString& message )
{
    if ( IsConnectedToP2PSystem()  ) m_socket_class->Send( message + _T("\n") );
}


bool TorrentWrapper::JoinTorrent( const TorrentTable::PRow& row, bool IsSeed )
{
    if ( !row.ok() ) return false;
    wxLogMessage(_T("(1) Joining torrent, hash=%s"),row->hash.c_str());
    if (ingame) return false;

    wxLogMessage(_T("(2) Joining torrent. IsSeed: ") + TowxString(IsSeed) + _T(" status: ") + TowxString(row->status) );

    if ( IsSeed && row->status != stored ) return false;
    if ( !IsSeed && ( row->status != queued ) && ( row->status != not_stored ) ) return false;

    wxString torrent_name=row->name;
    wxString torrent_infohash_b64=row->infohash;
    wxString path;

    switch (row->type)
    {
      case map:
      {
        torrent_name = torrent_name + _T("|MAP");
        break;
      }
      case mod:
      {
        torrent_name = torrent_name + _T("|MOD");
        break;
      }
    }

    if ( IsSeed )
    {
      switch( row->type ) /// if file is not present locally you can't seed it
      {
        case map:
        {
          if ( !usync()->MapExists( row->name, row->hash ) ) return false;
          break;
        }
        case mod:
        {
          if ( !usync()->ModExists( row->name, row->hash ) ) return false;
          break;
        }
      }

      try
      {
       path = usync()->GetArchivePath( row->name );
      } catch (std::exception& e)
       {
         wxLogError( WX_STRINGC( e.what() ) );
         wxLogMessage( _T("Local filepath couldn't be determined") );
         return false;
       }
      wxLogMessage( _T("local filename: %s"), path.c_str() );

    }
    else
    {
      path = sett().GetSpringDir() + wxFileName::GetPathSeparator();
      switch (row->type)
      {
        case map:
        {
          path = path + _T("maps") + wxFileName::GetPathSeparator();
          break;
        }
        case mod:
        {
          path = path + _T("mods") + wxFileName::GetPathSeparator();
          break;
        }
      }
    }
    wxLogMessage(_T("(3) Joining torrent: downloading info file"));
    if (!DownloadTorrentFileFromTracker( row->hash )) return false;

    /// read torrent from file
    std::ifstream in( wxString( sett().GetTorrentsFolder() + row->hash + _T(".torrent") ).mb_str(), std::ios_base::binary);
    in.unsetf(std::ios_base::skipws);
    libtorrent::entry e = libtorrent::bdecode(std::istream_iterator<char>(in), std::istream_iterator<char>());
    libtorrent::torrent_info t_info(e); /// decode the torrent infos from the file
    wxString torrentfilename = WX_STRING(t_info.begin_files()->path.string()); /// get the file name in the torrent infos
    wxLogMessage( _T("requested filename: %s"), torrentfilename.c_str() );


    if ( IsSeed )
    {
      int index = path.Find( torrentfilename );
      if ( index == -1 ) return false; /// if the filename locally is different from the torrent's, skip it or it will download it again and various crap may happend.
      path = path.Left( index ); /// truncate the path so it matches the archive's
    }
    wxLogMessage(_T("(4) Joining torrent: add_torrent(%s,[%s],%s,[%s])"),m_tracker_urls[m_connected_tracker_index].c_str(),torrent_infohash_b64.c_str(),row->name.c_str(),path.c_str());


    try
    {
        m_torrent_table.SetRowHandle(row, m_torr->add_torrent( t_info, boost::filesystem::path(STD_STRING(path))));
        if(IsSeed){
          if(row->handle.is_valid()){
            std::vector<bool> tmp(1,false);/// length 1 filled with falses
            row->handle.filter_files(tmp);
          }else{
            wxLogMessage(_T("cant set seed not to download"));
          }
        }
    }
    catch (std::exception& e)
    {
        wxLogError(_T("%s"),WX_STRINGC( e.what()).c_str()); /// TODO (BrainDamage#1#): add message to user on failure
    }

    if ( IsSeed )
    {
       GetTorrentTable().SetRowStatus( row, seeding );
       m_seed_count++;
    }
    else
    {
       GetTorrentTable().SetRowStatus( row, leeching );
      m_leech_count++;
    }

    wxLogMessage(_T("(5) Joining torrent: done"));
    return true;
}


void TorrentWrapper::CreateTorrent( const wxString& hash, const wxString& name, MediaType type )
{
    if (ingame) return;


    if ( sett().GetSpringDir().IsEmpty() ) return; /// no good things can happend if you don't know which folder to r/w files from

    libtorrent::torrent_info newtorrent;

    wxString StringFilePath = sett().GetSpringDir() + wxFileName::GetPathSeparator();
    switch (type)
    {
    case map:
        StringFilePath += _T("maps") + wxFileName::GetPathSeparator();
    case mod:
        StringFilePath += _T("mods") + wxFileName::GetPathSeparator();
    }
    StringFilePath += name;
    boost::filesystem::path InputFilePath = complete(boost::filesystem::path( STD_STRING( StringFilePath ) ) );

    newtorrent.add_file( InputFilePath.branch_path(), boost::filesystem::file_size( InputFilePath ) );

    for ( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
    {
        newtorrent.add_tracker( STD_STRING(m_tracker_urls[i] +  _T(":DEFAULT_P2P_TRACKER_PORT/announce") ) );
    }

    wxFile torrentfile( StringFilePath );
    if ( !torrentfile.IsOpened() ) return;
    /// calculate the hash for all pieces
    int num = newtorrent.num_pieces();
    std::vector<char> buf(newtorrent.piece_size(0));
    for ( int i = 0; i < num; ++i)
    {
        torrentfile.Read(&buf[0], newtorrent.piece_size(i));
        libtorrent::hasher h(&buf[0], newtorrent.piece_size(i));
        newtorrent.set_hash(i, h.final());
    }

    switch (type)
    {
    case map:
        newtorrent.set_comment( wxString( name + _T("|MAP") ).mb_str() );
    case mod:
        newtorrent.set_comment( wxString( name + _T("|MOD") ).mb_str() );
    }

    newtorrent.create_torrent(); /// creates the torrent and publishes on the tracker
}


bool TorrentWrapper::DownloadTorrentFileFromTracker( const wxString& hash )
{
    if ( sett().GetSpringDir().IsEmpty() ) return false; /// no good things can happend if you don't know which folder to r/w files from

#ifdef HAVE_WX26
    wxFileName filename( sett().GetTorrentsFolder() + hash + _T(".torrent") ) ;
    bool readable = filename.IsOk();
#else
    bool readable = wxFileName::IsFileReadable( sett().GetTorrentsFolder()+ hash + _T(".torrent") ) ;
#endif

    if ( readable  ) return true; ///file already present locally

    wxLogMessage(_T("torrent system downloading torrent info %s"), hash.c_str() );

    wxHTTP fileRequest;
    //versionRequest.SetHeader(_T("Content-type"), _T(""));
    /// normal timeout is 10 minutes.. set to 10 secs.
    fileRequest.SetTimeout(10);
    fileRequest.Connect( m_tracker_urls[m_connected_tracker_index], 80);
    wxInputStream *stream = fileRequest.GetInputStream(  _T("/torrents/") + hash + _T(".torrent") );
    bool ret = false;
    if (fileRequest.GetError() == wxPROTO_NOERR)
    {

        wxFileOutputStream output( sett().GetTorrentsFolder() + hash + _T(".torrent") );
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
        if(!it->ok())continue;

        if ( m_seed_count > 9 ) break; /// too many seeds open

        if ( (*it)->status != stored ) continue; /// torrent must be present locally and not seeding/leeching

        JoinTorrent( *it, true );

    }

}

void TorrentWrapper::RemoveUnneededTorrents()
{
    std::map<libtorrent::torrent_handle, TorrentTable::PRow> torrenthandles = GetTorrentTable().RowByTorrentHandles();
    for (std::map<libtorrent::torrent_handle, TorrentTable::PRow>::iterator  it = torrenthandles.begin(); it != torrenthandles.end(); ++it)
    {
        if ( !it->first.is_seed() ) continue;


        if ( it->second->status == leeching ) /// if torrent was opened in leech mode but now it's seeding it means it was requested from the user but now it's completed
        { ///torrent has finished download, refresh unitsync and remove file from list
            try
            {
                ASSERT_RUNTIME( RemoveTorrentByRow( it->second ), _T("failed to remove torrent: ")+ it->second->hash );

                m_socket_class->Send( _T("N-|")  + it->second->hash + _T("\n") ); ///notify the system we don't need the file anymore

                wxCommandEvent refreshevt(UnitSyncReloadRequest); /// request an unitsync reload
                wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), refreshevt );
            }
            catch (std::exception& e)
            {
                wxLogError( WX_STRINGC( e.what() ) );
            }
        }

        if ( GetTorrentTable().IsSeedRequest( it->second ) )/// if torrent not in request list but still seeding then remove
        {
            try
            {
                ASSERT_RUNTIME( RemoveTorrentByRow( it->second ), _T("failed to remove torrent: ")+ it->second->hash );
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

    if ( m_leech_count < 5 )
    {
        /// join queued files if there are available slots
        std::set<TorrentTable::PRow> queuedrequests = GetTorrentTable().QueuedTorrentsByRow();
        for ( std::set<TorrentTable::PRow>::iterator it = queuedrequests.begin(); ( it != queuedrequests.end() ) && ( m_leech_count < 4 ); it++ )
        {
            if ( !it->ok() ) continue;
            RequestFileByRow( *it );
        }
    }

}

void TorrentWrapper::ReceiveandExecute( const wxString& msg )
{
    wxLogMessage(_T("torrent: %s"), msg.c_str() );

    wxArrayString data = wxStringTokenize( msg, _T('|') );

    if ( data.GetCount() == 0 ) return;
    // T+|hash|name|type 	 informs client that new torrent was added to server (type is either MOD or MAP)
    else if ( data.GetCount() > 3 && data[0] == _T("T+") )
    {

        TorrentTable::PRow newtorrent = new TorrentTable::Row;

        newtorrent->hash = data[1];
        newtorrent->name = data[2];
        if ( data[3] == _T("MAP") )
        {
           newtorrent->type = map;
           if ( usync()->MapExists( data[2], data[1] ) ) newtorrent->status = stored;
        }
        else if ( data[3] == _T("MOD") )
        {
           newtorrent->type = mod;
           if ( usync()->ModExists( data[2], data[1] ) ) newtorrent->status = stored;
        }

        GetTorrentTable().InsertRow( newtorrent );

        // m_socket_class->Send(  _T("IH|") + data[1] + _T("\n") );

        // T-|hash 	 informs client that torrent was removed from server
    }
    else if ( data[0] == _T("T-") && data.GetCount() > 1 )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( data[1] );
        if ( !row.ok() ) return;
        GetTorrentTable().RemoveRow( row );

        // S+|hash|seeders|leechers 	 tells client that seed is needed for this torrent
    }
    else if ( data.GetCount() > 1 && data[0] == _T("S+") )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( data[1] );
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

        TorrentTable::PRow row = GetTorrentTable().RowByHash( data[1] );
        if ( !row.ok() ) return;
        GetTorrentTable().RemoveSeedRequest( row );

        // M+|hash|url 	 It tells the client if url is given that http mirror exists for given hash, else there are no mirrors.
    }
    else if ( data[0] == _T("M+") && data.GetCount() > 2 )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( data[1] );
        if ( !row.ok() ) return;

        for ( unsigned int index = 2; index < data.GetCount(); index++ )
        {
            row->handle.add_url_seed( STD_STRING( data[index] ) );
        }

        // PING 	 every minute - client must respond with its own "PING"
    }
    else if ( data[0] == _T("PING") )
    {

        m_socket_class->Send( _T("PING\n") );

        //IH|hash|infohash infos the client about torrent's infohash b64 encoded
    }
    else if ( data.GetCount() > 2 && data[0] == _T("IH") )
    {

        TorrentTable::PRow row = GetTorrentTable().RowByHash( data[1] );
        if ( !row.ok() ) return;

        row->infohash = data[2];

    }
}


void TorrentWrapper::OnConnected( Socket* sock )
{
    wxLogMessage(_T("torrent system connected") );
    try
    {
        m_torr->start_dht();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
    }

    m_torrent_table = TorrentTable(); /// flush the torrent data

    m_seed_count = 0;
    m_leech_count = 0;


}


void TorrentWrapper::OnDisconnected( Socket* sock )
{
    wxLogMessage(_T("torrent system disconnected") );

    std::set<TorrentTable::PRow> queued =  GetTorrentTable().QueuedTorrentsByRow();
    wxArrayString TorrentsToResume;
    for ( std::set<TorrentTable::PRow>::iterator it = queued.begin(); it != queued.end(); it++ ) TorrentsToResume.Add( (*it)->hash );

    std::map<libtorrent::torrent_handle, TorrentTable::PRow> handles =  GetTorrentTable().RowByTorrentHandles();

    for ( std::map<libtorrent::torrent_handle, TorrentTable::PRow>::iterator i = handles.begin(); i != handles.end(); i++)
    {
        if ( !i->first.is_seed() ) TorrentsToResume.Add( i->second->hash ); /// save leeching torrents for resume on next connection

        m_socket_class->Send( wxString::Format( _T("N-|%s\n"), i->second->hash.c_str() ) ); /// release all files requests
        try
        {
            m_torr->remove_torrent(i->first); ///remove all torrents upon disconnect
        }
        catch (std::exception& e)
        {
            wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
        }
    }

    try
    {
        m_torr->stop_dht();
    }
    catch (std::exception& e)
    {
        wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
    }


    m_torrent_table = TorrentTable(); /// flush the torrent data

    m_seed_count = 0;
    m_leech_count = 0;

    sett().SetTorrentListToResume( TorrentsToResume );
}


void TorrentWrapper::OnDataReceived( Socket* sock )
{
    if ( sock == 0 ) return;

    wxString data;


    do
    {

        data = _T("");
        if ( sock->Receive( data ) )
        {
            m_buffer += data;
            wxString cmd;
            if ( ( cmd = m_buffer.BeforeFirst( '\n' ) ) != _T("") )
            {
                m_buffer = m_buffer.AfterFirst( '\n' );
                ReceiveandExecute( cmd );
            }
        }
    }
    while ( !data.IsEmpty() );
}

#endif
