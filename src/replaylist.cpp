/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <stdexcept>
#include <iterator>

#include "replaylist.h"

#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/arrstr.h>
#include "iunitsync.h"

ReplayList::ReplayList()
{
    wxArrayString filenames = usync()->GetReplayList();
    for (int i = 0; i < filenames.GetCount(); ++i)
    {
        AddReplay( GetReplayInfos( filenames[i] ) );
    }

}


void ReplayList::AddReplay( Replay replay )
{
  m_replays[replay.id] = replay;
}


void ReplayList::RemoveReplay( replay_id_t const& id ) {
  m_replays.erase(id);
}

replay_map_t::size_type ReplayList::GetNumReplays()
{
  return m_replays.size();
}


//void ReplayList_Iter::IteratorBegin()
//{
//  if (m_replaylist) m_iterator = m_replaylist->m_replays.begin();
//}
//
//Replay ReplayList_Iter::GetReplay()
//{
//
//  Replay replay = m_iterator->second;
//  if ( m_replaylist && m_iterator != m_replaylist->m_replays.end() ) ++m_iterator;
//  return replay;
//}
//
//bool ReplayList_Iter::EOL()
//{
//  return ( m_replaylist && m_iterator == m_replaylist->m_replays.end() )?true:false;
//}


Replay ReplayList::GetReplayById( replay_id_t const& id ) {
//TODO catch
  replay_iter_t b = m_replays.find(id);
  if (b == m_replays.end())
    throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");
  return b->second;
}

Replay ReplayList::GetReplay( int const index ) {
//TODO secure index
  replay_iter_t b = m_replays.begin();
  std::advance(b,index);
//  if (b == m_replays.end())
//    throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");
  return b->second;
}

bool ReplayList::ReplayExists( replay_id_t const& id )
{
  return m_replays.find(id) != m_replays.end();
}


Replay GetReplayInfos ( wxString& ReplayPath )
{
  //wxLOG_Info  ( STD_STRING( ReplayPath ) );
//TODO extract moar info
  static long r_id = 0;
  Replay ret;
  ret.Filename = ReplayPath;
  wxString FileName = ReplayPath.AfterLast( '/' ); // strips file path
  FileName = FileName.Left( FileName.Find( _T(".sdf") ) ); //strips the file extension
  wxStringTokenizer args ( FileName, _T("-")); // chunks by '-' separator
//  if ( args.CountTokens() != 3 || args.CountTokens() != 4 ) // not a spring standard replay filename
//  {
//    ret.ReplayName = FileName;
//    return ret;
//  }
  wxString date = args.GetNextToken(); // date format YYMMDD
  ret.date = date;
  date.Left( 2 ).ToLong( &ret.year );
  date.Mid( 3, 4 ).ToLong( &ret.month );
  date.Mid( 5, 6 ).ToLong( &ret.day );
  ret.MapName = args.GetNextToken();
  ret.SpringVersion = args.GetNextToken();
  ret.ReplayName = args.GetNextToken(); // void string if multiple replays wich share previous paramteres aren't present
  ret.id = r_id;
  r_id++;
  return ret;
}


