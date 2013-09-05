/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/spring/spring.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


//
// Class: Spring
//

#ifdef _MSC_VER
#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

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
#include "mainwindow.h"
#include "utils/customdialogs.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "settings.h"
#include "userlist.h"
#include "battle.h"
#include "singleplayerbattle.h"
#include "offlinebattle.h"
#include "user.h"
#include "nonportable.h"
#include "tdfcontainer.h"
#include <lslutils/globalsmanager.h>

#include <lslutils/conversion.h>

BEGIN_EVENT_TABLE( Spring, wxEvtHandler )

	EVT_COMMAND ( PROC_SPRING, wxEVT_SPRING_EXIT, Spring::OnTerminated )

END_EVENT_TABLE()

#define FIRST_UDP_SOURCEPORT 8300

Spring& spring()
{
    static LSL::Util::LineInfo<Spring> m( AT );
	static LSL::Util::GlobalObjectHolder<Spring, LSL::Util::LineInfo<Spring> > m_spring( m );
	return m_spring;
}

Spring::Spring() :
	wxEvtHandler(),
        m_process(0),
        m_wx_process(0),
        m_running(false)
{ }


Spring::~Spring()
{
    delete m_process;
}


bool Spring::IsRunning() const
{
	return m_running;
}

bool Spring::RunReplay ( const wxString& filename )
{
  wxLogMessage( _T("launching spring with replay: ") + filename );

  return LaunchSpring( _T("\"") + filename + _T("\"") );
}

bool Spring::Run( Battle& battle )
{

  wxString path = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("script.txt");

  try
  {

    if ( !wxFile::Access( path , wxFile::write ) )
    {
      wxLogError( _T("Access denied to script.txt.") );
    }

    wxFile f( path, wxFile::write );
    battle.DisableHostStatusInProxyMode( true );
    f.Write( WriteScriptTxt(battle) );
    battle.DisableHostStatusInProxyMode( false );
    f.Close();

  }
    catch ( std::exception& e )
    {
        wxLogError( wxString::Format( _T("Couldn't write script.txt, exception caught:\n %s"), TowxString( e.what() ).c_str() ) );
        return false;
    }

  catch (...)
  {
    wxLogError( _T("Couldn't write script.txt") );
    return false;
  }

  #if 0 //TODO: BD, isn't this SUPER obsolete
  wxString CommandForAutomaticTeamSpeak = _T("SCRIPT|") + battle.GetFounder().GetNick() + _T("|");
  for ( UserList::user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ )
  {
    CommandForAutomaticTeamSpeak << TowxString<unsigned int>( battle.GetUser(i).BattleStatus().ally) << _T("|") << battle.GetUser(i).GetNick() << _T("|");
  }
//  torrent().SendMessageToCoordinator(CommandForAutomaticTeamSpeak); //this is gone too now, right?
  #endif

	wxString cmd;
	if ( battle.GetAutoHost().GetEnabled() )
	{
    // -m, --minimise          Start minimised
    // -q [T], --quit=[T]      Quit immediately on game over or after T seconds
	cmd = _T("--minimise");
	}
	cmd += _T(" \"") + path +  _T("\"");

	return LaunchSpring( cmd );
}


bool Spring::Run( SinglePlayerBattle& battle )
{

  wxString path = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator() + _T("script.txt");

  try
  {

    if ( !wxFile::Access( path, wxFile::write ) )
    {
      wxLogError( _T("Access denied to script.txt.") );
    }

    wxFile f( path, wxFile::write );
    f.Write( WriteScriptTxt(battle) );
    f.Close();

  }
  catch ( std::exception& e )
  {
    wxLogError( wxString::Format( _T("Couldn't write script.txt, exception caught:\n %s"), TowxString( e.what() ).c_str() ) );
    return false;
  }
  catch (...)
  {
    wxLogError( _T("Couldn't write script.txt") );
    return false;
  }

  return LaunchSpring( _T("\"") + path + _T("\"") );
}

bool Spring::Run( OfflineBattle& battle )
{

  wxString path = battle.GetPlayBackFilePath();

  return LaunchSpring( _T("\"") + path + _T("\"") );
}


