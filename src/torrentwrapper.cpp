/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#include "torrentwrapper.h"

#include "iunitsync.h"
#include "settings.h"


#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>


TorrentWrapper::TorrentWrapper()
{
  m_tracker_urls.Add( _T("http://tracker.caspring.org"));
  torr = new libtorrent::session();
  ReloadLocalFileList();
  for (  )
}


TorrentWrapper::~TorrentWrapper();
{
  delete torr;
}


void TorrentWrapper::ReloadLocalFileList()
{
  m_local_files.clear();
  for ( int i =0; i < usync()->GetNumMaps(); i++ )
  {
    UnitSyncMap mapinfo = usync()->GetMap( i );
    Torrentinfo info;
    info.type = map;
    unsigned long uhash;
    mapinfo.hash.ToLong(&uhash);
    info.hash = wxString::Format( _T("%ld"), (long)uhash );
    info.name = mapinfo.name;
    m_local_files[(long)uhash] = info;
  }
  for ( int i =0; i < usync()->GetNumMods(); i++ )
  {
    UnitSyncMod modinfo = usync()->GetMod( i );
    Torrentinfo info;
    info.type = mod;
    unsigned long uhash;
    modinfo.hash.ToLong(&uhash);
    info.hash = wxString::Format( _T("%ld"), (long)uhash );
    info.name = mpdinfo.name;
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
    e.what() /// TODO (BrainDamage#1#): add message on failure
  }
}


void TorrentWrapper::JoinTorrent( const wxString& name, const wxString& hash, MediaType type )
{
  if ( m_torrents_infos.find( hash ) && m_open_torrents_number < 5 )
  {
    wxString path = sett().GetSpringDir.mb_str();
    switch (type)
    {
      case map:
        path = path + _T("/maps/");
      case mod:
        path = path + _T("/mods/");
    }

    torr->add_torrent( m_tracker_urls[0].mb_str(), hash.mb_str() , shahash,boost::filesystem::path( path.mb_str() ), name.mb_str() );
  }
}


void TorrentWrapper::CreateTorrent( const wxString& hash, const wxString& name, MediaType type )
{
  torrent_info newtorrent;
  add_files(newtorrent, );
  for ( unsigned int i = 0; i < m_tracker_urls.GetCount(); i++ )
  {
    newtorrent.add_tracker( m_tracker_urls[i].mb_str() );
  }

  // calculate the hash for all pieces
  int num = newtorrent.num_pieces();
  std::vector<char> buf(piece_size);
  for (int i = 0; i < num; ++i)
  {
          st.read(&buf[0], i, 0, newtorrent.piece_size(i));
          hasher h(&buf[0], newtorrent.piece_size(i));
          newtorrent.set_hash(i, h.final());
  }
  newtorrent.create_torrent();
}


void ReceiveandExecute( const wxString& msg )
{
  wxArrayString data = wxStringTokenizer::wxStringTokenizer( msg, '|' );
  switch( data[0] )
  {
    case _T("T+"):
      TorrentData newtorrent;
      long shash;
      data[1].ToLong(&shash);
      newtorrent.hash = data[1];
      newtorrent.name = data[2];
      if ( data[3] == _T("MAP") ) newtorrent.type = map;
      else if ( data[3] == _T("MOD") ) newtorrent.type = mod;
      TorrentsIter iter = m_torrents_infos.begin();
      m_torrents_infos.insert(iter + (unsigned long)shash, newtorrent);
    case _T("T-"):
      long shash;
      data[1].ToLong(&shash);
      TorrentsIter iter = m_torrents_infos.begin();
      m_torrents_infos.erase(iter + (unsigned long)shash);
    case _T("S+"):
      m_seed_request.push_back(data[1]);
      long shash;
      data[1].ToLong(&shash);
      if ( torr->get_torrents().size() <= 5 )
      {
        if ( !m_local_files[shash].hash.IsEmpty() )
        {
          TorrentData info = m_torrents_infos[shash];
          JoinTorrent( info.hash, info.name, info.type );
        }
      }
    case _T("S-"):
      m_seed_request.remove(data[1]);
    else:
  }
}
