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
#include "playbacktab.h"
#include "playbackstructs.h"
#include "../uiutils.h"
#include "../globalsmanager.h"


ReplayList::ReplayList()
{
}

void ReplayList::LoadPlaybacks( const wxArrayString& filenames )
{
    m_fails = 0;

    m_replays.clear();
    size_t size = filenames.GetCount();
    for ( size_t i = 0; i < size; ++i)
    {
        Replay rep;
        rep.id = i;
				Replay& rep_ref = AddReplay( rep ); // don't touch this reference, since elements inside this data structure are filled using pointers, adding & not fecthing the new addresses would screw up references when rep gets destroyed
        if ( !GetReplayInfos( filenames[i] , rep_ref ) )
        {
            RemoveReplay( rep.id );
            m_fails++;
        }
    }
}

bool ReplayList::GetReplayInfos ( const wxString& ReplayPath, Replay& ret )
{
    //wxLogMessage(_T("GetReplayInfos %s"), ReplayPath.c_str());
    //wxLOG_Info  ( STD_STRING( ReplayPath ) );
    //TODO extract moar info
    ret.Filename = ReplayPath;
    ret.battle.SetPlayBackFilePath( ReplayPath );

    wxString FileName = ReplayPath.AfterLast( '/' ); // strips file path
    FileName = FileName.BeforeLast( _T('.') ); //strips the file extension;

    ret.date = FileName.BeforeFirst(_T('_'));
    FileName = FileName.AfterFirst(_T('_'));

    FileName = FileName.AfterFirst(_T('_')); // strips hours minutes seconds informatiom

    ret.SpringVersion = FileName.AfterLast(_T('_'));

    ret.MapName = FileName.BeforeLast(_T('_'));

		wxString script = GetScriptFromReplay( ReplayPath );
    ret.battle.SetScript( script );
    //wxLogMessage(_T("Script: %s"), script.c_str());

    if ( script.IsEmpty() ) return false;

    GetHeaderInfo( ret, ReplayPath );
    GetBattleFromScript( ret.battle, false  );
    ret.ModName = ret.battle.GetHostModName();
    if ( ReplayTraits::IsReplayType ) ret.battle.SetBattleType( BT_Replay );
    else ret.battle.SetBattleType( BT_Savegame );

    return true;
}

wxString ReplayList::GetScriptFromReplay ( const wxString& ReplayPath  )
{
	wxString script;
    try
    {
        wxFile replay( ReplayPath, wxFile::read );
        if ( !replay.IsOpened() ) return script;
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
    }
	return script;
}