bool Spring::LaunchSpring( const wxString& params  )
{
  if ( m_running )
  {
    wxLogError( _T("Spring already running!") );
    return false;
  }
    if ( !wxFile::Exists( sett().GetCurrentUsedSpringBinary() ) ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _T("The spring executable was not found at the set location, please re-check."), _T("Executable not found") );
        ui().mw().ShowConfigure( MainWindow::OPT_PAGE_SPRING );
        return false;
    }

  wxString configfileflags = sett().GetCurrentUsedSpringConfigFilePath();
  if ( !configfileflags.IsEmpty() )
  {

		configfileflags = _T("--config=\"") + configfileflags + _T("\" ");
		#ifdef __WXMSW__
      if ( LSL::usync().GetSpringVersion().find("0.78.") != std::string::npos ) configfileflags = _T("");
		#endif
  }

  wxString cmd =  _T("\"") + sett().GetCurrentUsedSpringBinary();
  #ifdef __WXMAC__
    wxChar sep = wxFileName::GetPathSeparator();
	if ( sett().GetCurrentUsedSpringBinary().AfterLast(_T('.')) == _T("app") )
        cmd += sep + wxString(_T("Contents")) + sep + wxString(_T("MacOS")) + sep + wxString(_T("spring")); // append app bundle inner path
  #endif
  cmd += _T("\" ") + configfileflags + params;

  wxLogMessage( _T("spring call params: %s"), cmd.c_str() );

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
	GlobalEvent::Send(GlobalEvent::OnSpringStarted);
  return true;
}

void Spring::OnTerminated( wxCommandEvent& event )
{
    wxLogDebugFunc( _T("") );
    m_running = false;
    m_process = 0; // NOTE I'm not sure if this should be deleted or not, according to wx docs it shouldn't.
    m_wx_process = 0;
	event.SetEventType(GlobalEvent::OnSpringTerminated);
	GlobalEvent::Send(event);
}


