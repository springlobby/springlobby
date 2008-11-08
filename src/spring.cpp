/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Spring
//

#include <wx/file.h>
#include <wx/intl.h>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fstream>
#include <clocale>

#include "spring.h"
#include "springprocess.h"
#include "ui.h"
#include "utils.h"
#include "settings.h"
#include "userlist.h"
#include "battle.h"
#include "singleplayerbattle.h"
#include "user.h"
#include "iunitsync.h"
#include "nonportable.h"
#include "tdfcontainer.h"
#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif

BEGIN_EVENT_TABLE( Spring, wxEvtHandler )

    EVT_COMMAND ( PROC_SPRING, wxEVT_SPRING_EXIT, Spring::OnTerminated )

END_EVENT_TABLE();

#define FIRST_UDP_SOURCEPORT 8300


Spring::Spring( Ui& ui ) :
        m_ui(ui),
        m_process(0),
        m_wx_process(0),
        m_running(false)
{ }


Spring::~Spring()
{
    if ( m_process != 0 )
        delete m_process;
}


bool Spring::IsRunning()
{
    return m_process != 0;
}

bool Spring::RunReplay ( wxString& filename )
{
    if ( m_running )
    {
        wxLogError( _T("Spring already running!") );
        return false;
    }

    wxLogMessage( _T("launching spring with replay: ") + filename );

    wxString cmd =  _T("\"") + sett().GetCurrentUsedSpringBinary() + _T("\" \"") + filename + _T("\"") ;
    wxLogMessage( _T("cmd: %s"), cmd.c_str() );
    wxSetWorkingDirectory( sett().GetCurrentUsedDataDir() );
    if ( sett().UseOldSpringLaunchMethod() )
    {
        if ( m_wx_process == 0 ) m_wx_process = new wxSpringProcess( *this );
        if ( wxExecute( cmd , wxEXEC_ASYNC, m_wx_process ) == 0 ) return false;
    }
    else
    {
        if ( m_process == 0 ) m_process = new SpringProcess( *this );
        wxLogMessage( _T("m_process->Create();") );
        m_process->Create();
        wxLogMessage( _T("m_process->SetCommand( cmd );") );
        m_process->SetCommand( cmd );
        wxLogMessage( _T("m_process->Run();") );
        m_process->Run();
    }
    m_running = true;
    wxLogMessage( _T("Done running = true") );
    return true;
}

bool Spring::Run( Battle& battle )
{
    if ( m_running )
    {
        wxLogError( _T("Spring already running!") );
        return false;
    }

    wxString path = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator();

    wxLogMessage( _T("Path to script: %sscript.txt"), path.c_str() );

    try
    {

        if ( !wxFile::Access( path +  _T("script.txt"), wxFile::write ) )
        {
            wxLogError( _T("Access denied to script.txt.") );
        }



        wxFile f( path + _T("script.txt"), wxFile::write );
        f.Write( WriteScriptTxt(battle) );
        f.Close();

    }
    catch (...)
    {
        wxLogError( _T("Couldn't write script.txt") );
        return false;
    }

#ifndef NO_TORRENT_SYSTEM
    wxString CommandForAutomaticTeamSpeak = _T("SCRIPT|") + battle.GetMe().GetNick() + _T("|");
    for ( UserList::user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ )
    {
        CommandForAutomaticTeamSpeak << battle.GetUser(i).GetNick() << _T("|") << u2s( battle.GetUser(i).BattleStatus().ally) << _T("|");
    }
    torrent().SendMessageToCoordinator(CommandForAutomaticTeamSpeak);
#endif

    wxString extra = battle.GetAutoHost().GetExtraCommandLineParams();
    wxString cmd =  _T("\"") + sett().GetCurrentUsedSpringBinary() + _T("\" ") + extra + _T(" \"") + path +  _T("script.txt\"");
    wxLogMessage( _T("cmd: %s"), cmd.c_str() );
    wxSetWorkingDirectory( sett().GetCurrentUsedDataDir() );
    if ( sett().UseOldSpringLaunchMethod() )
    {
        if ( m_wx_process == 0 ) m_wx_process = new wxSpringProcess( *this );
        if ( wxExecute( cmd , wxEXEC_ASYNC, m_wx_process ) == 0 ) return false;
    }
    else
    {
        if ( m_process == 0 ) m_process = new SpringProcess( *this );
        wxLogMessage( _T("m_process->Create();") );
        m_process->Create();
        wxLogMessage( _T("m_process->SetCommand( cmd );") );
        m_process->SetCommand( cmd );
        wxLogMessage( _T("m_process->Run();") );
        m_process->Run();
    }
    m_running = true;
    wxLogMessage( _T("Done running = true") );
    return true;
}


