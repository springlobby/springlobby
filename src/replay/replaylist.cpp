/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <wx/file.h>
#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/filefn.h>
#include <wx/log.h>

#include "replaylist.h"
#include <sstream>
#include "../iunitsync.h"
#include "../utils.h"
#include "../settings++/custom_dialogs.h"
#include "../tdfcontainer.h"
#include "replaytab.h"

const unsigned int replay_bulk_limit = 300;
const unsigned int replay_chunk_size = 50;
const unsigned int timer_interval = 300; //miliseconds

BEGIN_EVENT_TABLE(ReplayList,wxEvtHandler)
    	EVT_TIMER(wxID_ANY, ReplayList::OnTimer)
END_EVENT_TABLE()

ReplayList::ReplayList(ReplayTab& replay_tab)
    : m_timer(this,wxID_ANY),m_replay_tab(replay_tab),m_last_id(0)
{
}

void ReplayList::LoadReplays()
{
    m_fails = 0;
    if ( !usync().IsLoaded() ) return ;
    m_filenames.clear();
    usync().GetReplayList(m_filenames);
    m_replays.clear();

    m_timer.Stop();
    size_t size = m_filenames.size();

    if ( size < replay_bulk_limit )
        LoadReplays( 0, m_filenames.size() );
    else {
        LoadReplays( 0, replay_chunk_size );
        m_current_parse_pos = replay_chunk_size;
        m_timer.Start( timer_interval, wxTIMER_CONTINUOUS );
    }
//
//    std::cout<< "********" << std::endl;
//    for (int i = 0; i< m_filenames.size(); ++i)
//        std::cout<< STD_STRING(m_filenames[i])<< '\n';
//
//    std::cout<< "********" << std::endl;

}

void ReplayList::LoadReplays( const unsigned int from, const unsigned int to)
{
    static long replays_load_count=0;
    wxLogMessage(_T("ReplayList::LoadReplays(%d,%d) call #%d"),from,to,replays_load_count);
    unsigned int end=std::min((unsigned int)to, (unsigned int)m_filenames.size());
    for (unsigned int i = from; i < end; ++i)
    {
        Replay rep;
        rep.id = m_last_id;
        m_last_id++;
        AddReplay( rep );
        if ( GetReplayInfos( m_filenames[i] , m_replays[rep.id] ) )
        {
            m_replay_tab.AddReplay( m_replays[rep.id] );
        }
        else
        {
        	RemoveReplay( rep.id );
					m_last_id--;
        }
    }
    wxLogMessage(_T("done ReplayList::LoadReplays(%d,%d) %d"),from,to,replays_load_count);
    replays_load_count+=1;
}

void ReplayList::OnTimer(wxTimerEvent& event)
{
    if ( replay_chunk_size + m_current_parse_pos >  m_filenames.size() )
    {
        //final parse run
        m_timer.Stop();
        LoadReplays( m_current_parse_pos, m_filenames.size() );
    }
    else {
        LoadReplays( m_current_parse_pos, m_current_parse_pos + replay_chunk_size );
        m_current_parse_pos += replay_chunk_size;
    }
}

void ReplayList::AddReplay( const Replay& replay )
{
  m_replays[replay.id] = replay;
}


void ReplayList::RemoveReplay( replay_id_t const& id )
{
  m_replays.erase(id);
}

replay_map_t::size_type ReplayList::GetNumReplays()
{
  return m_replays.size();
}

Replay &ReplayList::GetReplayById( replay_id_t const& id )
{
//TODO catch
  replay_iter_t b = m_replays.find(id);
  if (b == m_replays.end())
    throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");
  return b->second;
}
/*
Replay& ReplayList::GetReplay( int const index ) {
//TODO secure index
  replay_iter_t b = m_replays.begin();
  std::advance(b,index);
  if (b == m_replays.end())
    throw std::runtime_error("ReplayList_Iter::GetReplay(): no such replay");
  return b->second;
}
*/
bool ReplayList::ReplayExists( replay_id_t const& id )
{
  return m_replays.find(id) != m_replays.end();
}

bool ReplayList::GetReplayInfos ( const wxString& ReplayPath, Replay& ret )
{
    //wxLogMessage(_T("GetReplayInfos %s"), ReplayPath.c_str());
    //wxLOG_Info  ( STD_STRING( ReplayPath ) );
    //TODO extract moar info
    ret.Filename = ReplayPath;

    wxString FileName = ReplayPath.AfterLast( '/' ); // strips file path
    FileName = FileName.BeforeLast( _T('.') ); //strips the file extension;

    ret.date = FileName.BeforeFirst(_T('_'));
    FileName = FileName.AfterFirst(_T('_'));

    FileName = FileName.AfterFirst(_T('_')); // strips hours minutes seconds informatiom

    ret.SpringVersion = FileName.AfterLast(_T('_'));

    ret.MapName = FileName.BeforeLast(_T('_'));

    wxString script;
    GetScriptFromReplay( ReplayPath,script );

    //wxLogMessage(_T("Script: %s"), script.c_str());

    if ( script.IsEmpty() )
        return false;

    GetHeaderInfo( ret, ReplayPath );
    GetBattleFromScript( script,ret.battle  );
    ret.ModName = ret.battle.GetHostModName();

    return true;
}

