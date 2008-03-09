/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

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

#include "torrentwrapper.h"

TorrentWrapper* torrent()
{
  static TorrentWrapper* m_torr_wrap = 0;
  if (!m_torr_wrap)
    m_torr_wrap = new TorrentWrapper;
  return m_torr_wrap;
}


TorrentWrapper::TorrentWrapper():
m_connected(false)
{
  m_tracker_urls.Add( _T("tracker.caspring.org"));
  m_tracker_urls.Add( _T("tracker2.caspring.org"));
  m_tracker_urls.Add( _T("backup-tracker.licho.eu"));
  m_torr = new libtorrent::session();
  m_torr->add_extension(&libtorrent::create_metadata_plugin);
  m_torr->add_extension(&libtorrent::create_ut_pex_plugin);
  m_socket_class = new Socket( *this );
  ReloadLocalFileList();
}


TorrentWrapper::~TorrentWrapper()
{
  delete m_torr;
  delete m_socket_class;
}


////////////////////////////////////////////////////////
////                gui interface                   ////
////////////////////////////////////////////////////////

void TorrentWrapper::ConnectToP2PSystem()
{
  for( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
  {
    m_socket_class->Connect( m_tracker_urls[i], DEFAULT_P2P_COORDINATOR_PORT );
    if ( m_connected ) return;
  }
}


void TorrentWrapper::DisconnectToP2PSystem()
{
  m_socket_class->Disconnect();
}


void TorrentWrapper::ChangeListeningPort( unsigned int port )
{
  try
  {
    m_torr->listen_on(std::make_pair(port, port));
  } catch (std::exception& e)
  {
    e.what(); /// TODO (BrainDamage#1#): add message on failure
  }
}


void TorrentWrapper::ChangeUploadSpeedLimit( unsigned int speed )
{
  m_torr->set_upload_rate_limit(speed);
}


void TorrentWrapper::ChangeDownloadSpeedLimit( unsigned int speed )
{
  m_torr->set_download_rate_limit(speed);
}


////////////////////////////////////////////////////////
////               lobby interface                  ////
////////////////////////////////////////////////////////


void TorrentWrapper::ReloadLocalFileList()
{
  if (ingame) return;
  m_local_files.clear();
  for ( int i =0; i < usync()->GetNumMaps(); i++ )
  {
    UnitSyncMap mapinfo = usync()->GetMap( i );
    TorrentData info;
    info.type = map;
    unsigned long uhash;
    mapinfo.hash.ToULong(&uhash);
    info.hash = wxString::Format( _T("%ld"), (long)uhash );
    info.name = mapinfo.name;
    m_local_files[info.hash] = info;
  }
  for ( int i =0; i < usync()->GetNumMods(); i++ )
  {
    UnitSyncMod modinfo = usync()->GetMod( i );
    TorrentData info;
    info.type = mod;
    unsigned long uhash;
    modinfo.hash.ToULong(&uhash);
    info.hash = wxString::Format( _T("%ld"), (long)uhash );
    info.name = modinfo.name;
    m_local_files[info.hash] = info;
  }
}


bool TorrentWrapper::RequestFile( const wxString& uhash )
{
  if (ingame) return;
  if ( m_connected ) return false;
  unsigned long hash;
  uhash.ToULong( &hash );
  wxString shash = wxString::Format( _T("%ld"), (long)hash );
  if ( m_torrents_infos[shash].hash.IsEmpty() ) return false; /// the file is not present in the system
  m_socket_class->Send( wxString::Format( _T("N+|%ld\n"), (long)hash ) ); /// request for seeders for the file
  JoinTorrent( shash );
  m_leech_joined[shash] = 1;
  return true;
}


void TorrentWrapper::SetIngameStatus( bool status )
{
  if ( status == ingame ) return; /// no change needed
  ingame = status;
  std::vector<torrent_handle> TorrentList = m_torr->get_torrents();
  if ( ingame ) /// going ingame, pause all torrents
  {
    for ( unsigned int i = 0; i < TorrentList.count(); i++) TorrentList[i]->pause();
  }
  else/// game closed, resume all torrents
  {
    for ( unsigned int i = 0; i < TorrentList.count(); i++) TorrentList[i]->resume();
  }

}


////////////////////////////////////////////////////////
//// private functions to interface with the system ////
////////////////////////////////////////////////////////


void TorrentWrapper::JoinTorrent( const wxString& hash )
{
  if (ingame) return;
  if ( !m_torrents_infos[ hash ].hash.IsEmpty() )
  {
    wxString path = sett().GetSpringDir();
    switch (m_torrents_infos[hash].type)
    {
      case map:
        path = path + _T("/maps/");
      case mod:
        path = path + _T("/mods/");
    }

    /// read torrent from file
    std::ifstream in( wxString( sett().GetSpringDir() + _T("/torrents/") + hash ).mb_str(), std::ios_base::binary);
    in.unsetf(std::ios_base::skipws);
    libtorrent::entry e = libtorrent::bdecode(std::istream_iterator<char>(in), std::istream_iterator<char>());
    libtorrent::torrent_handle JoinedTorrent =  m_torr->add_torrent(libtorrent::torrent_info(e), boost::filesystem::path( STD_STRING( path ) ) );
    /// add url seeds
    for( unsigned int i=0; i < m_torrents_infos[hash].seedurls.GetCount(); i++ )
      JoinedTorrent.add_url_seed( STD_STRING( m_torrents_infos[hash].seedurls[i] ) );
  }
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

  libtorrent::file_pool fp;
  libtorrent::storage st(newtorrent, InputFilePath.branch_path(), fp);

  /// calculate the hash for all pieces
  int num = newtorrent.num_pieces();
  std::vector<char> buf(newtorrent.piece_size(0));
  for (int i = 0; i < num; ++i)
  {
    st.read(&buf[0], i, 0, newtorrent.piece_size(i));
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


void TorrentWrapper::ReceiveandExecute( const wxString& msg )
{
  wxStringTokenizer tkz( msg, '|' );
  wxArrayString data;
  for( unsigned int pos = 0; tkz.HasMoreTokens(); pos++ )
  {
      data[pos] = tkz.GetNextToken(); /// fill the array with the message
  }
  // T+|hash|name|type 	 informs client that new torrent was added to server (type is either MOD or MAP)
  if ( data[0] == _T("T+") ) {
    TorrentData newtorrent;
    newtorrent.hash = data[1];
    newtorrent.name = data[2];
    if ( data[3] == _T("MAP") ) newtorrent.type = map;
    else if ( data[3] == _T("MOD") ) newtorrent.type = mod;
    m_torrents_infos[data[1]] = newtorrent;
  // T-|hash 	 informs client that torrent was removed from server
  } else if ( data[0] == _T("T-") ) {
    m_torrents_infos.erase(data[1]);
  // S+|hash|seeders|leechers 	 tells client that seed is needed for this torrent
  } else if ( data[0] == _T("S+") ) {
    m_seed_requests.push_back(data[1]);
    unsigned long seeders;
    unsigned long leechers;
    data[2].ToULong(&seeders);
    data[3].ToULong(&leechers);
    if ( m_seed_joined.size() <= 10 && !ingame )
    {
      if ( !m_local_files[data[1]].hash.IsEmpty() )
      {
        JoinTorrent( data[1] );
        m_seed_joined[data[1]] = seeders-leechers;
      }
    }
  // S-|hash 	 tells client that seed is no longer neede for this torrent
  } else if ( data[0] == _T("S-") ) {
    m_seed_requests.remove(data[1]);
    if ( m_seed_joined.count( data[1] ) > 0 )
    {
      m_seed_joined.erase( data[1] );
      ///TODO: leave the torrent
    }
  // M+|hash|url 	 It tells the client if url is given that http mirror exists for given hash, else there are no mirrors.
  } else if ( data[0] == _T("M+") ) {
    wxArrayString urllist = m_torrents_infos[data[1]].seedurls;
    urllist.Add( data[2] );
    m_torrents_infos[data[1]].seedurls = urllist;
  // PING 	 every minute - client must respond with its own "PING"
  } else if ( data[0] == _T("PING") ) {
    m_socket_class->Send( _T("PING\n") );
  }
}


void TorrentWrapper::OnConnected( Socket* sock )
{
  m_connected = true;
}


void TorrentWrapper::OnDisconnected( Socket* sock )
{
  m_connected = false;
  m_torrents_infos.clear();
  m_seed_requests.clear();
  m_leech_joined.clear();
  m_seed_joined.clear();
}


void TorrentWrapper::OnDataReceived( Socket* sock )
{
  if ( sock == 0 ) return;

  wxString data;
  if ( sock->Receive( data ) ) ReceiveandExecute( data );
}