bool Spring::Run( SinglePlayerBattle& battle )
{
    if ( m_running )
    {
        wxLogError( _T("Spring already running!") );
        return false;
    }

    wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

    try
    {

        if ( !wxFile::Access( path + _T("script.txt"), wxFile::write ) )
        {
            wxLogError( _T("Access denied to script.txt.") );
        }

        wxFile f( path + _T("script.txt"), wxFile::write );
        f.Write( WriteSPScriptTxt(battle) );
        f.Close();

    }
    catch (...)
    {
        wxLogError( _T("Couldn't write script.txt") );
        return false;
    }

    wxString cmd =  _T("\"") + sett().GetCurrentUsedSpringBinary() + _T("\" \"") + path + _T("script.txt\"");
    wxSetWorkingDirectory( sett().GetCurrentUsedDataDir() );
    if ( sett().UseOldSpringLaunchMethod() )
    {
        if ( m_wx_process == 0 ) m_wx_process = new wxSpringProcess( *this );
        if ( wxExecute( cmd , wxEXEC_ASYNC, m_wx_process ) == 0 ) return false;
    }
    else
    {
        if ( m_process == 0 ) m_process = new SpringProcess( *this );
        m_process->Create();
        m_process->SetCommand( cmd );
        m_process->Run();
    }

    m_running = true;
    return true;
}


void Spring::OnTerminated( wxCommandEvent& event )
{
    wxLogDebugFunc( _T("") );
    m_running = false;
    m_process = 0; // NOTE I'm not sure if this should be deleted or not, according to wx docs it shouldn't.
    m_wx_process = 0;
    m_ui.OnSpringTerminated( true );
}

struct SortingUser
{
    int ValueToSort;
    int OriginalIndex;
    bool IsBot;

    // comparison function for sorting
    bool operator<( const SortingUser b)
    {
        return ( ValueToSort < b.ValueToSort );
    }
};

