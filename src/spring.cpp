/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Spring
//

#include <wx/file.h>
#include <wx/intl.h>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <clocale>
#include <stdexcept>

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

BEGIN_EVENT_TABLE( Spring, wxEvtHandler )

    EVT_COMMAND ( PROC_SPRING, wxEVT_SPRING_EXIT, Spring::OnTerminated )

END_EVENT_TABLE();


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


bool Spring::Run( Battle& battle )
{
  if ( m_running ) {
    debug_error( "Spring already running!" );
    return false;
  }

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

  debug( "Path to script: " + STD_STRING(path) + "script.txt" );

  try {

    if ( !wxFile::Access( path + _T("script.txt"), wxFile::write ) ) {
      debug_error( "Access denied to script.txt." );
    }

    wxFile f( path + _T("script.txt"), wxFile::write );
    f.Write( GetScriptTxt(battle) );
    f.Close();

  } catch (...) {
    debug_error( "Couldn't write script.txt" ); //!@todo Some message to user.
    return false;
  }

  wxString cmd =  _T("\"") + WX_STRING(sett().GetSpringUsedLoc()) + _T("\" ") + path + _T("script.txt");
  debug( "cmd: " + STD_STRING(cmd) );
  wxSetWorkingDirectory( WX_STRING(sett().GetSpringDir()) );
  if ( sett().UseOldSpringLaunchMethod() ) {
    if ( m_wx_process == 0 ) m_wx_process = new wxSpringProcess( *this );
    if ( wxExecute( cmd , wxEXEC_ASYNC, m_wx_process ) == 0 ) return false;
  } else {
    if ( m_process == 0 ) m_process = new SpringProcess( *this );
    debug( "m_process->Create();" );
    m_process->Create();
    debug( "m_process->SetCommand( cmd );" );
    m_process->SetCommand( cmd );
    debug( "m_process->Run();" );
    m_process->Run();
  }
  m_running = true;
  debug( "Done running = true" );
  return true;
}


bool Spring::Run( SinglePlayerBattle& battle )
{
  if ( m_running ) {
    debug_error( "Spring already running!" );
    return false;
  }

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

  try {

    if ( !wxFile::Access( path + _T("script.txt"), wxFile::write ) ) {
      debug_error( "Access denied to script.txt." );
    }

    wxFile f( path + _T("script.txt"), wxFile::write );
    f.Write( GetSPScriptTxt(battle) );
    f.Close();

  } catch (...) {
    debug_error( "Couldn't write script.txt" ); //!@todo Some message to user.
    return false;
  }

  wxString cmd =  _T("\"") + WX_STRING(sett().GetSpringUsedLoc()) + _T("\" ") + path + _T("script.txt");
  wxSetWorkingDirectory( WX_STRING(sett().GetSpringDir()) );
  if ( sett().UseOldSpringLaunchMethod() ) {
    if ( m_wx_process == 0 ) m_wx_process = new wxSpringProcess( *this );
    if ( wxExecute( cmd , wxEXEC_ASYNC, m_wx_process ) == 0 ) return false;
  } else {
    if ( m_process == 0 ) m_process = new SpringProcess( *this );
    m_process->Create();
    m_process->SetCommand( cmd );
    m_process->Run();
  }

  m_running = true;
  return true;
}


bool Spring::TestSpringBinary()
{
  if ( !wxFileName::FileExists( WX_STRING(sett().GetSpringUsedLoc()) ) ) return false;
  if ( usync()->GetSpringVersion() != "") return true;
  else return false;
}


void Spring::OnTerminated( wxCommandEvent& event )
{
  debug_func("");
  m_running = false;
  m_process = 0; // NOTE I'm not sure if this should be deleted or not, according to wx docs it shouldn't.
  m_wx_process = 0;
  m_ui.OnSpringTerminated( true );
}


std::string GetWord( std::string& params )
{
  std::string::size_type pos;
  std::string param;

  pos = params.find( " ", 0 );
  if ( pos == std::string::npos ) {
    param = params;
    params = "";
    return param;
  } else {
    param = params.substr( 0, pos );
    params = params.substr( pos + 1 );
    return param;
  }
}


