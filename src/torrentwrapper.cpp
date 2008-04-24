/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#ifndef NO_TORRENT_SYSTEM

#include "iunitsync.h"
#include "settings.h"
#include "utils.h"
#include "socket.h"

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
#include <wx/sstream.h>

#include "torrentwrapper.h"

TorrentWrapper* torrent()
{
  static TorrentWrapper* m_torr_wrap = 0;
  if (!m_torr_wrap)
    m_torr_wrap = new TorrentWrapper;
  return m_torr_wrap;
}


TorrentWrapper::TorrentWrapper():
m_connected(false),
ingame(false)
{
  m_tracker_urls.Add( _T("tracker.caspring.org"));
  m_tracker_urls.Add( _T("tracker2.caspring.org"));
  m_tracker_urls.Add( _T("backup-tracker.licho.eu"));
  m_torr = new libtorrent::session();
  m_torr->add_extension(&libtorrent::create_metadata_plugin);
  m_torr->add_extension(&libtorrent::create_ut_pex_plugin);
  m_torr->start_upnp();
  m_torr->start_natpmp();
  m_torr->start_lsd();
  m_torr->start_dht();
  m_socket_class = new Socket( *this );
  UpdateSettings();
  ReloadLocalFileList();
}


TorrentWrapper::~TorrentWrapper()
{
  m_torr->stop_upnp();
  m_torr->stop_natpmp();
  m_torr->stop_lsd();
  m_torr->stop_dht();
  DisconnectToP2PSystem();
  delete m_torr;
  delete m_socket_class;
}


////////////////////////////////////////////////////////
////                gui interface                   ////
////////////////////////////////////////////////////////

