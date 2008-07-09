/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#ifndef NO_TORRENT_SYSTEM

#include "iunitsync.h"
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

TorrentWrapper* torrent()
{
  static TorrentWrapper* m_torr_wrap = 0;
  if (!m_torr_wrap)
    m_torr_wrap = new TorrentWrapper;
  return m_torr_wrap;
}


TorrentWrapper::TorrentWrapper():
ingame(false),
m_seed_count(0),
m_leech_count(0),
m_timer_count(0)
{
  m_tracker_urls.Add( _T("tracker.caspring.org"));
  m_tracker_urls.Add( _T("tracker2.caspring.org"));
  m_tracker_urls.Add( _T("backup-tracker.licho.eu"));
  m_torr = new libtorrent::session();
  try
  {
    m_torr->add_extension(&libtorrent::create_metadata_plugin);
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  try
  {
    m_torr->add_extension(&libtorrent::create_ut_pex_plugin);
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  try
  {
    m_torr->start_upnp();
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  try
  {
    m_torr->start_natpmp();
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  try
  {
    m_torr->start_lsd();
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  m_socket_class = new Socket( *this );
  UpdateSettings();
}


TorrentWrapper::~TorrentWrapper()
{
  try
  {
    m_torr->stop_upnp();
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  try
  {
    m_torr->stop_natpmp();
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  try
  {
    m_torr->stop_lsd();
  } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
  DisconnectToP2PSystem();
  delete m_torr;
  delete m_socket_class;
}


////////////////////////////////////////////////////////
////                gui interface                   ////
////////////////////////////////////////////////////////

bool TorrentWrapper::ConnectToP2PSystem()
{
  if ( IsConnectedToP2PSystem() ) return true;
  m_socket_class->Connect( m_tracker_urls[0], DEFAULT_P2P_COORDINATOR_PORT );
  m_connected_tracker_index= 0;
  return IsConnectedToP2PSystem();
  for( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
  {
    m_socket_class->Connect( m_tracker_urls[i], DEFAULT_P2P_COORDINATOR_PORT );
    if ( IsConnectedToP2PSystem() )
    {
       m_connected_tracker_index = i;
       return true;
    }
    else m_socket_class->Disconnect();
  }
  return false;
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
  try
  {
    if ( !ingame || sett().GetTorrentSystemSuspendMode() == 0 )
    {
      m_torr->set_upload_rate_limit(sett().GetTorrentUploadRate() * 1024);
      m_torr->set_download_rate_limit(sett().GetTorrentDownloadRate() *1024 );
    }
    else
    {
      m_torr->set_upload_rate_limit(sett().GetTorrentThrottledUploadRate() * 1024);
      m_torr->set_download_rate_limit(sett().GetTorrentThrottledDownloadRate() *1024 );
    }
    m_torr->set_max_connections(sett().GetTorrentMaxConnections());

    m_torr->listen_on(std::make_pair(sett().GetTorrentPort(), sett().GetTorrentPort()));

  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
  }
}


bool TorrentWrapper::IsFileInSystem( const wxString& hash )
{
  return m_torrent_infos.find(hash) != m_torrent_infos.end();
}


void TorrentWrapper::RemoveFile( const wxString& hash )
{
  wxString name;
  HashToTorrentData::iterator it = m_torrent_infos.find(hash);
  if (it==m_torrent_infos.end()) return; /// file not in system
  name = it->second.name;

  TorrentHandleToHash::to::iterator iter = m_torrent_handles.to.find(hash);
  if ( iter == m_torrent_handles.to.end() ) return; /// torrent handler not found
  if ( iter->second.is_seed() ) m_seed_count--;
  else m_leech_count--;

  try
  {
  m_torr->remove_torrent( iter->second );
  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
    return;
  }

  OpenTorrents::iterator itor = m_open_torrents.find(name);
  if ( itor == m_open_torrents.end() ) return; /// bad hash request
  m_open_torrents.erase(itor);
}


int TorrentWrapper::GetTorrentSystemStatus()
{
  if (!IsConnectedToP2PSystem()) return 0;
  if (ingame) return 2;
  return 1;
}


HashToTorrentData& TorrentWrapper::GetSystemFileList()
{
    return m_torrent_infos;
}


////////////////////////////////////////////////////////
////               lobby interface                  ////
////////////////////////////////////////////////////////


DownloadRequestStatus TorrentWrapper::RequestFileByHash( const wxString& hash )
{
  if (ingame) return paused_ingame;
  if ( !IsConnectedToP2PSystem()  ) return not_connected;

  wxString name;

  HashToTorrentData::iterator it=m_torrent_infos.find(hash);
  if (it==m_torrent_infos.end()) return file_not_found;
  if ( it->second.hash.IsEmpty() ) return file_not_found; /// the file is not present in the system
  name=it->second.name;


  OpenTorrents::iterator itor = m_open_torrents.find(name);
  if ( itor != m_open_torrents.end() ) return duplicate_request; /// don't request twice the same file

  if ( m_leech_count > 4 )
  {
    if ( m_queued_requests.Index( hash ) != wxNOT_FOUND ) return duplicate_request; /// don't request twice the same file
    m_queued_requests.Add( hash );
    return scheduled_in_cue;
  }

  if ( !JoinTorrent( hash ) ) return torrent_join_failed;
  m_socket_class->Send( wxString::Format( _T("N+|%s\n"), hash.c_str() ) ); /// request for seeders for the file
  m_leech_count++;
  m_open_torrents[name] = false; /// not seeding when just joined

  return success;
}


DownloadRequestStatus TorrentWrapper::RequestFileByName( const wxString& name )
{
    NameToHash::from::iterator iter = m_name_to_hash.from.find(name);
    if( iter == m_name_to_hash.from.end() ) return file_not_found;
    return RequestFileByHash( iter->second );
}


void TorrentWrapper::SetIngameStatus( bool status )
{
  if ( !IsConnectedToP2PSystem() ) return;
  if ( status == ingame ) return; /// no change needed
  ingame = status;
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
  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
  }
}


void TorrentWrapper::UpdateFromTimer( int mselapsed )
{
  m_timer_count++;
  if ( m_timer_count < 20 ) return;////update every 2 sec
  m_timer_count = 0;
  if (!ingame && IsConnectedToP2PSystem() )
  {
      FixTorrentList();
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
        for ( unsigned int i = 0; i < ResumeCount; i++ ) {
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
    for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
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

      TorrentHandleToHash::iterator itor = m_torrent_handles.from.find(*i);
      if ( itor == m_torrent_handles.end() ) continue;
      CurrentTorrent.hash = itor->second;

      ret[s2l(CurrentTorrent.hash)] = CurrentTorrent;
    }
  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) );
  }

  /// display infos about queued torrents
  wxArrayString queuecopy = m_queued_requests;

  unsigned int RequestCount = queuecopy.GetCount();
  for ( unsigned int i = 0; i < RequestCount; i++ )
  {
    TorrentInfos QueuedTorrent;
    QueuedTorrent.numcopies = -1;
    QueuedTorrent.hash = queuecopy[i];
    QueuedTorrent.downloadstatus = queued;

    HashToTorrentData::iterator it=m_torrent_infos.find(QueuedTorrent.hash);
    if (it==m_torrent_infos.end()) continue; /// the file is not present in the system
    if ( it->second.hash.IsEmpty() ) continue; /// the file is not present in the system
    QueuedTorrent.name =it->second.name;

    ret[s2l(QueuedTorrent.hash)] = QueuedTorrent;
  }

  return ret;
}


void TorrentWrapper::SendMessageToCoordinator( const wxString& message )
{
  if ( IsConnectedToP2PSystem()  ) m_socket_class->Send( message + _T("\n") );
}


bool TorrentWrapper::JoinTorrent( const wxString& hash )
{
  wxLogMessage(_T("(1) Joining torrent, hash=%s"),hash.c_str());
  if (ingame) return false;
  wxLogMessage(_T("(2) Joining torrent."));

  MediaType type;
  wxString torrent_name;
  wxString torrent_infohash_b64;

  HashToTorrentData::iterator it=m_torrent_infos.find(hash);
  if(it==m_torrent_infos.end())
  {
    wxLogMessage(_T("(3) Joining torrent: hash not found"));
    return false;
  }
  type=it->second.type;
  torrent_name=it->second.name;
  torrent_infohash_b64=it->second.infohash;

  wxString path = sett().GetSpringDir() + wxFileName::GetPathSeparator();
  wxString name;
  if ( type == map )
  {
    path = path + _T("maps") + wxFileName::GetPathSeparator();
    name = torrent_name + _T("|MAP");
  }
  else
  {
    path = path + _T("mods") + wxFileName::GetPathSeparator();
    name = torrent_name + _T("|MOD");
  }
  wxLogMessage(_T("(3) Joining torrent: downloading info file"));

  #ifdef HAVE_WX26
    wxFileName filename( sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("torrents")  + wxFileName::GetPathSeparator() + hash + _T(".torrent") ) ;
    bool readable = filename.IsOk();
  #else
    bool readable = wxFileName::IsFileReadable( sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("torrents")  + wxFileName::GetPathSeparator() + hash + _T(".torrent") ) ;
  #endif

  if ( !readable  ) /// file descriptor not present, download it
  {
     if (!DownloadTorrentFileFromTracker( hash )) return false;
  }
  /// read torrent from file
  std::ifstream in( wxString( sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("torrents") + wxFileName::GetPathSeparator() + hash + _T(".torrent") ).mb_str(), std::ios_base::binary);
  in.unsetf(std::ios_base::skipws);
  libtorrent::entry e = libtorrent::bdecode(std::istream_iterator<char>(in), std::istream_iterator<char>());
  wxLogMessage(_T("(4) Joining torrent: add_torrent(%s,[%s],%s,[%s])"),m_tracker_urls[m_connected_tracker_index].c_str(),torrent_infohash_b64.c_str(),name.c_str(),path.c_str());

  try
  {
    m_torrent_handles.to[hash] = m_torr->add_torrent(libtorrent::torrent_info(e), boost::filesystem::path( STD_STRING( path ) ) );
  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
  }

  /*
  wxLogMessage(_T("torrent b64 infohash: %s"), torrent_infohash_b64.c_str() );
  std::string torrent_infohash_binary = wxBase64::Decode(torrent_infohash_b64 );

  wxString torrent_infohash_reencoded=wxBase64::Encode(reinterpret_cast<const wxUint8*>(torrent_infohash_binary.c_str()),torrent_infohash_binary.size());

  if(torrent_infohash_reencoded!=torrent_infohash_b64){
    wxLogMessage(_T("Base64 decoding phailed!"));
  }

  wxString torrent_infohash_b16=ToBase16((unsigned char *)&torrent_infohash_binary[0],torrent_infohash_binary.size());

  wxLogMessage( _T("torrent b16 infohash: %s"),torrent_infohash_b16.c_str());

  libtorrent::sha1_hash infohash( torrent_infohash_binary );

  wxLogMessage(_T("(4) Joining torrent: add_torrent(%s,[%s],%s,[%s])"),m_tracker_urls[m_connected_tracker_index].c_str(),torrent_infohash_reencoded.c_str(),name.c_str(),path.c_str());

  m_torr->add_torrent( m_tracker_urls[m_connected_tracker_index].mb_str(), infohash, name.mb_str(), boost::filesystem::path( STD_STRING( path ) ) );
  */
  wxLogMessage(_T("(5) Joining torrent: done"));
  return true;
}


void TorrentWrapper::CreateTorrent( const wxString& hash, const wxString& name, MediaType type )
{
  if (ingame) return;


  if ( sett().GetSpringDir().IsEmpty() ) return; /// no good things can happend if you don't know which folder to r/w files from
  bool creationsuccess = true;
  if ( !wxFileName::DirExists( sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("torrents") + wxFileName::GetPathSeparator()  ) ) creationsuccess = wxFileName::Mkdir(  sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("torrents") + wxFileName::GetPathSeparator()  );
  if (!creationsuccess) return;

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
  wxLogMessage(_T("torrent system downloading torrent info %s"), hash.c_str() );

  if ( sett().GetSpringDir().IsEmpty() ) return false; /// no good things can happend if you don't know which folder to r/w files from
  bool creationsuccess = true;
  if ( !wxFileName::DirExists(  sett().GetSpringDir() +  wxFileName::GetPathSeparator()  + _T("torrents") +  wxFileName::GetPathSeparator()   ) ) creationsuccess = wxFileName::Mkdir(  sett().GetSpringDir() +  wxFileName::GetPathSeparator() +  _T("torrents") +  wxFileName::GetPathSeparator()   );
  if (!creationsuccess) return false;
  wxHTTP fileRequest;
  //versionRequest.SetHeader(_T("Content-type"), _T(""));
  /// normal timeout is 10 minutes.. set to 10 secs.
  fileRequest.SetTimeout(10);
  fileRequest.Connect( m_tracker_urls[m_connected_tracker_index], 80);
  wxInputStream *stream = fileRequest.GetInputStream(  _T("/torrents/") + hash + _T(".torrent") );
  bool ret = false;
  if (fileRequest.GetError() == wxPROTO_NOERR)
  {

    wxFileOutputStream output(sett().GetSpringDir() + wxFileName::GetPathSeparator() +  _T("torrents") + wxFileName::GetPathSeparator() + hash + _T(".torrent") );
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


void TorrentWrapper::FixTorrentList()
{

  m_seed_count = 0;
  m_leech_count = 0;


  for ( SeedRequests::iterator i = m_seed_requests.begin(); i != m_seed_requests.end(); i++ )
  {
    if( m_seed_count > 9 ) break;
    if (  ( usync()->MapExists( i->first, i->second ) || usync()->ModExists( i->first ) ) && (m_open_torrents.find( i->first ) == m_open_torrents.end()) && (m_torrent_infos.find(i->second) !=m_torrent_infos.end()) ) /// torrent is requested and present, but not joined yet
    {
        JoinTorrent( i->second );
        m_seed_count++;
        m_open_torrents[i->first] = true;
    }
  }


  std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
  for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
  {
    if ( i->is_seed() ) m_seed_count++;
    else
    {
      m_leech_count++;
      continue;
    }

    wxString StrippedName;

    TorrentHandleToHash::iterator itor = m_torrent_handles.from.find(*i);
    if ( itor == m_torrent_handles.end() ) continue;

    HashToTorrentData::iterator iter =m_torrent_infos.find( itor->second );
    if ( iter ==m_torrent_infos.end() ) continue;
    StrippedName = iter->second.name;

    OpenTorrents::iterator open_torrent_i=m_open_torrents.find(StrippedName);

    if ( open_torrent_i != m_open_torrents.end() && !(open_torrent_i->second) ) ///torrent has finished download, refresh unitsync and remove file from list
    {
      try
      {
        m_torr->remove_torrent( *i );

        m_leech_count--;
        m_open_torrents.erase(open_torrent_i);
        m_torrent_handles.erase( m_torrent_handles.from.find(*i) );

        m_socket_class->Send( _T("N-|")  + m_seed_requests.from[StrippedName] + _T("\n") ); ///notify the system we don't need the file anymore

        wxCommandEvent refreshevt(UnitSyncReloadRequest); /// request an unitsync reload
        wxPostEvent( &SL_GlobalEvtHandler::GetSL_GlobalEvtHandler(), refreshevt );
      } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
    }

    if ( m_seed_requests.from.find( StrippedName ) == m_seed_requests.from.end() )/// if torrent not in request list but still seeding then remove
    {
      try
      {
        m_torr->remove_torrent( *i );
        m_torrent_handles.erase( m_torrent_handles.from.find(*i) );
        if( open_torrent_i != m_open_torrents.end() ) m_open_torrents.erase(open_torrent_i);
      } catch (std::exception& e) {wxLogError( WX_STRINGC( e.what() ) );}
    }
  }

  if ( m_leech_count < 5 )
  { /// join queued files if there are available slots
    wxArrayString queuecopy = m_queued_requests;
    unsigned int RequestCount = queuecopy.GetCount();
    for ( unsigned int i = 0; ( ( i < RequestCount) && ( m_leech_count < 4 ) ); i++ )
    {
      if ( RequestFileByHash( queuecopy[i] ) == success ) m_queued_requests.RemoveAt( i );
    }
  }
}


void TorrentWrapper::ReceiveandExecute( const wxString& msg )
{
  wxLogMessage(_T("torrent: %s"), msg.c_str() );

  wxArrayString data = wxStringTokenize( msg, _T('|') );

  if ( data.GetCount() == 0 ) return;
  // T+|hash|name|type 	 informs client that new torrent was added to server (type is either MOD or MAP)
  else if ( data.GetCount() > 3 && data[0] == _T("T+") ) {

    TorrentData newtorrent;
    newtorrent.hash = data[1];
    newtorrent.name = data[2];
    if ( data[3] == _T("MAP") ) newtorrent.type = map;
    else if ( data[3] == _T("MOD") ) newtorrent.type = mod;

    m_torrent_infos[data[1]] = newtorrent;
    m_name_to_hash.from[newtorrent.name] = newtorrent.hash;

   // m_socket_class->Send(  _T("IH|") + data[1] + _T("\n") );

  // T-|hash 	 informs client that torrent was removed from server
  } else if ( data[0] == _T("T-") && data.GetCount() > 1 ) {

    HashToTorrentData::iterator itor =m_torrent_infos.find(data[1]);
    if( itor ==m_torrent_infos.end() ) return;

    m_torrent_infos.erase( itor );

    NameToHash::to::iterator iter = m_name_to_hash.to.find(data[1]);
    if( iter == m_name_to_hash.to.end() ) return;

    m_name_to_hash.erase( iter );

  // S+|hash|seeders|leechers 	 tells client that seed is needed for this torrent
  } else if ( data.GetCount() > 1 && data[0] == _T("S+") ) {

    wxString name;

    HashToTorrentData::iterator itor =m_torrent_infos.find(data[1]);
    if ( itor ==m_torrent_infos.end() ) return;
    name =  itor->second.name;

    m_seed_requests.from[name] = data[1];

    unsigned long seeders=0;
    unsigned long leechers=0;
    if(data.GetCount() > 2)data[2].ToULong(&seeders);
    if(data.GetCount() > 3)data[3].ToULong(&leechers);

  // S-|hash 	 tells client that seed is no longer neede for this torrent
  } else if ( data.GetCount() > 1 && data[0] == _T("S-") ) {

    wxString name;

    HashToTorrentData::iterator itor =m_torrent_infos.find(data[1]);
    if ( itor ==m_torrent_infos.end() ) return;
    name =  itor->second.name;

    SeedRequests::iterator iter = m_seed_requests.find(name);
    if ( iter == m_seed_requests.end() ) return;

    m_seed_requests.from.erase(iter);

  // M+|hash|url 	 It tells the client if url is given that http mirror exists for given hash, else there are no mirrors.
  } else if ( data[0] == _T("M+") && data.GetCount() > 2 ) {

    TorrentHandleToHash::to::iterator iter = m_torrent_handles.to.find(data[1]);
    if ( iter == m_torrent_handles.to.end() ) return;

    for( unsigned int index = 2; index < data.GetCount(); index++ )
      iter->second.add_url_seed( STD_STRING( data[index] ) );

  // PING 	 every minute - client must respond with its own "PING"
  } else if ( data[0] == _T("PING") ) {

    m_socket_class->Send( _T("PING\n") );

  //IH|hash|infohash infos the client about torrent's infohash b64 encoded
  } else if ( data.GetCount() > 2 && data[0] == _T("IH") ) {

    HashToTorrentData::iterator itor =m_torrent_infos.find(data[1]);
    if ( itor ==m_torrent_infos.end() ) return;

    itor->second.infohash = data[2];

  }
}


void TorrentWrapper::OnConnected( Socket* sock )
{
  wxLogMessage(_T("torrent system connected") );
  try
  {
    m_torr->start_dht();
  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
  }

   m_torrent_infos.clear();
   m_seed_requests.clear();
   m_open_torrents.clear();
   m_torrent_handles.clear();
   m_queued_requests.Empty();
   m_name_to_hash.clear();

  m_seed_count = 0;
  m_leech_count = 0;


}


void TorrentWrapper::OnDisconnected( Socket* sock )
{
  wxLogMessage(_T("torrent system disconnected") );

  wxArrayString TorrentsToResume = m_queued_requests;

  std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
  for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
  {
     TorrentHandleToHash::from::iterator iter = m_torrent_handles.from.find(*i);
     if ( iter == m_torrent_handles.from.end() ) continue; /// torrent handler not found

     if ( !i->is_seed() ) TorrentsToResume.Add( iter->second ); /// save leeching torrents for resume on next connection

     m_socket_class->Send( wxString::Format( _T("N-|%s\n"), iter->second.c_str() ) ); /// release all files requests
    try
    {
     m_torr->remove_torrent(*i); ///remove all torrents upon disconnect
    } catch (std::exception& e)
    {
      wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
    }
  }

  try
  {
    m_torr->stop_dht();
  } catch (std::exception& e)
  {
    wxLogError( WX_STRINGC( e.what() ) ); /// TODO (BrainDamage#1#): add message to user on failure
  }


  m_torrent_infos.clear();
  m_seed_requests.clear();
  m_open_torrents.clear();
  m_torrent_handles.clear();
  m_queued_requests.clear();
  m_name_to_hash.clear();

  m_seed_count = 0;
  m_leech_count = 0;

  sett().SetTorrentListToResume( TorrentsToResume );
}


void TorrentWrapper::OnDataReceived( Socket* sock )
{
  if ( sock == 0 ) return;

  wxString data;


  do {

    data = _T("");
    if ( sock->Receive( data ) ) {
      m_buffer += data;
      wxString cmd;
      if ( ( cmd = m_buffer.BeforeFirst( '\n' ) ) != _T("") )
      {
        m_buffer = m_buffer.AfterFirst( '\n' );
        ReceiveandExecute( cmd );
      }
    }
  } while ( !data.IsEmpty() );
}

#endif
