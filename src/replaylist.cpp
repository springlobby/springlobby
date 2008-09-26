/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <stdexcept>
#include <iterator>
#include <wx/file.h>

#include "replaylist.h"

#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/arrstr.h>
#include <sstream>
#include "iunitsync.h"
#include "utils.h"
#include "settings++/custom_dialogs.h"
#include "tdfcontainer.h"

ReplayList::ReplayList()
{
    wxArrayString filenames = usync().GetReplayList();
    for (unsigned int i = 0; i < filenames.GetCount(); ++i)
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

  std::stringstream ss ( STD_STRING(GetScriptFromReplay(ReplayPath)) );
  PDataList script( ParseTDF(ss) );
  PDataList replayNode ( script->Find(_T("GAME") ) );
  if ( replayNode.ok() ){

  /// PDataList game(root->Find(_T("GAME")))
/// if(!game.ok()){wxLogMessage(_T("Game tag is missing"));return false;}
/// wxString gamename=game->GetString(_T("Mapname"));
//    wxString huh = replayNode->GetString( _T("GameType") );
    ret.ModName = replayNode->GetString( _T("GameType") );
    ret.playernum = s2l( replayNode->GetString( _T("NumPlayers") ) );
    for ( unsigned int i = 0; i < ret.playernum; ++i ){
        PDataList player ( script->Find(_T("PLAYER")+i2s(i) ) );
        ret.playernames.Add( player->GetString( _T("name") ) );
    }

  }
  return ret;
}

wxString GetScriptFromReplay ( wxString& ReplayPath )
{
    wxString script;

    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        replay.Seek( 20 );
        int headerSize ;
        replay.Read( &headerSize, 4);
        replay.Seek( 64 );
        int scriptSize;
        replay.Read( &scriptSize, 4);
        replay.Seek( headerSize );
        char* script_a = new char[scriptSize];
        replay.Read( script_a, scriptSize );
        wxString script = WX_STRINGC( script_a ) ;//(script_a,scriptSize);
        //TODO (koshi) delete me after stable
        serverMessageBox(SL_MAIN_ICON,script,_("GG") );
        return script;
    }
    catch (...)
    {
        return wxEmptyString;
    }

}
