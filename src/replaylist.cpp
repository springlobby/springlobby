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
        Replay rep;
        if ( GetReplayInfos( filenames[i] , rep ) )
            AddReplay( rep );
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

Replay& ReplayList::GetReplay( int const index ) {
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

bool GetReplayInfos ( const wxString& ReplayPath, Replay& ret )
{
    //wxLOG_Info  ( STD_STRING( ReplayPath ) );
    //TODO extract moar info
    static long r_id = 0;
    ret.Filename = ReplayPath;

    wxString FileName = ReplayPath.AfterLast( '/' ); // strips file path
    FileName = FileName.Left( FileName.Find( _T(".sdf") ) ); //strips the file extension
    FileName = FileName.AfterFirst( _T('-') );

    ret.ReplayName = FileName.AfterLast(_T('-')); // void string if multiple replays wich share previous paramteres aren't present
    FileName = FileName.BeforeLast(_T('-'));
    if ( ret.ReplayName.Contains(_T(".")) ) /// what we just parsed is not a multiple replay but spring version
    {
      ret.SpringVersion = ret.ReplayName;
      ret.ReplayName = _T("");
    }
    else
    {
       ret.SpringVersion = FileName.AfterLast(_T('-'));
       FileName = FileName.BeforeLast(_T('-'));
    }
    ret.MapName = FileName;
    ret.id = r_id;
    wxString script = GetScriptFromReplay( ReplayPath );
    if ( script.IsEmpty() )
        return false;

    GetHeaderInfo( ret, ReplayPath );
    ret.battle = GetBattleFromScript( script );
    ret.ModName = ret.battle.GetHostModName();
    ret.can_watch = ret.battle.ModExists() && ret.battle.MapExists();
    r_id++; //sucessful parsing assumed --> increment id(index)
    return true;
}

wxString GetScriptFromReplay ( const wxString& ReplayPath )
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

    PDataList replayNode ( script->Find(_T("GAME") ) );
    if ( replayNode.ok() ){

        wxString modname = replayNode->GetString( _T("GameType") );
        wxString modhash    = replayNode->GetString( _T("ModHash") );
        battle.SetHostMod( modname, modhash );

        //don't have the maphash, what to do?
        //ui download function works with mapname if hash is empty, so works for now
        opts.mapname    = replayNode->GetString( _T("Mapname") );
        battle.SetHostMap( opts.mapname, wxEmptyString );

        opts.ip         = replayNode->GetString( _T("HostIP") );
        opts.port       = replayNode->GetInt  ( _T("HostPort"), DEFAULT_EXTERNAL_UDP_SOURCE_PORT );
        opts.spectators = 0;

        int playernum = replayNode->GetInt  ( _T("NumPlayers"), 0);
        int allynum = replayNode->GetInt  ( _T("NumAllyTeams"), 1);
        int teamnum = replayNode->GetInt  ( _T("NumTeams"), 1);

        //[PLAYERX] sections
        for ( int i = 0; i < playernum ; ++i ){
            PDataList player ( replayNode->Find( _T("PLAYER") + i2s(i) ) );
            if ( player.ok() ) {
                OfflineUser user ( player->GetString( _T("name") ), wxEmptyString, 0);
                UserBattleStatus status;
                //how to convert back?
                user.SetSideName( player->GetString( _T("side") ) );
                status.spectator = player->GetInt( _T("Spectator"), 0 );
                opts.spectators += status.spectator;
                status.team = player->GetInt( _T("team") );

                user.UpdateBattleStatus( status );
                battle.AddUser( user );
            }
        }

        //MMoptions, this'll fail unless loading map/mod into wrapper first
        LoadMMOpts( _T("mapoptions"), battle, replayNode );
        LoadMMOpts( _T("modoptions"), battle, replayNode );

        opts.maxplayers = playernum ;

    }
    battle.SetBattleOptions( opts );
    return battle;
}

void LoadMMOpts( const wxString& sectionname, OfflineBattle& battle, const PDataList& node )
{
    PDataList section ( node->Find(sectionname) );
    mmOptionsWrapper& opts = battle.CustomBattleOptions();
    for ( PNode n = section->First(); n != section->Last(); n = section->Next( n ) )
        opts.setSingleOption( n->Name(), section->GetString( n->Name() ) );
}

void LoadMMOpts( OfflineBattle& battle, const PDataList& node )
{
    mmOptionsWrapper& opts = battle.CustomBattleOptions();
    typedef std::map<wxString,wxString> optMap;
    optMap options = opts.getOptionsMap(EngineOption);
    for ( optMap::const_iterator i = options.begin(); i != options.end(); ++i)
        opts.setSingleOption( i->first, node->GetString( i->first, i->second ) );
}

void GetHeaderInfo( Replay& rep, const wxString& ReplayPath )
{
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        replay.Seek( 72 );
        int gametime = 0 ;
        replay.Read( &gametime, 4);
        rep.duration = gametime;
        rep.size = replay.Length();
        unsigned long unixtime = 0;
        replay.Seek( 56 );
        replay.Read( &unixtime, 8 );
        wxDateTime dt;
        dt.Set( (time_t) unixtime );
        wxString date = dt.FormatDate();
        rep.date = date;
    }
    catch (...){ }
}