wxString Spring::GetScriptTxt( Battle& battle )
{
  wxString s;

  int NumTeams=0, NumBots = 0, NumAllys=0, LastOrder=-1,Lowest=-1,MyPlayerNum=-1;
  int PlayerOrder[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int BotOrder[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int TeamConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyRevConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  debug("1 numusers: " + i2s(battle.GetNumUsers()) );
  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    Lowest = -1;
    // Find next player in the order they were sent from the server.
    debug("2 i: " + i2s(i) );
    for ( user_map_t::size_type gl = 0; gl < battle.GetNumUsers(); gl++ ) {
      User& ou = battle.GetUser(gl);
      debug("2.1 order: " + i2s(ou.BattleStatus().order) );
      if ( (ou.BattleStatus().order <= LastOrder) && (LastOrder != -1) ) continue;
      if ( Lowest == -1 ) Lowest = gl;
      else if ( ou.BattleStatus().order < battle.GetUser(Lowest).BattleStatus().order ) Lowest = gl;
    }
    debug("3 Lowest: " + i2s(Lowest) );
    LastOrder = battle.GetUser(Lowest).BattleStatus().order;
    User& u = battle.GetUser( Lowest );

    PlayerOrder[i] = Lowest;
    if ( &u == &battle.GetMe() ) MyPlayerNum = i;

    UserBattleStatus bs = u.BattleStatus();
    if ( bs.spectator ) continue;

    // Transform team numbers.
    if ( TeamConv[bs.team] == -1 ) TeamConv[bs.team] = NumTeams++;

    // Transform ally numbers.
    if ( AllyConv[bs.ally] == -1 ) {
      AllyConv[bs.ally] = NumAllys;
      AllyRevConv[NumAllys] = bs.ally;
      NumAllys++;
    }

  }
  debug("4");

  // Get bot order
  LastOrder = -1;
  for ( std::list<BattleBot*>::size_type i = 0; i < battle.GetNumBots(); i++ ) {

    Lowest = -1;

    debug("5");
    for ( std::list<BattleBot*>::size_type gl = 0; gl < battle.GetNumBots(); gl++ ) {
      if ( (battle.GetBot(gl)->bs.order <= LastOrder) && (LastOrder != -1) ) continue;
      if ( Lowest == -1 ) Lowest = gl;
      else if ( battle.GetBot(gl)->bs.order < battle.GetBot(Lowest)->bs.order ) Lowest = gl;
    }
    debug("6");

    LastOrder = battle.GetBot(Lowest)->bs.order;
    BotOrder[i] = Lowest;

    if ( AllyConv[battle.GetBot(Lowest)->bs.ally] == -1 ) {
      AllyConv[battle.GetBot(Lowest)->bs.ally] = NumAllys;
      AllyRevConv[NumAllys] = battle.GetBot(Lowest)->bs.ally;
      NumAllys++;
    }

    NumBots++;
  }
  debug("7");


  //BattleOptions bo = battle.opts();

  // Start generating the script.
  s  = wxString::Format( _T("[GAME]\n{\n") );

  //s += wxString::Format( _T("\tMapname=%s;\n"), bo.mapname.c_str() );
  s += _T("\tMapname=") + battle.GetMapName() + _T(";\n");
  s += wxString::Format( _T("\tStartMetal=%d;\n"), battle.GetStartMetal() );
  s += wxString::Format( _T("\tStartEnergy=%d;\n"), battle.GetStartEnergy() );
  s += wxString::Format( _T("\tMaxUnits=%d;\n"), battle.GetMaxUnits() );
  s += wxString::Format( _T("\tStartPosType=%d;\n"), battle.GetStartType() );
  s += wxString::Format( _T("\tGameMode=%d;\n"), battle.GetGameType() );
  s += WX_STRING(("\tGameType=" + usync()->GetModArchive(usync()->GetModIndex(STD_STRING(battle.GetModName()))) + ";\n"));
  s += wxString::Format( _T("\tLimitDGun=%d;\n"), battle.LimitDGun()?1:0 );
  s += wxString::Format( _T("\tDiminishingMMs=%d;\n"), battle.DimMMs()?1:0 );
  s += wxString::Format( _T("\tGhostedBuildings=%d;\n\n"), battle.GhostedBuildings()?1:0 );

  if ( battle.IsFounderMe() ) s += wxString::Format( _T("\tHostIP=localhost;\n") );
  else s += WX_STRING(("\tHostIP=" + battle.GetHostIp() + ";\n"));
  s += wxString::Format( _T("\tHostPort=%d;\n\n"), battle.GetHostPort() );

  s += wxString::Format( _T("\tMyPlayerNum=%d;\n\n"), MyPlayerNum );

  s += wxString::Format( _T("\tNumPlayers=%d;\n"), battle.GetNumUsers() );
  s += wxString::Format( _T("\tNumTeams=%d;\n"), NumTeams + NumBots );
  s += wxString::Format( _T("\tNumAllyTeams=%d;\n\n"), NumAllys );

  debug("8");
  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    s += wxString::Format( _T("\t[PLAYER%d]\n"), i );
    s += wxString::Format( _T("\t{\n") );
    s += WX_STRING(("\t\tname=" + battle.GetUser( PlayerOrder[i] ).GetNick() + ";\n"));
    wxString cc = WX_STRING( battle.GetUser( PlayerOrder[i] ).GetCountry() );
    cc.MakeLower();
    s += _T("\t\tcountryCode=") + cc + _T(";\n");
    s += wxString::Format( _T("\t\tSpectator=%d;\n"), battle.GetUser( PlayerOrder[i] ).BattleStatus().spectator?1:0 );
    if ( !(battle.GetUser( PlayerOrder[i] ).BattleStatus().spectator) ) {
      s += wxString::Format( _T("\t\tteam=%d;\n"), TeamConv[battle.GetUser( PlayerOrder[i] ).BattleStatus().team] );
    }
    s += wxString::Format( _T("\t}\n") );
  }
  debug("9");

  s += _T("\n");


  for ( int i = 0; i < NumTeams; i++ ) {
    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i );

    // Find Team Leader.
    int TeamLeader = -1;

    debug("10");
    for( user_map_t::size_type tlf = 0; tlf < battle.GetNumUsers(); tlf++ ) {
      // First Player That Is In The Team Is Leader.
      if ( TeamConv[battle.GetUser( PlayerOrder[tlf] ).BattleStatus().team] == i ) {

        // Make sure player is not spectator.
        if ( battle.GetUser( PlayerOrder[tlf] ).BattleStatus().spectator ) continue;

        // Assign as team leader.
        TeamLeader = tlf;
        break;
      }

    }
    debug("11");

    s += wxString::Format( _T("\t\tTeamLeader=%d;\n") ,TeamLeader );
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().ally] );
    const char* old_locale = std::setlocale(LC_NUMERIC, "C");
    s += wxString::Format( _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
           (double)(battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().color_r/255.0),
           (double)(battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().color_g/255.0),
           (double)(battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().color_b/255.0)
         );
    std::setlocale(LC_NUMERIC, old_locale);
    debug( i2s(battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().side) );
    usync()->SetCurrentMod( STD_STRING(battle.GetModName()) );
    s += WX_STRING(("\t\tSide=" + usync()->GetSideName( battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().side ) + ";\n"));
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().handicap );
    s +=  _T("\t}\n");
  }
  debug("12");

  for ( int i = 0; i < NumBots; i++ ) {

    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i + NumTeams );

    BattleBot& bot = *battle.GetBot( BotOrder[i] );

    // Find Team Leader.
    int TeamLeader = TeamConv[ battle.GetUser( bot.owner ).BattleStatus().team ];

    s += wxString::Format( _T("\t\tTeamLeader=%d;\n") ,TeamLeader );
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[bot.bs.ally] );
    const char* old_locale = std::setlocale(LC_NUMERIC, "C");
    s += wxString::Format(
      _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
      (double)(bot.bs.color_r/255.0),
      (double)(bot.bs.color_g/255.0),
      (double)(bot.bs.color_b/255.0)
    );

    std::setlocale(LC_NUMERIC, old_locale);
    usync()->SetCurrentMod(STD_STRING(battle.GetModName()) );
    s += WX_STRING(("\t\tSide=" + usync()->GetSideName( bot.bs.side ) + ";\n"));
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), bot.bs.handicap );

    wxString ai = WX_STRING( bot.aidll );
