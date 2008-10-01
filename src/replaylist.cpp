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

wxString testscript= _T("[GAME]\
{\
	Mapname=LeoAss.smf;\
	GameType=XTAPEV944.sdz;\
	ModHash=884413695;\
	diminishingmms=0;\
	ghostedbuildings=1;\
	limitdgun=0;\
	gamemode=0;\
	maxunits=500;\
	startenergy=1000;\
	startmetal=1000;\
	startpostype=3;\
	HostIP=localhost;\
	HostPort=8452;\
	MyPlayerNum=0;\
	NumPlayers=1;\
	NumTeams=2;\
	NumAllyTeams=2;\
	[PLAYER0]\
	{\
		name=Player;\
		Spectator=0;\
		team=0;\
	}\
}\
");

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
    wxString script = GetScriptFromReplay( ReplayPath );
    ret.battle = GetBattleFromScript( testscript );
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

        return script;
    }
    catch (...)
    {
        return wxEmptyString;
    }

}

//BattleOptions GetBattleOptsFromScript( const wxString& script_ )
OfflineBattle GetBattleFromScript( const wxString& script_ )
{
    OfflineBattle battle;
    BattleOptions opts;
    std::stringstream ss ( (const char *)script_.mb_str(wxConvUTF8) );// no need to convert wxstring-->std::string-->std::stringstream, convert directly.
    PDataList script( ParseTDF(ss) );
    wxString dump_str;
    TDFWriter dumper(dump_str);
    script->Save(dumper);
    wxLogMessage(_T("tdf dump='%s'"),dump_str.c_str());


    PDataList replayNode ( script->Find(_T("GAME") ) );
    if ( replayNode.ok() ){

        opts.modname    = replayNode->GetString( _T("GameType") );
        opts.modhash    = replayNode->GetString( _T("ModHash") );
        opts.mapname    = replayNode->GetString( _T("Mapname") );
        opts.ip         = replayNode->GetString( _T("HostIP") );
        opts.port       = replayNode->GetInt  ( _T("HostPort"), DEFAULT_EXTERNAL_UDP_SOURCE_PORT );

        int playernum = replayNode->GetInt  ( _T("NumPlayers"), 1);
        int allynum = replayNode->GetInt  ( _T("NumAllyTeams"), 1);
        int teamnum = replayNode->GetInt  ( _T("NumTeams"), 1);

        for ( int i = 0; i < playernum ; ++i ){
            PDataList player ( script->Find(_T("PLAYER") + i2s(i) ) );
            if ( player.ok() ) {
                OfflineUser user ( player->GetString( _T("name") ), wxEmptyString, 0);
                UserBattleStatus status;
                //how to convert back?
                //status.side = player->GetString( _T("side") );
                status.spectator = player->GetInt( _T("Spectator"), 0 );
                status.team = player->GetInt( _T("team") );

                user.UpdateBattleStatus( status );
                battle.AddUser( user );
            }
        }
        opts.maxplayers = playernum ;

    }
    battle.SetBattleOptions( opts );
    return battle;
}
