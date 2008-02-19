/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: TorrentWrapper
//

#include "torrentwrapper.h"

#include "iunitsync.h"
#include "settings.h"

/*
#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
*/

TorrentWrapper::TorrentWrapper()
{
  m_tracker_urls.Add( _T("http://tracker.caspring.org"));
  torr = new libtorrent::session();
}


TorrentWrapper::~TorrentWrapper();
{
  delete torr;
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


void TorrentWrapper::JoinTorrent( const wxString& name, unsigned int hash, MediaType type )
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
    std::string crchash = wxString::Format( _T("%d"), (int)hash ).mb_str();
    sha1_hash shahash = crchash.c_str();
    torr->add_torrent( m_tracker_urls[0].mb_str(), shahash,boost::filesystem::path( path.mb_str() ), name.mb_str() );
    m_open_torrents_number++;
  }
}


void TorrentWrapper::CreateTorrent( unsigned int hash )
{
  torrent_info newtorrent;
  add_files(newtorrent, );
  newtorrent.set_piece_size(256 * 1024);
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
  m_torrents_infos[hash] = newtorrent; /// stores the newly created torrent in the vector
  return newtorrent.create_torrent();
}


void ReceiveandExecute( const wxString& msg )
{
  wxArrayString data = wxStringTokenizer::wxStringTokenizer( msg, '|' );
  switch( data[0] )
  {
    case _T("T+"):
      TorrentData newtorrent;
      long hash;
      data[1].ToLong(&hash);
      newtorrent.hash = (unsigned long)shash;
      newtorrent.name = data[2];
      if ( data[3] == _T("MAP") ) newtorrent.type = map;
      else if ( data[3] == _T("MOD") ) newtorrent.type = mod;
      TorrentsIter iter = m_torrents_infos.begin();
      m_torrents_infos.insert(iter + (unsigned long)shash, newtorrent);
    case _T("T-"):
      long hash;
      data[1].ToLong(&hash);
      TorrentsIter iter = m_torrents_infos.begin();
      m_torrents_infos.erase(iter + (unsigned long)shash);
    case _T("S+"):
      long hash;
      data[1].ToLong(&hash);
      m_seed_request.push_back((unsigned long)shash);
    case _T("S-"):
      long hash;
      data[1].ToLong(&hash);
      m_seed_request.remove((unsigned long)shash);
    else:
  }
}
