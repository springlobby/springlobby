/* This file is part of the Springlobby (GPL v2 or later), see COPYING */


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

#include <wx/filename.h>
#include <stdexcept>
#include <vector>
#include <clocale>
#include <fstream>

#include "spring.h"
#include "springprocess.h"
#include "ui.h"
#include "mainwindow.h"
#include "utils/customdialogs.h"
#include "utils/debug.h"
#include "utils/conversion.h"
#include "utils/slpaths.h"
#include "settings.h"
#include "ibattle.h"

#include <lsl/battle/tdfcontainer.h>
#include <lslutils/globalsmanager.h>
#include <lslutils/conversion.h>
#include <lslunitsync/unitsync.h>

SLCONFIG("/Spring/Safemode", false, "launch spring in safemode");

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
	m_running(false)
{ }


Spring::~Spring()
{
	m_process=NULL;
}


bool Spring::IsRunning() const
{
	return m_running;
}

bool Spring::Run( IBattle& battle )
{

	wxString scripttxt = SlPaths::GetDataDir() + wxFileName::GetPathSeparator() + _T("script.txt");

	try {

		if ( !wxFile::Access( scripttxt , wxFile::write ) ) {
			wxLogError( _T("Access denied to script.txt.") );
		}

		wxFile f( scripttxt, wxFile::write );
		battle.DisableHostStatusInProxyMode( true );
		f.Write( WriteScriptTxt(battle) );
		battle.DisableHostStatusInProxyMode( false );
		f.Close();

	} catch ( std::exception& e ) {
		wxLogError( wxString::Format( _T("Couldn't write script.txt, exception caught:\n %s"), TowxString( e.what() ).c_str() ) );
		return false;
	}

	catch (...) {
		wxLogError( _T("Couldn't write script.txt") );
		return false;
	}

	const wxString executable = SlPaths::GetSpringBinary(battle.GetEngineVersion());
	if ( !wxFile::Exists(executable) ) {
		customMessageBoxNoModal( SL_MAIN_ICON, _T("The spring executable was not found at the set location, please re-check."), _T("Executable not found") );
		ui().mw().ShowConfigure( MainWindow::OPT_PAGE_SPRING );
		return false;
	}

	wxArrayString params;
	params.push_back(scripttxt);

	return LaunchEngine(executable, params);
}

bool Spring::LaunchEngine(const wxString& cmd, wxArrayString& params)
{
	if ( m_running ) {
		wxLogError( _T("Spring already running!") );
		return false;
	}

	if (cfg().ReadBool(_T( "/Spring/Safemode" ))) {
		params.push_back(_T("--safemode "));
	}
	wxLogMessage( _T("spring call params: %s"), cmd.c_str() );
	wxSetWorkingDirectory( SlPaths::GetDataDir() );

	if ( m_process == 0 ) {
		m_process = new SpringProcess( *this );
	}
	m_process->Create();
	m_process->SetCommand( cmd, params );
	m_process->Run();
	m_running = true;
	GlobalEvent::Send(GlobalEvent::OnSpringStarted);
	return true;
}

void Spring::OnTerminated( wxCommandEvent& event )
{
	wxLogDebugFunc( wxEmptyString );
	m_running = false;
	m_process = NULL;
	event.SetEventType(GlobalEvent::OnSpringTerminated);
	GlobalEvent::Send(event);
}