wxString Spring::WriteScriptTxt( IBattle& battle ) const
{
    wxLogMessage(_T("0 WriteScriptTxt called "));

    wxString ret;

    TDFWriter tdf(ret);

    // Start generating the script.
    tdf.EnterSection( _T("GAME") );

		tdf.Append( _T("HostIP"), battle.GetHostIp() );
			if ( battle.IsFounderMe() )
			{
					if ( battle.GetNatType() == NAT_Hole_punching ) tdf.Append( _T("HostPort"), battle.GetMyInternalUdpSourcePort() );
					else tdf.Append(_T("HostPort"), battle.GetHostPort() );
			}
			else
			{
					tdf.Append( _T("HostPort"), battle.GetHostPort() );
					if ( battle.GetNatType() == NAT_Hole_punching )
					{
						tdf.Append( _T("SourcePort"), battle.GetMyInternalUdpSourcePort() );
					}
					else if ( sett().GetClientPort() != 0)
					{
						tdf.Append( _T("SourcePort"), sett().GetClientPort() ); /// this allows to play with broken router by setting SourcePort to some forwarded port.
					}
			}
			tdf.Append( _T("IsHost"), battle.IsFounderMe() );

			User& me = battle.GetMe();
			tdf.Append(_T("MyPlayerName"), me.GetNick() );

			if ( !me.BattleStatus().scriptPassword.IsEmpty() )
			{
				tdf.Append( _T("MyPasswd"), me.BattleStatus().scriptPassword );
			}

			if ( !battle.IsFounderMe() )
			{
					tdf.LeaveSection();
					return ret;
			}

			/**********************************************************************************
																		Host-only section
			**********************************************************************************/

			tdf.AppendLineBreak();

			tdf.Append(_T("ModHash"), battle.LoadMod().hash );
			tdf.Append(_T("MapHash"), battle.LoadMap().hash );

			tdf.Append( _T("Mapname"), battle.GetHostMapName() );
			tdf.Append( _T("GameType"), battle.GetHostModName() );

			tdf.AppendLineBreak();

			switch ( battle.GetBattleType() )
			{
				case BT_Played: break;
				case BT_Replay:
				{
					wxString path = battle.GetPlayBackFilePath();
					if ( path.Find(_T("/")) != wxNOT_FOUND ) path.BeforeLast(_T('/'));
					tdf.Append( _T("DemoFile"), path );
					tdf.AppendLineBreak();
					break;
				}
				case BT_Savegame:
				{
					wxString path = battle.GetPlayBackFilePath();
					if ( path.Find(_T("/")) != wxNOT_FOUND ) path.BeforeLast(_T('/'));
					tdf.Append( _T("Savefile"), path );
					tdf.AppendLineBreak();
					break;
				}
				default:
                    wxLogDebugFunc( _T("") ); break;
			}

            const long startpostype = LSL::Util::FromString<long>(
                battle.CustomBattleOptions().getSingleValue("startpostype", LSL::OptionsWrapper::EngineOption ));

            std::vector<LSL::StartPos> remap_positions;
			if ( battle.IsProxy() && ( startpostype != IBattle::ST_Pick ) && ( startpostype != IBattle::ST_Choose ) )
			{
				std::set<int> parsedteams;
				unsigned int NumUsers = battle.GetNumUsers();
				unsigned int NumTeams = 0;
				for ( unsigned int i = 0; i < NumUsers; i++ )
				{
						User& usr = battle.GetUser( i );
						UserBattleStatus& status = usr.BattleStatus();
						if ( status.spectator ) continue;
						if ( parsedteams.find( status.team ) != parsedteams.end() ) continue; // skip duplicates
						parsedteams.insert( status.team );
						NumTeams++;
				}

                LSL::MapInfo infos = battle.LoadMap().info;
				unsigned int nummapstartpositions = infos.positions.size();
				unsigned int copysize = std::min( nummapstartpositions, NumTeams );
                remap_positions = std::vector<LSL::StartPos> ( infos.positions.begin(), infos.positions.begin() + copysize ); // only add the first x positions

				if ( startpostype == IBattle::ST_Random )
				{
                    std::random_shuffle( remap_positions.begin(), remap_positions.end() ); // shuffle the positions
				}

			}
			if ( battle.IsProxy() )
			{
				if ( ( startpostype == IBattle::ST_Random ) || ( startpostype == IBattle::ST_Fixed ) )
				{
					tdf.Append( _T("startpostype"), IBattle::ST_Pick );
				}
				else tdf.Append( _T("startpostype"), startpostype );
			}
			else tdf.Append( _T("startpostype"), startpostype );

            tdf.EnterSection( _T("mapoptions") );
                for (const auto& it : battle.CustomBattleOptions().getOptions( LSL::OptionsWrapper::MapOption ))
				{
                    tdf.Append(TowxString(it.first), TowxString(it.second.second));
				}
			tdf.LeaveSection();


			tdf.EnterSection(_T("modoptions"));
				tdf.Append( _T("relayhoststartpostype"), startpostype ); // also save the original wanted setting
                for (const auto& it : battle.CustomBattleOptions().getOptions( LSL::OptionsWrapper::ModOption ))
				{
                    tdf.Append(TowxString(it.first), TowxString(it.second.second));
				}
			tdf.LeaveSection();

			std::map<wxString,int> units = battle.RestrictedUnits();
			tdf.Append( _T("NumRestrictions"), units.size());
			tdf.EnterSection( _T("RESTRICT") );
				int restrictcount = 0;
				for ( std::map<wxString, int>::const_iterator itor = units.begin(); itor != units.end(); ++itor )
				{
						tdf.Append(_T("Unit") + TowxString( restrictcount ), itor->first );
						tdf.Append(_T("Limit") + TowxString( restrictcount ), itor->second );
						restrictcount++;
				}
			tdf.LeaveSection();


			tdf.AppendLineBreak();

			if ( battle.IsProxy() )
			{
				tdf.Append( _T("NumPlayers"), battle.GetNumPlayers() -1 );
				tdf.Append( _T("NumUsers"), battle.GetNumUsers() -1 );
			}
			else
			{
				tdf.Append( _T("NumPlayers"), battle.GetNumPlayers() );
				tdf.Append( _T("NumUsers"), battle.GetNumUsers() );
			}

			tdf.AppendLineBreak();

			unsigned int NumUsers = battle.GetNumUsers();

			typedef std::map<int, int> ProgressiveTeamsVec;
			typedef ProgressiveTeamsVec::iterator ProgressiveTeamsVecIter;
			ProgressiveTeamsVec teams_to_sorted_teams; // original team -> progressive team
			int free_team = 0;
			std::map<User*, int> player_to_number; // player -> ordernumber
			srand ( time(NULL) );
			for ( unsigned int i = 0; i < NumUsers; i++ )
			{
					User& user = battle.GetUser( i );
					UserBattleStatus& status = user.BattleStatus();
					if ( !status.spectator )
					{
						ProgressiveTeamsVecIter itor = teams_to_sorted_teams.find ( status.team );
						if ( itor == teams_to_sorted_teams.end() )
						{
							teams_to_sorted_teams[status.team] = free_team;
							free_team++;
						}
					}
				  if ( battle.IsProxy() && ( user.GetNick() == battle.GetFounder().GetNick() ) ) continue;
					if ( status.IsBot() ) continue;
					tdf.EnterSection( _T("PLAYER") + TowxString( i ) );
							tdf.Append( _T("Name"), user.GetNick() );
							tdf.Append( _T("CountryCode"), user.GetCountry().Lower());
							tdf.Append( _T("Spectator"), status.spectator );
							tdf.Append( _T("Rank"), (int)user.GetRank() );
							tdf.Append( _T("IsFromDemo"), int(status.isfromdemo) );
							if ( !status.scriptPassword.IsEmpty() )
							{
								tdf.Append( _T("Password"), status.scriptPassword );
							}
							if ( !status.spectator )
							{
								tdf.Append( _T("Team"), teams_to_sorted_teams[status.team] );
							}
							else
							{
								int speccteam = 0;
								if ( teams_to_sorted_teams.size() != 0 ) speccteam = rand() % teams_to_sorted_teams.size();
								tdf.Append( _T("Team"), speccteam );
							}
					tdf.LeaveSection();
					player_to_number[&user] = i;
			}
            if ( LSL::usync().VersionSupports( LSL::USYNC_GetSkirmishAI ) )
			{
				for ( unsigned int i = 0; i < NumUsers; i++ )
				{
						User& user = battle.GetUser( i );
						UserBattleStatus& status = user.BattleStatus();
						if ( !status.IsBot() ) continue;
						tdf.EnterSection( _T("AI") + TowxString( i ) );
								tdf.Append( _T("Name"), user.GetNick() ); // AI's nick;
								tdf.Append( _T("ShortName"), status.aishortname ); // AI libtype
								tdf.Append( _T("Version"), status.aiversion ); // AI libtype version
								tdf.Append( _T("Team"), teams_to_sorted_teams[status.team] );
								tdf.Append( _T("IsFromDemo"), int(status.isfromdemo) );
								tdf.Append( _T("Host"), player_to_number[&battle.GetUser( status.owner )] );
								tdf.EnterSection( _T("Options") );
                                    int optionmapindex = battle.CustomBattleOptions().GetAIOptionIndex(STD_STRING(user.GetNick()));
									if ( optionmapindex > 0 )
                                    {
                                        for (const auto& it : battle.CustomBattleOptions().getOptions((LSL::OptionsWrapper::GameOption)optionmapindex ))
										{
                                            tdf.Append(TowxString(it.first), TowxString(it.second.second));
										}
									}
								tdf.LeaveSection();
						tdf.LeaveSection();
						player_to_number[&user] = i;
				}
			}

			tdf.AppendLineBreak();

			std::set<int> parsedteams;
            const auto sides = LSL::usync().GetSides(STD_STRING(battle.GetHostModName()));
			for ( unsigned int i = 0; i < NumUsers; i++ )
			{
					User& usr = battle.GetUser( i );
					UserBattleStatus& status = usr.BattleStatus();
					if ( status.spectator ) continue;
					if ( parsedteams.find( status.team ) != parsedteams.end() ) continue; // skip duplicates
					parsedteams.insert( status.team );

					tdf.EnterSection( _T("TEAM") + TowxString( teams_to_sorted_teams[status.team] ) );
                        if ( !LSL::usync().VersionSupports(LSL::USYNC_GetSkirmishAI ) && status.IsBot() )
						{
								tdf.Append( _T("AIDLL"), status.aishortname );
								tdf.Append( _T("TeamLeader"), player_to_number[&battle.GetUser( status.owner )] ); // bot owner is the team leader
						}
						else
						{
								if ( status.IsBot() )
								{
										tdf.Append( _T("TeamLeader"), player_to_number[&battle.GetUser( status.owner )] );
								}
								else
								{
										tdf.Append( _T("TeamLeader"), player_to_number[&usr] );
								}
						}
						if ( battle.IsProxy() )
						{
							if ( startpostype == IBattle::ST_Pick )
							{
									tdf.Append(_T("StartPosX"), status.pos.x );
									tdf.Append(_T("StartPosZ"), status.pos.y );
							}
							else if ( ( startpostype == IBattle::ST_Fixed ) || ( startpostype == IBattle::ST_Random ) )
							{
									int teamnumber = teams_to_sorted_teams[status.team];
									if ( teamnumber < int(remap_positions.size()) ) // don't overflow
									{
                                        LSL::StartPos position = remap_positions[teamnumber];
										tdf.Append(_T("StartPosX"), position.x );
										tdf.Append(_T("StartPosZ"), position.y );
									}
							}
						}
						else
						{
							if ( startpostype == IBattle::ST_Pick )
							{
									tdf.Append(_T("StartPosX"), status.pos.x );
									tdf.Append(_T("StartPosZ"), status.pos.y );
							}
						}

						tdf.Append( _T("AllyTeam"),status.ally );

						wxString colourstring =
								TowxString( status.colour.Red()/255.0 ) + _T(' ') +
								TowxString( status.colour.Green()/255.0 ) + _T(' ') +
								TowxString( status.colour.Blue()/255.0 );
						tdf.Append( _T("RGBColor"), colourstring);

						unsigned int side = status.side;
                        if ( side < sides.size() ) tdf.Append( _T("Side"), sides[side] );
						tdf.Append( _T("Handicap"), status.handicap );
					tdf.LeaveSection();
			}

			tdf.AppendLineBreak();

			unsigned int maxiter = std::max( NumUsers, battle.GetLastRectIdx() + 1 );
			std::set<int> parsedallys;
			for ( unsigned int i = 0; i < maxiter; i++ )
			{

					User& usr = battle.GetUser( i );
					UserBattleStatus& status = usr.BattleStatus();
					BattleStartRect sr = battle.GetStartRect( i );
					if ( status.spectator && !sr.IsOk() )
						continue;
					int ally = status.ally;
					if ( status.spectator )
						ally = i;
					if ( parsedallys.find( ally ) != parsedallys.end() )
						continue; // skip duplicates
					sr = battle.GetStartRect( ally );
					parsedallys.insert( ally );

					tdf.EnterSection( _T("ALLYTEAM") + TowxString( ally ) );
						tdf.Append( _T("NumAllies"), 0 );
						if ( startpostype == IBattle::ST_Choose )
						{
							if ( sr.IsOk() )
							{
									const char* old_locale = std::setlocale(LC_NUMERIC, "C");

									tdf.Append( _T("StartRectLeft"), wxFormat( _T("%.3f") ) % ( sr.left / 200.0 ) );
									tdf.Append( _T("StartRectTop"), wxFormat( _T("%.3f") ) % ( sr.top / 200.0 ) );
									tdf.Append( _T("StartRectRight"), wxFormat( _T("%.3f") ) % ( sr.right / 200.0 ) );
									tdf.Append( _T("StartRectBottom"), wxFormat( _T("%.3f") ) % ( sr.bottom / 200.0 ) );

									std::setlocale(LC_NUMERIC, old_locale);
							}
						}
					tdf.LeaveSection();
			}

    tdf.LeaveSection();

    return ret;

}

