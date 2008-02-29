/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#include "iunitsync.h"
#include "settings.h"
#include "utils.h"

#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>

#include <fstream>

#include <wx/tokenzr.h>

#include "torrentwrapper.h"


TorrentWrapper::TorrentWrapper()
{
  m_tracker_urls.Add( _T("http://tracker.caspring.org"));
  m_tracker_urls.Add( _T("tracker2.caspring.org"));
  m_tracker_urls.Add( _T("backup-tracker.licho.eu"));
  torr = new libtorrent::session();
  ReloadLocalFileList();
}


TorrentWrapper::~TorrentWrapper()
{
  delete torr;
}


void TorrentWrapper::ReloadLocalFileList()
{
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
    m_local_files[(long)uhash] = info;
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
    m_local_files[(long)uhash] = info;
  }
}


void TorrentWrapper::ChangeListeningPort( unsigned int port )
{
  try
  {
    torr->listen_on(std::make_pair(port, port));
  } catch (std::exception& e)
  {
    e.what(); /// TODO (BrainDamage#1#): add message on failure
  }
}


void TorrentWrapper::JoinTorrent( const wxString& uhash )
{
  unsigned long hash;
  uhash.ToULong( &hash );
  if ( !m_torrents_infos[ hash ].hash.IsEmpty() && m_open_torrents_number < 5 )
  {
    wxString path = sett().GetSpringDir();
    switch (m_torrents_infos[hash].type)
    {
      case map:
        path = path + _T("/maps/");
      case mod:
        path = path + _T("/mods/");
    }
    libtorrent::sha1_hash torrenthash;
    libtorrent::torrent_handle JoinedTorrent =  torr->add_torrent( STD_STRING(m_tracker_urls[0]), torrenthash,boost::filesystem::path( STD_STRING(path) ), STD_STRING(m_torrents_infos[hash].name) ); /// TODO (BrainDamage#1#): add proper sha1 hash of the torrent file
    /// add url seeds
    for( unsigned int i=0; i < m_torrents_infos[hash].seedurls.GetCount(); i++ )
      JoinedTorrent.add_url_seed( STD_STRING(m_torrents_infos[hash]seedurls[i]), i );
  }
}


void TorrentWrapper::CreateTorrent( const wxString& hash, const wxString& name, MediaType type )
{
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
  boost::filesystem::path InputFilePath = boost::filesystem::complete(boost::filesystem::path( STD_STRING( StringFilePath ) ) );

  libtorrent::add_files(newtorrent, InputFilePath.branch_path(), InputFilePath.leaf() );

  for ( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
  {
    newtorrent.add_tracker( STD_STRING(m_tracker_urls[i] ) );
  }

  libtorrent::file_pool fp;
  libtorrent::storage st(newtorrent, InputFilePath.branch_path(), fp);

  // calculate the hash for all pieces
  int num = newtorrent.num_pieces();
  std::vector<char> buf(newtorrent.piece_size(0));
  for (int i = 0; i < num; ++i)
  {
    st.read(&buf[0], i, 0, newtorrent.piece_size(i));
    libtorrent::hasher h(&buf[0], newtorrent.piece_size(i));
    newtorrent.set_hash(i, h.final());
  }
  libtorrent::entry e = newtorrent.create_torrent();

  std::ofstream TorrentFile( boost::filesystem:: (boost::filesystem::path(), std::ios_base::binary) );
  libtorrent::bencode(std::ostream_iterator<char>(TorrentFile), e);
}


bool TorrentWrapper::RequestFile( const wxString& uhash )
{
  unsigned long hash;
  uhash.ToULong( &hash );
  if ( m_torrents_infos[hash].hash.IsEmpty() ) return false; /// the file is not present in the system
  SocketSend( wxString::Format( _T("N+|%ld\n"), (long)hash ) ); /// request for seeders for the file
  JoinTorrent( uhash );
  return true;
}


void TorrentWrapper::ReceiveandExecute( const wxString& msg )
{
  wxStringTokenizer tkz( msg, '|' );
  wxArrayString data;
  for( unsigned int pos = 0; tkz.HasMoreTokens(); pos++ )
  {
      data[pos] = tkz.GetNextToken(); /// fill the array with the message
  }
  if ( data[0] == _T("T+") ) {
    TorrentData newtorrent;
    long shash;
    data[1].ToLong(&shash);
    newtorrent.hash = data[1];
    newtorrent.name = data[2];
    if ( data[3] == _T("MAP") ) newtorrent.type = map;
    else if ( data[3] == _T("MOD") ) newtorrent.type = mod;
    TorrentsIter iter = m_torrents_infos.begin();
    m_torrents_infos.insert(iter + (unsigned long)shash, newtorrent);
  } else if ( data[0] == _T("T-") ) {
    long shash;
    data[1].ToLong(&shash);
    TorrentsIter iter = m_torrents_infos.begin();
    m_torrents_infos.erase(iter + (unsigned long)shash);
  } else if ( data[0] == _T("S+") ) {
    m_seed_requests.push_back(data[1]);
    long shash;
    data[1].ToLong(&shash);
    if ( torr->get_torrents().size() <= 5 )
    {
      if ( !m_local_files[shash].hash.IsEmpty() )
      {
        TorrentData info = m_torrents_infos[shash];
        JoinTorrent( info.hash );
      }
    }
  } else if ( data[0] == _T("S-") ) {
    m_seed_requests.remove(data[1]);
  } else if ( data[0] == _T("M+") ) {
    long shash;
    data[1].ToLong(&shash);
    m_torrents_infos[(unsigned long)shash].seedurls.Add( data[2] );
  } else if ( data[0] == _T("PING") ) {
    SocketSend( _T("PING\n") );
  }
}