void ReplayList::GetScriptFromReplay ( const wxString& ReplayPath, wxString& script  )
{
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        replay.Seek( 20 );
        int headerSize=0 ;
        replay.Read( &headerSize, 4);
        replay.Seek( 64 );
        int scriptSize=0;
        replay.Read( &scriptSize, 4);
        replay.Seek( headerSize );
        std::string script_a(scriptSize,0);
        replay.Read( &script_a[0], scriptSize );
        script = WX_STRING( script_a ) ;//(script_a,scriptSize);

    }
    catch (...)
    {
        return ;
    }
}

//! (koshi) don't delete commented things please, they might be need in the future and i'm lazy
void ReplayList::GetBattleFromScript( const wxString& script_, OfflineBattle& battle )
{

    BattleOptions opts;
    std::stringstream ss ( (const char *)script_.mb_str(wxConvUTF8) );// no need to convert wxstring-->std::string-->std::stringstream, convert directly.
    PDataList script( ParseTDF(ss) );

    PDataList replayNode ( script->Find(_T("GAME") ) );
    if ( replayNode.ok() )
    {

        wxString modname = replayNode->GetString( _T("GameType") );
        wxString modhash    = replayNode->GetString( _T("ModHash") );
        battle.SetHostMod( modname, modhash );

        //don't have the maphash, what to do?
        //ui download function works with mapname if hash is empty, so works for now
        opts.mapname    = replayNode->GetString( _T("Mapname") );
        battle.SetHostMap( opts.mapname, wxEmptyString );

//        opts.ip         = replayNode->GetString( _T("HostIP") );
//        opts.port       = replayNode->GetInt  ( _T("HostPort"), DEFAULT_EXTERNAL_UDP_SOURCE_PORT );
        opts.spectators = 0;

        int playernum = replayNode->GetInt  ( _T("NumPlayers"), 0);
//        int allynum = replayNode->GetInt  ( _T("NumAllyTeams"), 1);
//        int teamnum = replayNode->GetInt  ( _T("NumTeams"), 1);

        //[PLAYERX] sections
        for ( int i = 0; i < playernum ; ++i )
        {
            PDataList player ( replayNode->Find( _T("PLAYER") + i2s(i) ) );
            if ( player.ok() )
            {
                User user ( player->GetString( _T("name") ), (player->GetString( _T("countryCode")).Upper() ), 0);
								wxArrayString sides = usync().GetSides( battle.LoadMod().name );
                user.BattleStatus().side = sides.Index( player->GetString( _T("side") ) );
                user.BattleStatus().spectator = player->GetInt( _T("Spectator"), 0 );
                opts.spectators += user.BattleStatus().spectator;
                user.BattleStatus().team = player->GetInt( _T("team") );

                battle.OnOfflineAddUser( user );
            }
        }

        //MMoptions, this'll fail unless loading map/mod into wrapper first
//        LoadMMOpts( _T("mapoptions"), battle, replayNode );
//        LoadMMOpts( _T("modoptions"), battle, replayNode );

        opts.maxplayers = playernum ;

    }
    battle.SetBattleOptions( opts );
}

void ReplayList::LoadMMOpts( const wxString& sectionname, OfflineBattle& battle, const PDataList& node )
{
    PDataList section ( node->Find(sectionname) );
    OptionsWrapper& opts = battle.CustomBattleOptions();
    for ( PNode n = section->First(); n != section->Last(); n = section->Next( n ) )
        opts.setSingleOption( n->Name(), section->GetString( n->Name() ) );
}

void ReplayList::LoadMMOpts( OfflineBattle& battle, const PDataList& node )
{
    OptionsWrapper& opts = battle.CustomBattleOptions();
    typedef std::map<wxString,wxString> optMap;
    optMap options = opts.getOptionsMap(OptionsWrapper::EngineOption);
    for ( optMap::const_iterator i = options.begin(); i != options.end(); ++i)
        opts.setSingleOption( i->first, node->GetString( i->first, i->second ) );
}

void ReplayList::GetHeaderInfo( Replay& rep, const wxString& ReplayPath )
{
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        replay.Seek( 72 );
        int gametime = 0 ;
        replay.Read( &gametime, 4);
        rep.duration = gametime;
        rep.size = replay.Length();
		wxLongLong_t unixtime = 0;
        replay.Seek( 56 );
        replay.Read( &unixtime, 8 );
        wxDateTime dt;
        dt.Set( (time_t) unixtime );
        /// todo: add 2 strings one for date other for time?
        wxString date = dt.FormatISODate()+_T(" ")+dt.FormatISOTime();
        rep.date = date;
    }
    catch (...){ }
}

bool ReplayList::DeleteReplay( replay_id_t const& id )
{
    Replay rep = m_replays[id];
    if ( wxRemoveFile( rep.Filename ) ) {

        m_filenames.resize(std::remove(m_filenames.begin(), m_filenames.end(), rep.Filename)-m_filenames.begin());

        m_replays.erase(id);
        return true;
    }
    return false;
}

void ReplayList::RemoveAll()
{
    m_filenames.clear();
    m_replays.clear();
    m_last_id = 0;
    m_replay_tab.RemoveAllReplays();
}


replay_map_t &ReplayList::GetReplaysMap(){
  return m_replays;
}
