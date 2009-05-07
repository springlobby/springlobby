#include "scriptparser.h"

#include "../iunitsync.h"
#include "../tdfcontainer.h"
#include "../ibattle.h"
#include "../uiutils.h"

#include <sstream>
#include <wx/string.h>

void LoadMMOpts( const wxString& sectionname, IBattle& battle, const PDataList& node )
{
    PDataList section ( node->Find(sectionname) );
    OptionsWrapper& opts = battle.CustomBattleOptions();
    for ( PNode n = section->First(); n != section->Last(); n = section->Next( n ) )
        opts.setSingleOption( n->Name(), section->GetString( n->Name() ) );
}

void LoadMMOpts( IBattle& battle, const PDataList& node )
{
    OptionsWrapper& opts = battle.CustomBattleOptions();
    typedef std::map<wxString,wxString> optMap;
    optMap options = opts.getOptionsMap(OptionsWrapper::EngineOption);
    for ( optMap::const_iterator i = options.begin(); i != options.end(); ++i)
        opts.setSingleOption( i->first, node->GetString( i->first, i->second ) );
}

//! (koshi) don't delete commented things please, they might be need in the future and i'm lazy
void GetBattleFromScript( IBattle& battle, bool loadmod )
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