wxString Spring::WriteScriptTxt( Battle& battle )
{
    wxLogMessage(_T("0 WriteScriptTxt called "));

    wxString ret;

    TDFWriter tdf(ret);

    // Start generating the script.
    tdf.EnterSection( _T("GAME") );

    if ( battle.IsFounderMe() )
    {
        tdf.Append( _T("HostIP"), _T("localhost") );
        if ( battle.GetNatType() == IBattle::NAT_Hole_punching ) tdf.Append( _T("HostPort"), battle.GetMyInternalUdpSourcePort() );
        else tdf.Append(_T("HostPort"), battle.GetHostPort() );

    }
    else
    {
        tdf.Append( _T("HostIP"), battle.GetHostIp() );
        if ( battle.GetNatType() == IBattle::NAT_Hole_punching ) tdf.Append( _T("SourcePort"), battle.GetMyInternalUdpSourcePort() );
    }
    tdf.Append( _T("IsHost"), battle.IsFounderMe() );


    tdf.AppendLineBreak();

    tdf.Append(_T("MyPlayerName"), battle.GetMe().GetNick() );

    if ( !battle.IsFounderMe() )
    {
        tdf.LeaveSection();
        return ret;
    }

    /**********************************************************************************
                                  Host-only section
    **********************************************************************************/

    tdf.Append(_T("Mapname"), battle.GetHostMapName());
    tdf.Append(_T("GameType"), usync().GetModArchive(usync().GetModIndex(battle.GetHostModName())));

    tdf.Append( _T("ModHash"), s2l( battle.LoadMod().hash ) );

    OptionsWrapper::wxStringTripleVec optlistEng = battle.CustomBattleOptions().getOptions( OptionsWrapper::EngineOption );
    for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistEng.begin(); it != optlistEng.end(); ++it)
    {
        tdf.Append(it->first,it->second.second);
    }

    tdf.AppendLineBreak();

    unsigned int NumPlayers = battle.GetNumUsers();
    unsigned int NumBots = battle.GetNumBots();

    std::vector<SortingUser> SortedByTeam;
    std::vector<SortingUser> SortedByAlly;

    for ( unsigned int i = 0; i < NumPlayers; i++ )
    {
        SortingUser UserTeamSort;
        SortingUser UserAllySort;

        UserTeamSort.ValueToSort = battle.GetUser( i ).BattleStatus().team;
        UserAllySort.ValueToSort = battle.GetUser( i ).BattleStatus().ally;

        UserTeamSort.OriginalIndex = i;
        UserAllySort.OriginalIndex = i;

        UserTeamSort.IsBot = false;
        UserAllySort.IsBot = false;

        SortedByTeam.push_back( UserTeamSort );
        SortedByAlly.push_back( UserAllySort );
    }

    for ( unsigned int i = 0; i < NumBots; i++ )
    {
        SortingUser UserTeamSort;
        SortingUser UserAllySort;

        UserTeamSort.ValueToSort = battle.GetBot( i )->bs.team;
        UserAllySort.ValueToSort = battle.GetBot( i )->bs.ally;

        UserTeamSort.OriginalIndex = i;
        UserAllySort.OriginalIndex = i;

        UserTeamSort.IsBot = true;
        UserAllySort.IsBot = true;

        SortedByTeam.push_back( UserTeamSort );
        SortedByAlly.push_back( UserAllySort );
    }

    std::sort( SortedByTeam.begin(), SortedByTeam.end() );
    std::sort( SortedByAlly.begin(), SortedByAlly.end() );

    unsigned int NumTotalPlayersWithBots = SortedByTeam.size();

    for ( unsigned int i = 0; i < NumPlayers; i++ )
    {
        tdf.EnterSection( _T("PLAYER") + i2s( i ) );

        tdf.Append( _T("name"), battle.GetUser( i ).GetNick() );

        tdf.Append( _T("countryCode"),battle.GetUser( i ).GetCountry().Lower());
        tdf.Append( _T("Spectator"), battle.GetUser( i ).BattleStatus().spectator );

        if ( !(battle.GetUser( i ).BattleStatus().spectator) )
        {
            tdf.Append( _T("team"), battle.GetUser( i ).BattleStatus().team );
        }
        tdf.LeaveSection();
    }

    tdf.AppendLineBreak();

    int PreviousTeam = -1;
    for ( int i = 0; i < NumTotalPlayersWithBots; i++ )
    {
        if ( PreviousTeam == SortedByTeam[i].ValueToSort ) continue; // skip duplicates
        PreviousTeam = SortedByTeam[i].ValueToSort;

        tdf.EnterSection( _T("TEAM") + i2s( SortedByTeam[i].ValueToSort ) );

        UserBattleStatus status;
        if ( SortedByTeam[i].IsBot )
        {
            BattleBot usr = *battle.GetBot( SortedByTeam[i].OriginalIndex );
            status = usr.bs;
            tdf.Append( _T("AIDLL"), usr.aidll );
            tdf.Append( _T("TeamLeader"), battle.GetUser( usr.owner ).BattleStatus().team ); // bot owner is the team leader
        }
        else
        {
            User usr = battle.GetUser( SortedByTeam[i].OriginalIndex );
            tdf.Append( _T("TeamLeader"), SortedByTeam[i].OriginalIndex );
        }

        tdf.Append( _T("AllyTeam"), status.ally );

        wxString colourstring =
            TowxString( status.colour.Red()/255.0 ) + _T(' ') +
            TowxString( status.colour.Green()/255.0 ) + _T(' ') +
            TowxString( status.colour.Blue()/255.0 );
        tdf.Append( _T("RGBColor"), colourstring);

        tdf.Append( _T("Side"), usync().GetSideName( battle.GetHostModName(), status.side ) );
        tdf.Append( _T("Handicap"), status.handicap );

        tdf.LeaveSection();
    }

    long startpostype;
    battle.CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ).ToLong( &startpostype );

    int PreviousAlly = -1;
    for ( int i = 0; i < NumTotalPlayersWithBots; i++ )
    {
        if ( PreviousAlly == SortedByTeam[i].ValueToSort ) continue;
        PreviousAlly = SortedByTeam[i].ValueToSort;

        tdf.EnterSection( _T("ALLYTEAM") + i2s( SortedByTeam[i].ValueToSort ) );

        tdf.Append( _T("NumAllies"), 0 );

        if ( startpostype == IBattle::ST_Choose )
        {
            BattleStartRect sr = battle.GetStartRect( SortedByTeam[i].ValueToSort );
            if ( sr.IsOk() )
            {
                const char* old_locale = std::setlocale(LC_NUMERIC, "C");

                tdf.Append( _T("StartRectLeft"), wxString::Format( _T("%.3f"), sr.left / 200.0 ) );
                tdf.Append( _T("StartRectTop"), wxString::Format( _T("%.3f"), sr.top / 200.0 ) );
                tdf.Append( _T("StartRectRight"), wxString::Format( _T("%.3f"), sr.right / 200.0 ) );
                tdf.Append( _T("StartRectBottom"), wxString::Format( _T("%.3f"), sr.bottom / 200.0 ) );

                std::setlocale(LC_NUMERIC, old_locale);
            }
        }

        tdf.LeaveSection();
    }

    wxArrayString units = battle.DisabledUnits();

    tdf.Append( _T("NumRestrictions"), units.GetCount());
    tdf.EnterSection( _T("RESTRICT") );
    for ( unsigned int i = 0; i < units.GetCount(); i++)
    {
        tdf.Append(_T("Unit") + i2s( i ), units[i].c_str() );
        tdf.Append(_T("Limit") + i2s(i), _T("0") );
    }
    tdf.LeaveSection();

    tdf.EnterSection( _T("mapoptions") );

    OptionsWrapper::wxStringTripleVec optlistMap = battle.CustomBattleOptions().getOptions( OptionsWrapper::MapOption );
    for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistMap.begin(); it != optlistMap.end(); ++it)
    {
        tdf.Append(it->first,it->second.second);
    }
    tdf.LeaveSection();


    tdf.EnterSection(_T("modoptions"));

    OptionsWrapper::wxStringTripleVec optlistMod = battle.CustomBattleOptions().getOptions( OptionsWrapper::ModOption );
    for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistMod.begin(); it != optlistMod.end(); ++it)
    {
        tdf.Append(it->first,it->second.second);
    }
    tdf.LeaveSection();


    tdf.LeaveSection();

    return ret;

}