void TorrentWrapper::ConnectToP2PSystem()
{
  m_socket_class->Connect( m_tracker_urls[0], DEFAULT_P2P_COORDINATOR_PORT );
  m_connected_tracker_index= 0;
  return;
  for( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
  {
    m_socket_class->Connect( m_tracker_urls[i], DEFAULT_P2P_COORDINATOR_PORT );
    if ( m_connected )
    {
       m_connected_tracker_index = i;
       return;
    }
  }
}


void TorrentWrapper::DisconnectToP2PSystem()
{
  if ( m_connected ) m_socket_class->Disconnect();
}


bool TorrentWrapper::IsConnectedToP2PSystem()
{
  return m_connected;
}


void TorrentWrapper::UpdateSettings()
{
  m_torr->set_upload_rate_limit(sett().GetTorrentUploadRate());
  m_torr->set_download_rate_limit(sett().GetTorrentDownloadRate());
  m_torr->set_max_connections(sett().GetTorrentMaxConnections());
  try
  {
    m_torr->listen_on(std::make_pair(sett().GetTorrentPort(), sett().GetTorrentPort()));
  } catch (std::exception& e)
  {
    e.what(); /// TODO (BrainDamage#1#): add message on failure
  }
}


bool TorrentWrapper::IsFileInSystem( const wxString& uhash )
{
  unsigned long ulong;
  uhash.ToULong( &ulong );
  ScopedLocker<HashToTorrentData> torrents_infos_l(m_torrents_infos);
  return torrents_infos_l.Get().find(u2s((int)ulong)) != torrents_infos_l.Get().end();
}

////////////////////////////////////////////////////////
////               lobby interface                  ////
////////////////////////////////////////////////////////


void TorrentWrapper::ReloadLocalFileList()
{
  if (ingame) return;
  {
    ScopedLocker<HashToTorrentData> local_files_l(m_local_files);
    local_files_l.Get().clear();
  }
  for ( int i =0; i < usync()->GetNumMaps(); i++ )
  {
    UnitSyncMap mapinfo = usync()->GetMap( i );
    TorrentData info;
    info.type = map;
    unsigned long uhash;
    mapinfo.hash.ToULong(&uhash);
    info.hash = wxString::Format( _T("%d"), (int)uhash );
    info.name = mapinfo.name;
    {
      ScopedLocker<HashToTorrentData> local_files_l(m_local_files);
      local_files_l.Get()[info.hash] = info;
    }
  }
  for ( int i =0; i < usync()->GetNumMods(); i++ )
  {
    UnitSyncMod modinfo = usync()->GetMod( i );
    TorrentData info;
    info.type = mod;
    unsigned long uhash;
    modinfo.hash.ToULong(&uhash);
    info.hash = wxString::Format( _T("%d"), (int)uhash );
    info.name = modinfo.name;
    {
      ScopedLocker<HashToTorrentData> local_files_l(m_local_files);
      local_files_l.Get()[info.hash] = info;
    }
  }
}


bool TorrentWrapper::RequestFile( const wxString& uhash )
{
  if (ingame) return false;
  if ( !m_connected ) return false;
  unsigned long hash;
  uhash.ToULong( &hash );
  int singedver = (int)hash;
  wxString shash = i2s(singedver);//wxString::Format( _T("%d"), (int)hash );

  wxString name;
  {
    ScopedLocker<HashToTorrentData> torrents_infos_l(m_torrents_infos);
    HashToTorrentData::iterator it=torrents_infos_l.Get().find(shash);
    if (it==torrents_infos_l.Get().end()) return false;
    if ( it->second.hash.IsEmpty() ) return false; /// the file is not present in the system
    name=it->second.name;
  }

  if ( !JoinTorrent( shash ) ) return false;
  m_socket_class->Send( wxString::Format( _T("N+|%d\n"), (int)hash ) ); /// request for seeders for the file
  m_leech_count++;
  {
    ScopedLocker<OpenTorrents> open_torrents_l(m_open_torrents);
    open_torrents_l.Get()[name] = false; /// not seeding when just joined
  }
  return true;
}


void TorrentWrapper::SetIngameStatus( bool status )
{
  if ( status == ingame ) return; /// no change needed
  ingame = status;
  std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
  if ( ingame ) /// going ingame, pause all torrents and disable dht
  {
    for ( unsigned int i = 0; i < TorrentList.size(); i++) TorrentList[i].pause();
    m_torr->stop_dht();
  }
  else/// game closed, resume all torrents and reactivate dht
  {
    m_torr->start_dht();
    for ( unsigned int i = 0; i < TorrentList.size(); i++) TorrentList[i].resume();
  }

}


void TorrentWrapper::UpdateFromTimer( int mselapsed )
{
  m_timer_count++;
  if ( m_timer_count < 20 ) return;////update every 2 sec
  m_timer_count = 0;
  if (!ingame) FixTorrentList();
}


////////////////////////////////////////////////////////
//// private functions to interface with the system ////
////////////////////////////////////////////////////////


std::map<int,TorrentInfos> TorrentWrapper::CollectGuiInfos()
{
  std::map<int,TorrentInfos> ret;
  std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
  for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
  {
    TorrentInfos CurrentTorrent;
    CurrentTorrent.name = WX_STRING(i->name()).BeforeFirst(_T('|'));
    CurrentTorrent.leeching = !i->is_seed();
    CurrentTorrent.progress = i->status().progress;
    CurrentTorrent.downloaded = i->status().total_payload_download;
    CurrentTorrent.uploaded = i->status().total_payload_upload;
    CurrentTorrent.inspeed = i->status().total_payload_download;
    CurrentTorrent.outspeed = i->status().total_payload_upload;
    CurrentTorrent.numcopies = i->status().distributed_copies;
    {
      ScopedLocker<HashToTorrentData> torrents_infos_l(m_torrents_infos);
      CurrentTorrent.filehash = s2l(torrents_infos_l.Get()[CurrentTorrent.name].hash);
    }

    ret[CurrentTorrent.filehash] = CurrentTorrent;
  }
  return ret;
}


bool TorrentWrapper::JoinTorrent( const wxString& hash )
{
  if (ingame) return false;

  MediaType type;
  wxString torrent_name;
  wxString torrent_infohash;
  {
    ScopedLocker<HashToTorrentData> torrents_infos_l(m_torrents_infos);
    HashToTorrentData::iterator it=torrents_infos_l.Get().find(hash);
    if(it==torrents_infos_l.Get().end())return false;
    type=it->second.type;
    torrent_name=it->second.name;
    torrent_infohash=it->second.infohash;
  }
  m_socket_class->Send(  _T("IH|") + hash + _T("\n") ); /// request for infohash
  wxString path = sett().GetSpringDir();
  wxString name;
  if ( type == map )
  {
    path = path + _T("/maps/");
    name = torrent_name + _T("|MAP");
  }
  else
  {
    path = path + _T("/mods/");
    name = torrent_name + _T("|MOD");
  }
  /*
  if ( !wxFileName::IsFileReadable( sett().GetSpringDir() + _T("/torrents/") + hash ) ) /// file descriptor not present, download it
  {
     while (!DownloadTorrentFileFromTracker( hash ) );
     {
       m_connected_tracker_index++;
       if ( m_connected_tracker_index > m_tracker_urls.Count() -1 ) return false;
     }
  }
  /// read torrent from file
  std::ifstream in( wxString( sett().GetSpringDir() + _T("/torrents/") + hash ).mb_str(), std::ios_base::binary);
  in.unsetf(std::ios_base::skipws);
  libtorrent::entry e = libtorrent::bdecode(std::istream_iterator<char>(in), std::istream_iterator<char>());
  libtorrent::torrent_handle JoinedTorrent =  m_torr->add_torrent(libtorrent::torrent_info(e), boost::filesystem::path( STD_STRING( path ) ) );
  */
  libtorrent::sha1_hash infohash( STD_STRING( torrent_infohash ) );
  libtorrent::torrent_handle JoinedTorrent =  m_torr->add_torrent( m_tracker_urls[m_connected_tracker_index].mb_str(), infohash, name.mb_str(), boost::filesystem::path( STD_STRING( path ) ) );
  return true;
}


void TorrentWrapper::CreateTorrent( const wxString& hash, const wxString& name, MediaType type )
{
  if (ingame) return;
  libtorrent::torrent_info newtorrent;

  wxString StringFilePath = sett().GetSpringDir();
  switch (type)
  {
    case map:
      StringFilePath += _T("/maps/");
    case mod:
      StringFilePath += _T("/mods/");
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

  libtorrent::entry e = newtorrent.create_torrent();

  /// write the torrent to a file
  std::ofstream TorrentFile( wxString( sett().GetSpringDir() +  _T("/torrents/") + hash ).mb_str(), std::ios_base::binary );
  bencode(std::ostream_iterator<char>(TorrentFile), e);
}


bool TorrentWrapper::DownloadTorrentFileFromTracker( const wxString& shash )
{
  wxLogMessage(_T("torrent system downloading torrent info %s"), shash.c_str() );
  wxHTTP fileRequest;
  //versionRequest.SetHeader(_T("Content-type"), _T(""));
  /// normal timeout is 10 minutes.. set to 10 secs.
  fileRequest.SetTimeout(10);
  fileRequest.Connect( m_tracker_urls[m_connected_tracker_index], DEFAULT_P2P_COORDINATOR_PORT);
  wxInputStream *stream = fileRequest.GetInputStream( _T("/") + shash + _T(".torrent") );

  if (fileRequest.GetError() == wxPROTO_NOERR)
  {
   wxFile FileDescriptor( sett().GetSpringDir() +  _T("/torrents/") + shash, wxFile::write );
   if ( FileDescriptor.IsOpened() )
   {
    wxString buffer;
    wxStringOutputStream output(&buffer);
    stream->Read(output);
    FileDescriptor.Write( buffer );
    FileDescriptor.Close();
   }

   wxDELETE(stream);
   fileRequest.Close();
  wxLogMessage(_T("torrent system downloading torrent info %s successful"), shash.c_str() );
   return true;
  }

  wxDELETE(stream);
  fileRequest.Close();
  wxLogMessage(_T("torrent system downloading torrent info %s failed"), shash.c_str() );
  return false;
}


void TorrentWrapper::FixTorrentList()
{
  wxLogMessage(_T("torrent system doing maintenance") );
  std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
  //std::map<wxString,wxString> InvertedSeedRequests;
  //InvertedSeedRequests.swap(m_seed_requests);

  m_seed_count = 0;
  m_leech_count = 0;
  for ( SeedRequests::iterator i = m_seed_requests.begin(); i != m_seed_requests.end(); i++ )
  {
    if( m_seed_count > 9 ) break;
    if ( (m_open_torrents.find( i->first ) == m_open_torrents.end()) && (m_local_files.find(i->second) != m_local_files.end()) ) /// torrent is requested and present, but not joined yet
    {
      JoinTorrent( i->second );
      m_seed_count++;
      m_open_torrents[i->first] = true;
    }
  }
  for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
  {
    if ( i->is_seed() ) m_seed_count++;
    else
    {
      m_leech_count++;
      break;
    }
    wxString StrippedName = WX_STRING(i->name()).BeforeFirst( _T('|') );
    if ( !m_open_torrents[StrippedName] ) ///torrent has finished download, refresh unitsync and remove file from list
    {
      m_socket_class->Send( _T("N-|") + m_seed_requests[StrippedName] + _T("\n") ); ///notify the system we don't need the file anymore
      usync()->ReloadUnitSyncLib();
      m_torr->remove_torrent( *i );
      m_open_torrents.erase(m_open_torrents.find(StrippedName));
      continue;
    }
    if ( InvertedSeedRequests.find( StrippedName ) == InvertedSeedRequests.end() )/// if torrent not in request list but still seeding then remove
    {
      m_torr->remove_torrent( *i );
      m_open_torrents.erase(m_open_torrents.find( StrippedName ));
    }
  }
}


void TorrentWrapper::ReceiveandExecute( const wxString& msg )
{
  wxLogMessage(_T("torrent: %s"), msg.c_str() );
  wxStringTokenizer tkz( msg, _T('|') );
  wxArrayString data;
  for( unsigned int pos = 0; tkz.HasMoreTokens(); pos++ )
  {
      data.Add( tkz.GetNextToken() ); /// fill the array with the message
  }
  // T+|hash|name|type 	 informs client that new torrent was added to server (type is either MOD or MAP)
  if ( data[0] == _T("T+") ) {
    TorrentData newtorrent;
    newtorrent.hash = data[1];
    newtorrent.name = data[2];
    if ( data[3] == _T("MAP") ) newtorrent.type = map;
    else if ( data[3] == _T("MOD") ) newtorrent.type = mod;
    m_torrents_infos[data[1]] = newtorrent;
    m_socket_class->Send(  _T("IH|") + data[1] + _T("\n") );
  // T-|hash 	 informs client that torrent was removed from server
  } else if ( data[0] == _T("T-") ) {
    m_torrents_infos.erase(data[1]);
  // S+|hash|seeders|leechers 	 tells client that seed is needed for this torrent
  } else if ( data[0] == _T("S+") ) {
    m_seed_requests[m_torrents_infos[data[1]].name] = data[1];
    unsigned long seeders;
    unsigned long leechers;
    data[2].ToULong(&seeders);
    data[3].ToULong(&leechers);
  // S-|hash 	 tells client that seed is no longer neede for this torrent
  } else if ( data[0] == _T("S-") ) {
    m_seed_requests.erase(m_torrents_infos[data[1]].name);
  // M+|hash|url 	 It tells the client if url is given that http mirror exists for given hash, else there are no mirrors.
  } else if ( data[0] == _T("M+") ) {
    std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
    for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++)
    {
      wxString StrippedName = WX_STRING(i->name()).BeforeFirst( _T('|') );
      if ( m_seed_requests[StrippedName] = data[1] )
      {
        i->add_url_seed( STD_STRING( data[2] ) );
        return;
      }
    }
  // PING 	 every minute - client must respond with its own "PING"
  } else if ( data[0] == _T("PING") ) {
    m_socket_class->Send( _T("PING\n") );
  //IH|hash|infohash infos the client about torrent's infohash b64 encoded
  } else if ( data[0] == _T("IH") ) {
    m_torrents_infos[data[1]].infohash = data[2];
  }
}


void TorrentWrapper::OnConnected( Socket* sock )
{
  wxLogMessage(_T("torrent system connected") );
  m_connected = true;
  m_torrents_infos.clear();
  m_seed_requests.clear();
  m_open_torrents.clear();
  m_seed_count = 0;
  m_leech_count = 0;
}


void TorrentWrapper::OnDisconnected( Socket* sock )
{
  wxLogMessage(_T("torrent system disconnected") );
  std::vector<libtorrent::torrent_handle> TorrentList = m_torr->get_torrents();
  for( std::vector<libtorrent::torrent_handle>::iterator i = TorrentList.begin(); i != TorrentList.end(); i++) m_torr->remove_torrent(*i); ///remove all torrents upon disconnect
  m_connected = false;
  m_torrents_infos.clear();
  m_seed_requests.clear();
  m_open_torrents.clear();
  m_seed_count = 0;
  m_leech_count = 0;
}


void TorrentWrapper::OnDataReceived( Socket* sock )
{
  if ( sock == 0 ) return;

  wxString data;
  if ( sock->Receive( data ) ) ReceiveandExecute( data );
}

#endif