wxString Spring::WriteScriptTxt( IBattle& battle ) const
{
	wxLogMessage(_T("0 WriteScriptTxt called "));

	std::stringstream ret;

	LSL::TDF::TDFWriter tdf(ret);

	// Start generating the script.
	tdf.EnterSection("GAME");

	if ( battle.IsFounderMe() ) {
		tdf.Append("HostIP", ""); //Listen on all addresses for connections when hosting
		if ( battle.GetNatType() == NAT_Hole_punching ) tdf.Append("HostPort", battle.GetMyInternalUdpSourcePort() );
		else tdf.Append("HostPort", battle.GetHostPort() );
	} else {
		tdf.Append("HostIP", STD_STRING(battle.GetHostIp()) );
		tdf.Append("HostPort", battle.GetHostPort() );
		if ( battle.GetNatType() == NAT_Hole_punching ) {
			tdf.Append("SourcePort", battle.GetMyInternalUdpSourcePort() );
		} else if ( sett().GetClientPort() != 0) {
			tdf.Append("SourcePort", sett().GetClientPort() ); /// this allows to play with broken router by setting SourcePort to some forwarded port.
		}
	}
	tdf.Append("IsHost", battle.IsFounderMe() );

	User& me = battle.GetMe();
	tdf.Append("MyPlayerName", STD_STRING(me.GetNick()));

	if ( !me.BattleStatus().scriptPassword.empty() ) {
		tdf.Append("MyPasswd", me.BattleStatus().scriptPassword);
	}

	if ( !battle.IsFounderMe() ) {
		tdf.LeaveSection();
		return TowxString(ret.str());
	}

	/**********************************************************************************
																Host-only section
	**********************************************************************************/

	tdf.AppendLineBreak();

	tdf.Append("ModHash", battle.LoadMod().hash);
	tdf.Append("MapHash", battle.LoadMap().hash);

	tdf.Append("Mapname", STD_STRING(battle.GetHostMapName()));
	tdf.Append("GameType", STD_STRING(battle.GetHostModName()));

	tdf.AppendLineBreak();

	switch ( battle.GetBattleType() ) {
	case BT_Played:
		break;
	case BT_Replay: {
		wxString path = battle.GetPlayBackFilePath();
		if ( path.Find(_T("/")) != wxNOT_FOUND ) path.BeforeLast(_T('/'));
		tdf.Append("DemoFile", STD_STRING(path));
		tdf.AppendLineBreak();
		break;
	}
	case BT_Savegame: {
		wxString path = battle.GetPlayBackFilePath();
		if ( path.Find(_T("/")) != wxNOT_FOUND ) path.BeforeLast(_T('/'));
		tdf.Append("Savefile", STD_STRING(path));
		tdf.AppendLineBreak();
		break;
	}
	default:
		wxLogDebugFunc( wxEmptyString );
		break;
	}

	const long startpostype = LSL::Util::FromString<long>(
					  battle.CustomBattleOptions().getSingleValue("startpostype", LSL::OptionsWrapper::EngineOption ));

	std::vector<LSL::StartPos> remap_positions;
	if ( battle.IsProxy() && ( startpostype != IBattle::ST_Pick ) && ( startpostype != IBattle::ST_Choose ) ) {
		std::set<int> parsedteams;
		unsigned int NumUsers = battle.GetNumUsers();
		unsigned int NumTeams = 0;
		for ( unsigned int i = 0; i < NumUsers; i++ ) {
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

		if ( startpostype == IBattle::ST_Random ) {
			std::random_shuffle( remap_positions.begin(), remap_positions.end() ); // shuffle the positions
		}

	}
	if ( battle.IsProxy() ) {
		if ( ( startpostype == IBattle::ST_Random ) || ( startpostype == IBattle::ST_Fixed ) ) {
			tdf.Append("startpostype", IBattle::ST_Pick );
		} else tdf.Append("startpostype", startpostype );
	} else tdf.Append("startpostype", startpostype );

	tdf.EnterSection("mapoptions");
	for (const auto& it : battle.CustomBattleOptions().getOptions( LSL::OptionsWrapper::MapOption )) {
		tdf.Append(it.first, it.second.second);
	}
	tdf.LeaveSection();


	tdf.EnterSection("modoptions");
	tdf.Append("relayhoststartpostype", startpostype ); // also save the original wanted setting
	for (const auto& it : battle.CustomBattleOptions().getOptions( LSL::OptionsWrapper::ModOption )) {
		tdf.Append(it.first, it.second.second);
	}
	tdf.LeaveSection();

	std::map<wxString,int> units = battle.RestrictedUnits();
	tdf.Append("NumRestrictions", units.size());
	tdf.EnterSection("RESTRICT");
	int restrictcount = 0;
	for ( std::map<wxString, int>::const_iterator itor = units.begin(); itor != units.end(); ++itor ) {
		tdf.Append(stdprintf("Unit%d", restrictcount), itor->first );
		tdf.Append(stdprintf("Limit%d", restrictcount), itor->second );
		restrictcount++;
	}
	tdf.LeaveSection();


	tdf.AppendLineBreak();

	if ( battle.IsProxy() ) {
		tdf.Append("NumPlayers", battle.GetNumPlayers() -1 );
		tdf.Append("NumUsers", battle.GetNumUsers() -1 );
	} else {
		tdf.Append("NumPlayers", battle.GetNumPlayers() );
		tdf.Append("NumUsers", battle.GetNumUsers() );
	}

	tdf.AppendLineBreak();

	unsigned int NumUsers = battle.GetNumUsers();

	typedef std::map<int, int> ProgressiveTeamsVec;
	typedef ProgressiveTeamsVec::iterator ProgressiveTeamsVecIter;
	ProgressiveTeamsVec teams_to_sorted_teams; // original team -> progressive team
	int free_team = 0;
	std::map<User*, int> player_to_number; // player -> ordernumber
	srand ( time(NULL) );
	for ( unsigned int i = 0; i < NumUsers; i++ ) {
		User& user = battle.GetUser( i );
		UserBattleStatus& status = user.BattleStatus();
		if ( !status.spectator ) {
			ProgressiveTeamsVecIter itor = teams_to_sorted_teams.find ( status.team );
			if ( itor == teams_to_sorted_teams.end() ) {
				teams_to_sorted_teams[status.team] = free_team;
				free_team++;
			}
		}
		if ( battle.IsProxy() && ( user.GetNick() == battle.GetFounder().GetNick() ) ) continue;
		if ( status.IsBot() ) continue;
		tdf.EnterSection(stdprintf("PLAYER%d", i));
		tdf.Append("Name", user.GetNick() );
		tdf.Append("CountryCode", user.GetCountry().Lower());
		tdf.Append("Spectator", status.spectator );
		tdf.Append("Rank", (int)user.GetRank() );
		tdf.Append("IsFromDemo", int(status.isfromdemo) );
		if ( !status.scriptPassword.empty() ) {
			tdf.Append("Password", status.scriptPassword );
		}
		if ( !status.spectator ) {
			tdf.Append("Team", teams_to_sorted_teams[status.team] );
		} else {
			int speccteam = 0;
			if ( !teams_to_sorted_teams.empty() ) speccteam = rand() % teams_to_sorted_teams.size();
			tdf.Append("Team", speccteam );
		}
		tdf.LeaveSection();
		player_to_number[&user] = i;
	}
	if ( LSL::usync().VersionSupports( LSL::USYNC_GetSkirmishAI ) ) {
		for ( unsigned int i = 0; i < NumUsers; i++ ) {
			User& user = battle.GetUser( i );
			UserBattleStatus& status = user.BattleStatus();
			if ( !status.IsBot() ) continue;
			tdf.EnterSection(stdprintf("AI%d", i));
			tdf.Append("Name", STD_STRING(user.GetNick())); // AI's nick;
			tdf.Append("ShortName", status.aishortname ); // AI libtype
			tdf.Append("Version", status.aiversion ); // AI libtype version
			tdf.Append("Team", teams_to_sorted_teams[status.team] );
			tdf.Append("IsFromDemo", int(status.isfromdemo) );
			tdf.Append("Host", player_to_number[&battle.GetUser( status.owner )] );
			tdf.EnterSection("Options");
			int optionmapindex = battle.CustomBattleOptions().GetAIOptionIndex(STD_STRING(user.GetNick()));
			if ( optionmapindex > 0 ) {
	for (const auto& it : battle.CustomBattleOptions().getOptions((LSL::OptionsWrapper::GameOption)optionmapindex )) {
					tdf.Append(it.first, it.second.second);
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
	for ( unsigned int i = 0; i < NumUsers; i++ ) {
		User& usr = battle.GetUser( i );
		UserBattleStatus& status = usr.BattleStatus();
		if ( status.spectator ) continue;
		if ( parsedteams.find( status.team ) != parsedteams.end() ) continue; // skip duplicates
		parsedteams.insert( status.team );

		tdf.EnterSection(stdprintf("TEAM%d", teams_to_sorted_teams[status.team]));
		if ( !LSL::usync().VersionSupports(LSL::USYNC_GetSkirmishAI ) && status.IsBot() ) {
			tdf.Append("AIDLL", status.aishortname );
			tdf.Append("TeamLeader", player_to_number[&battle.GetUser( status.owner )] ); // bot owner is the team leader
		} else {
			if ( status.IsBot() ) {
				tdf.Append("TeamLeader", player_to_number[&battle.GetUser( status.owner )] );
			} else {
				tdf.Append("TeamLeader", player_to_number[&usr] );
			}
		}
		if ( battle.IsProxy() ) {
			if ( startpostype == IBattle::ST_Pick ) {
				tdf.Append("StartPosX", status.pos.x );
				tdf.Append("StartPosZ", status.pos.y );
			} else if ( ( startpostype == IBattle::ST_Fixed ) || ( startpostype == IBattle::ST_Random ) ) {
				int teamnumber = teams_to_sorted_teams[status.team];
				if ( teamnumber < int(remap_positions.size()) ) { // don't overflow
					LSL::StartPos position = remap_positions[teamnumber];
					tdf.Append("StartPosX", position.x );
					tdf.Append("StartPosZ", position.y );
				}
			}
		} else {
			if ( startpostype == IBattle::ST_Pick ) {
				tdf.Append("StartPosX", status.pos.x );
				tdf.Append("StartPosZ", status.pos.y );
			}
		}

		tdf.Append("AllyTeam",status.ally );

		wxString colourstring =
			TowxString( status.colour.Red()/255.0 ) + _T(' ') +
			TowxString( status.colour.Green()/255.0 ) + _T(' ') +
			TowxString( status.colour.Blue()/255.0 );
		tdf.Append("RGBColor", STD_STRING(colourstring));

		unsigned int side = status.side;
		if ( side < sides.size() ) tdf.Append("Side", sides[side] );
		tdf.Append("Handicap", status.handicap );
		tdf.LeaveSection();
	}

	tdf.AppendLineBreak();

	unsigned int maxiter = std::max( NumUsers, battle.GetLastRectIdx() + 1 );
	std::set<int> parsedallys;
	for ( unsigned int i = 0; i < maxiter; i++ ) {

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

		tdf.EnterSection( stdprintf("ALLYTEAM%d", ally));
		tdf.Append("NumAllies", 0 );
		if ( startpostype == IBattle::ST_Choose ) {
			if ( sr.IsOk() ) {
				const char* old_locale = std::setlocale(LC_NUMERIC, "C");

				tdf.Append("StartRectLeft", wxFormat( _T("%.3f") ) % ( sr.left / 200.0 ) );
				tdf.Append("StartRectTop", wxFormat( _T("%.3f") ) % ( sr.top / 200.0 ) );
				tdf.Append("StartRectRight", wxFormat( _T("%.3f") ) % ( sr.right / 200.0 ) );
				tdf.Append("StartRectBottom", wxFormat( _T("%.3f") ) % ( sr.bottom / 200.0 ) );

				std::setlocale(LC_NUMERIC, old_locale);
			}
		}
		tdf.LeaveSection();
	}

	tdf.LeaveSection();
	return TowxString(ret.str());
}