/*    if ( wxFileName::FileExists( WX_STRING( sett().GetSpringDir() ) + wxFileName::GetPathSeparator() + _T("AI") + wxFileName::GetPathSeparator() + _T("Bot-libs") + wxFileName::GetPathSeparator() + ai + _T(".dll") ) ) {
      ai += _T(".dll");
    } else {
      ai += _T(".so");
    }*/

    //s += WX_STRING(("\t\tAIDLL=AI/Bot-libs/" + STD_STRING(ai) + ";\n"));
    s += _T("\t\tAIDLL=") + usync()->GetBotLibPath( ai ) + _T(";\n");
    s +=  _T("\t}\n");
  }
  debug("13");


  for ( int i = 0; i < NumAllys; i++ ) {
    s += wxString::Format( _T("\t[ALLYTEAM%d]\n\t{\n"), i );

    int NumInAlly = 0;
    s += wxString::Format( _T("\t\tNumAllies=%d;\n"), NumInAlly );

   if ( (battle.GetStartRect(AllyRevConv[i]) != 0) && (battle.GetStartType() == ST_Choose) ) {
      BattleStartRect* sr = (BattleStartRect*)battle.GetStartRect(AllyRevConv[i]);
      const char* old_locale = std::setlocale(LC_NUMERIC, "C");
      s += wxString::Format( _T("\t\tStartRectLeft=%.3f;\n"), sr->left / 200.0 );
      s += wxString::Format( _T("\t\tStartRectTop=%.3f;\n"), sr->top / 200.0 );
      s += wxString::Format( _T("\t\tStartRectRight=%.3f;\n"), sr->right / 200.0 );
      s += wxString::Format( _T("\t\tStartRectBottom=%.3f;\n"), sr->bottom / 200.0 );
      std::setlocale(LC_NUMERIC, old_locale);
    }

    s +=  _T("\t}\n");
  }

  debug("14");

  s += wxString::Format( _T("\tNumRestrictions=%d;\n"), battle.GetNumDisabledUnits() );
  s += _T("\t[RESTRICT]\n");
  s += _T("\t{\n");

  std::string units = battle.DisabledUnits();
  std::string unit;
  int i = 0;

  while ( (unit = GetWord( units )) != "" ) {
    s += WX_STRING(("\t\tUnit" + i2s(i) + "=" + unit + ";\n"));
    s += wxString::Format( _T("\t\tLimit%d=0;\n"), i );
    i++;
  }

  debug("15");

  s += _T("\t}\n");
  s += _T("}\n");

  if ( DOS_TXT ) {
    s.Replace( _T("\n"), _T("\r\n"), true );
  }

  wxString ds = _T("[Script End]\n\n----------------------[ Debug info ]-----------------------------\nUsers:\n\n");
  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    User& tmpu = battle.GetUser( i );
    ds += WX_STRING( tmpu.GetNick() );
    ds += wxString::Format( _T(":\n  team: %d ally: %d spec: %d order: %d side: %d hand: %d sync: %d ready: %d col: %d,%d,%d\n\n"),
      tmpu.BattleStatus().team,
      tmpu.BattleStatus().ally,
      tmpu.BattleStatus().spectator,
      tmpu.BattleStatus().order,
      tmpu.BattleStatus().side,
      tmpu.BattleStatus().handicap,
      tmpu.BattleStatus().sync,
      tmpu.BattleStatus().ready,
      tmpu.BattleStatus().color_r,
      tmpu.BattleStatus().color_g,
      tmpu.BattleStatus().color_b
    );
  }

  debug("16");
  ds += _T("\n\nBots: \n\n");
  for ( std::list<BattleBot*>::size_type i = 0; i < battle.GetNumBots(); i++ ) {
    BattleBot* bot = battle.GetBot(i);
    if ( bot == 0 ) {
      ds += _T( "NULL" );
      continue;
    }
    ds += WX_STRING( bot->name ) + _T(" (") + WX_STRING( bot->owner ) + _T(")");
    ds += wxString::Format( _T(":\n  team: %d ally: %d spec: %d order: %d side: %d hand: %d sync: %d ready: %d col: %d,%d,%d\n\n"),
      bot->bs.team,
      bot->bs.ally,
      bot->bs.spectator,
      bot->bs.order,
      bot->bs.side,
      bot->bs.handicap,
      bot->bs.sync,
      bot->bs.ready,
      bot->bs.color_r,
      bot->bs.color_g,
      bot->bs.color_b
    );
  }
  debug("17");

  ds += _T("\n\nPlayerOrder: { ");
  for ( int i = 0; i < 16; i++ ) {
    ds += wxString::Format( _T(" %d,"), PlayerOrder[i] );
  }
  ds += _T(" }\n\n");

  ds += _T("TeamConv: { ");
  for ( int i = 0; i < 16; i++ ) {
    ds += wxString::Format( _T(" %d,"), TeamConv[i] );
  }
  ds += _T(" }\n\n");

  ds += _T("AllyConv: { ");
  for ( int i = 0; i < 16; i++ ) {
    ds += wxString::Format( _T(" %d,"), AllyConv[i] );
  }
  ds += _T(" }\n\n");

  ds += _T("AllyRevConv: { ");
  for ( int i = 0; i < 16; i++ ) {
    ds += wxString::Format( _T(" %d,"), AllyRevConv[i] );
  }
  ds += _T(" }\n\n\n");

  ds += wxString::Format( _T("NumTeams: %d\n\nNumAllys: %d\n\nMyPlayerNum: %d\n\n"), NumTeams, NumAllys, MyPlayerNum );

  if ( DOS_TXT ) {
    ds.Replace( _T("\n"), _T("\r\n"), true );
  }
  debug("18");

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

  wxFile f( wxString(path + _T("script_debug.txt")), wxFile::write );
  f.Write( _T("[Script Start]") + s );
  f.Write( ds );
  f.Close();
  debug("19");

  return s;
}