wxString Spring::WriteSPScriptTxt( SinglePlayerBattle& battle )
{
    wxString ret;
    TDFWriter tdf(ret);
    std::vector<int> AllyConv;

    int NumAllys = 0;
    int PlayerTeam = -1;

    long startpostype;
    battle.CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ).ToLong( &startpostype );

    wxLogMessage( _T("StartPosType=%d"), (int)startpostype );


    for ( unsigned int i = 0; i < battle.GetNumBots(); i++ )
    {
        BattleBot* bot;
        bot = battle.GetBot( i );
        ASSERT_LOGIC( bot != 0, _T("bot == 0") );
        if ( bot->aidll == _T("") ) PlayerTeam = i;
        if (bot->bs.ally>int(AllyConv.size())-1)
        {
            AllyConv.resize(bot->bs.ally+1,-1);
        }
        if ( AllyConv[bot->bs.ally] == -1 ) AllyConv[bot->bs.ally] = NumAllys++;
    }

    ASSERT_LOGIC( PlayerTeam != -1, _T("no player found") );

    // Start generating the script.
    //s  = wxString::Format( _T("[GAME]\n{\n") );
    tdf.EnterSection(_T("GAME"));

    tdf.Append(_T("Mapname"),battle.GetHostMapName());

    tdf.Append(_T("GameType"),usync().GetModArchive(usync().GetModIndex(battle.GetHostModName())));

    unsigned long uhash;
    battle.LoadMod().hash.ToULong(&uhash);

    tdf.Append(_T("ModHash"),int(uhash));

    OptionsWrapper::wxStringTripleVec optlistEng = battle.CustomBattleOptions().getOptions( OptionsWrapper::EngineOption );
    for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistEng.begin(); it != optlistEng.end(); ++it)
    {
        tdf.Append(it->first, it->second.second);
    }

    tdf.Append(_T("HostIP"),"localhost");

    tdf.Append(_T("HostPort"),"8452");

    tdf.Append(_T("MyPlayerName"), _T("Player") );

    tdf.Append(_T("IsHost"), battle.IsFounderMe());

    tdf.EnterSection(_T("PLAYER0"));
    tdf.Append(_T("name"),"Player");
    tdf.Append(_T("Spectator"),"0");
    tdf.Append(_T("team"),PlayerTeam);
    tdf.LeaveSection();

    for ( unsigned int i = 0; i < battle.GetNumBots(); i++ )
    {
        BattleBot* bot;
        if ( startpostype == IBattle::ST_Pick) bot = battle.GetBot( i );
        else bot = battle.GetBotByStartPosition( i );
        ASSERT_LOGIC( bot != 0, _T("bot == 0") );

        tdf.EnterSection(_T("TEAM")+i2s(i));

        if ( startpostype == IBattle::ST_Pick )
        {
            tdf.Append(_T("StartPosX"),bot->posx);
            tdf.Append(_T("StartPosZ"),bot->posy);
        }

        tdf.Append(_T("TeamLeader"),"0");
        tdf.Append(_T("AllyTeam"),AllyConv[bot->bs.ally]);

        wxString colourstring =
            TowxString( bot->bs.colour.Red()/255.0f ) + _T(' ') +
            TowxString( bot->bs.colour.Green()/255.0f ) + _T(' ') +
            TowxString( bot->bs.colour.Blue()/255.0f );
        tdf.Append(_T("RGBColor"), colourstring);

        tdf.Append(_T("Side"),usync().GetSideName(battle.GetHostModName(), bot->bs.side));

        tdf.Append(_T("Handicap"),bot->bs.handicap);

        if ( bot->aidll != _T("") )
        {
            wxString ai = bot->aidll;
            tdf.Append(_T("AIDLL"),ai);
        }
        tdf.LeaveSection();
    }

    for ( int i = 0; i < NumAllys; i++ )
    {
        tdf.EnterSection(_T("ALLYTEAM")+i2s(i));
        tdf.Append(_T("NumAllies"),"0");
        tdf.LeaveSection();
    }
    wxArrayString units = battle.DisabledUnits();
    tdf.Append(_T("NumRestrictions"),units.GetCount());

    tdf.EnterSection(_T("RESTRICT"));

    for ( unsigned int i = 0; i < units.GetCount(); i++)
    {
        tdf.Append(_T("Unit")+i2s(i),units[i].c_str());
        tdf.Append(_T("Limit")+i2s(i),_T("0"));
    }
    tdf.LeaveSection();

    tdf.EnterSection(_T("mapoptions"));
    OptionsWrapper::wxStringTripleVec optlistMap = battle.CustomBattleOptions().getOptions( OptionsWrapper::MapOption );
    for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistMap.begin(); it != optlistMap.end(); ++it)
    {
        tdf.Append(it->first, it->second.second);
    }
    tdf.LeaveSection();

    tdf.EnterSection(_T("modoptions"));
    OptionsWrapper::wxStringTripleVec optlistMod = battle.CustomBattleOptions().getOptions( OptionsWrapper::ModOption );
    for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistMod.begin(); it != optlistMod.end(); ++it)
    {
        tdf.Append(it->first, it->second.second);
    }
    tdf.LeaveSection();

    tdf.LeaveSection();

    return ret;
}