//! (koshi) don't delete commented things please, they might be need in the future and i'm lazy
void ReplayList::GetBattleFromScript( IBattle& battle, bool loadmod )
{

    BattleOptions opts;
    std::stringstream ss ( (const char *)battle.GetScript().mb_str(wxConvUTF8) );// no need to convert wxstring-->std::string-->std::stringstream, convert directly.
    PDataList script( ParseTDF(ss) );

    PDataList replayNode ( script->Find(_T("GAME") ) );
    if ( replayNode.ok() )
    {

        wxString modname = replayNode->GetString( _T("GameType") );
        wxString modhash = replayNode->GetString( _T("ModHash") );
        if ( !modhash.IsEmpty() ) modhash = MakeHashUnsigned( modhash );
        battle.SetHostMod( modname, modhash );

        //don't have the maphash, what to do?
        //ui download function works with mapname if hash is empty, so works for now
        wxString mapname    = replayNode->GetString( _T("MapName") );
			  wxString maphash    = replayNode->GetString( _T("MapHash") );
        if ( !maphash.IsEmpty() ) maphash = MakeHashUnsigned( maphash );
        battle.SetHostMap( mapname, maphash );

//        opts.ip         = replayNode->GetString( _T("HostIP") );
//        opts.port       = replayNode->GetInt  ( _T("HostPort"), DEFAULT_EXTERNAL_UDP_SOURCE_PORT );
        opts.spectators = 0;

        int playernum = replayNode->GetInt  ( _T("NumPlayers"), 0);
        int usersnum = replayNode->GetInt  ( _T("NumUsers"), 0);
        if ( usersnum > 0 ) playernum = usersnum;
//        int allynum = replayNode->GetInt  ( _T("NumAllyTeams"), 1);
//        int teamnum = replayNode->GetInt  ( _T("NumTeams"), 1);



        wxArrayString sides;
        if ( loadmod )
        {
        	sides = usync().GetSides( modname );
        }

				IBattle::TeamVec parsed_teams = battle.GetParsedTeamsVec();
				IBattle::AllyVec parsed_allies = battle.GetParsedAlliesVec();

        //[PLAYERX] sections
        for ( int i = 0; i < playernum ; ++i )
        {
            PDataList player ( replayNode->Find( _T("PLAYER") + i2s(i) ) );
            PDataList bot ( replayNode->Find( _T("AI") + i2s(i) ) );
            if ( player.ok() || bot.ok() )
            {
								if ( bot.ok() ) player = bot;
                User user ( player->GetString( _T("Name") ), (player->GetString( _T("CountryCode")).Upper() ), 0);
                user.BattleStatus().isfromdemo = true;
                user.BattleStatus().spectator = player->GetInt( _T("Spectator"), 0 );
                opts.spectators += user.BattleStatus().spectator;
                user.BattleStatus().team = player->GetInt( _T("Team") );
                user.BattleStatus().sync = true;
                user.BattleStatus().ready = true;
                user.Status().rank = (UserStatus::ServerRankContainer)player->GetInt( _T("Rank"), -1 );
                if ( bot.ok() )
                {
                	user.BattleStatus().aishortname = bot->GetString( _T("ShortName" ) );
                	user.BattleStatus().aiversion = bot->GetString( _T("Version" ) );
                	int ownerindex = bot->GetInt( _T("Host" ) );
                	PDataList aiowner ( replayNode->Find( _T("PLAYER") + i2s(ownerindex) ) );
                	if ( aiowner.ok() )
                	{
                		user.BattleStatus().owner = aiowner->GetString( _T("Name") );
                	}
                }

                IBattle::TeamInfoContainer teaminfos = parsed_teams[user.BattleStatus().team];
                if ( !teaminfos.exist )
                {
									PDataList team( replayNode->Find( _T("TEAM") + i2s( user.BattleStatus().team ) ) );
									if ( team.ok() )
									{
											teaminfos.exist = true;
											teaminfos.TeamLeader = team->GetInt( _T("TeamLeader"), 0 );
											teaminfos.StartPosX = team->GetInt( _T("StartPosX"), -1 );
											teaminfos.StartPosY = team->GetInt( _T("StartPosY"), -1 );
											teaminfos.TeamLeader = team->GetInt( _T("AllyTeam"), 0 );
											teaminfos.RGBColor = GetColorFromFloatStrng( team->GetString( _T("RGBColor") ) );
											teaminfos.SideName = team->GetString( _T("Side"), _T("") );
											teaminfos.Handicap = team->GetInt( _T("Handicap"), 0 );
											int sidepos = sides.Index( teaminfos.SideName );
											teaminfos.SideNum = sidepos;
											parsed_teams[ user.BattleStatus().team ] = teaminfos;
									}
                }
                if ( teaminfos.exist )
                {
										user.BattleStatus().ally = teaminfos.AllyTeam;
										user.BattleStatus().pos.x = teaminfos.StartPosX;
										user.BattleStatus().pos.y = teaminfos.StartPosY;
										user.BattleStatus().colour = teaminfos.RGBColor;
										user.BattleStatus().handicap = teaminfos.Handicap;
										if ( teaminfos.SideNum >= 0 ) user.BattleStatus().side = teaminfos.SideNum;
										IBattle::AllyInfoContainer allyinfos = parsed_allies[user.BattleStatus().ally];
										if ( !allyinfos.exist )
										{
												PDataList ally( replayNode->Find( _T("ALLYTEAM") + i2s( user.BattleStatus().ally ) ) );
												if ( ally.ok() )
												{
													allyinfos.exist = true;
													allyinfos.NumAllies = ally->GetInt( _T("NumAllies"), 0 );
													allyinfos.StartRectLeft = ally->GetInt( _T("StartRectLeft"), 0 );
													allyinfos.StartRectTop = ally->GetInt( _T("StartRectTop"), 0 );
													allyinfos.StartRectRight = ally->GetInt( _T("StartRectRight"), 0 );
													allyinfos.StartRectBottom = ally->GetInt( _T("StartRectBottom"), 0 );
													parsed_allies[ user.BattleStatus().ally ] = allyinfos;
													battle.AddStartRect( user.BattleStatus().ally, allyinfos.StartRectTop, allyinfos.StartRectTop, allyinfos.StartRectRight, allyinfos.StartRectBottom );
												}
										}
                }

                battle.AddUserFromDemo( user );
            }

        }
        battle.SetParsedTeamsVec( parsed_teams );
        battle.SetParsedAlliesVec( parsed_allies );

        //MMoptions, this'll fail unless loading map/mod into wrapper first
        if ( loadmod )
        {
					LoadMMOpts( _T("mapoptions"), battle, replayNode );
					LoadMMOpts( _T("modoptions"), battle, replayNode );
        }

        opts.maxplayers = playernum ;

    }
    battle.SetBattleOptions( opts );
}

void ReplayList::LoadMMOpts( const wxString& sectionname, IBattle& battle, const PDataList& node )
{
    PDataList section ( node->Find(sectionname) );
    OptionsWrapper& opts = battle.CustomBattleOptions();
    for ( PNode n = section->First(); n != section->Last(); n = section->Next( n ) )
        opts.setSingleOption( n->Name(), section->GetString( n->Name() ) );
}

void ReplayList::LoadMMOpts( IBattle& battle, const PDataList& node )
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
        // todo: add 2 strings one for date other for time?
        wxString date = dt.FormatISODate()+_T(" ")+dt.FormatISOTime();
        rep.date = date;
    }
    catch (...){ }
}