wxString Spring::GetSPScriptTxt( SinglePlayerBattle& battle )
{
  wxString s;
  int AllyConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int NumAllys = 0;
  int PlayerTeam = -1;

  for ( unsigned int i = 0; i < battle.GetNumBots(); i++ ) {
    BattleBot* bot = battle.GetBotByStartPosition( i );
    ASSERT_LOGIC( bot != 0, "bot == 0" );
    if ( bot->aidll == "" ) PlayerTeam = i;
    if ( AllyConv[bot->bs.ally] == -1 ) AllyConv[bot->bs.ally] = NumAllys++;
  }

  ASSERT_LOGIC( PlayerTeam != -1, "no player found" );

  // Start generating the script.
  s  = wxString::Format( _T("[GAME]\n{\n") );

  //s += wxString::Format( _T("\tMapname=%s;\n"), bo.mapname.c_str() );
  s += _T("\tMapname=") + battle.GetMapName() + _T(";\n");
  s += wxString::Format( _T("\tStartMetal=%d;\n"), battle.GetStartMetal() );
  s += wxString::Format( _T("\tStartEnergy=%d;\n"), battle.GetStartEnergy() );
  s += wxString::Format( _T("\tMaxUnits=%d;\n"), battle.GetMaxUnits() );
  s += wxString::Format( _T("\tStartPosType=%d;\n"), battle.GetStartType() );
  s += wxString::Format( _T("\tGameMode=%d;\n"), battle.GetGameType() );
  s += WX_STRING(("\tGameType=" + usync()->GetModArchive(usync()->GetModIndex(STD_STRING(battle.GetModName()))) + ";\n"));
  s += wxString::Format( _T("\tLimitDGun=%d;\n"), battle.LimitDGun()?1:0 );
  s += wxString::Format( _T("\tDiminishingMMs=%d;\n"), battle.DimMMs()?1:0 );
  s += wxString::Format( _T("\tGhostedBuildings=%d;\n\n"), battle.GhostedBuildings()?1:0 );

  s += _T("\tHostIP=localhost;\n");

  s += _T("\tHostPort=8452;\n\n");

  s += _T("\tMyPlayerNum=0;\n\n");

  s += _T("\tNumPlayers=1;\n");
  s += wxString::Format( _T("\tNumTeams=%d;\n"), battle.GetNumBots() );
  s += wxString::Format( _T("\tNumAllyTeams=%d;\n\n"), NumAllys );

  s += wxString::Format( _T("\t[PLAYER0]\n") );
    s += _T("\t{\n");
    s += _T("\t\tname=Player;\n");
    s += _T("\t\tSpectator=0;\n");
    s += wxString::Format( _T("\t\tteam=%d;\n"), PlayerTeam );
  s += _T("\t}\n\n");


  for ( unsigned int i = 0; i < battle.GetNumBots(); i++ ) { // TODO fix this when new Spring comes.
    BattleBot* bot = battle.GetBotByStartPosition( i );
    ASSERT_LOGIC( bot != 0, "bot == 0" );
    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i );

    s += _T("\t\tTeamLeader=0;\n");
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[bot->bs.ally] );
    const char* old_locale = std::setlocale(LC_NUMERIC, "C");
    s += wxString::Format( _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
           (double)(bot->bs.color_r/255.0),
           (double)(bot->bs.color_g/255.0),
           (double)(bot->bs.color_b/255.0)
         );
    std::setlocale(LC_NUMERIC, old_locale);
    usync()->SetCurrentMod( STD_STRING(battle.GetModName()) );
    s += WX_STRING(("\t\tSide=" + usync()->GetSideName( bot->bs.side ) + ";\n"));
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), bot->bs.handicap );
    if ( bot->aidll != "" ) {
      wxString ai = WX_STRING( bot->aidll );
      /*if ( wxFileName::FileExists( WX_STRING( sett().GetSpringDir() ) + wxFileName::GetPathSeparator() + _T("AI") + wxFileName::GetPathSeparator() + _T("Bot-libs") + wxFileName::GetPathSeparator() + ai + _T(".dll") ) ) {
        ai += _T(".dll");
      } else {
        ai += _T(".so");
      }*/
      s += _T("\t\tAIDLL=") + usync()->GetBotLibPath( ai ) + _T(";\n");
      //s += _T("\t\tAIDLL=AI/Bot-libs/") + ai + _T(";\n");
    }
    s +=  _T("\t}\n");
  }

  for ( int i = 0; i < NumAllys; i++ ) {
    s += wxString::Format( _T("\t[ALLYTEAM%d]\n\t{\n"), i );
      s += _T("\t\tNumAllies=0;\n");
    s += _T("\t}\n");
  }


  s += wxString::Format( _T("\tNumRestrictions=%d;\n"), battle.GetNumDisabledUnits() );
  s += _T("\t[RESTRICT]\n");
  s += _T("\t{\n");

  std::string units = battle.DisabledUnits();
  std::string unit;
  int i = 0;

  while ( (unit = GetWord( units )) != "" ) {
    s += WX_STRING(("\t\tUnit" + i2s(i) + "=" + unit + ";\n"));
    s += wxString::Format( _T("\t\tLimit%d=0;\n"), i );
    i++;
  }


  s += _T("\t}\n");
  s += _T("}\n");

  if ( DOS_TXT ) {
    s.Replace( _T("\n"), _T("\r\n"), true );
  }

  return s;
}

