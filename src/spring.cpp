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
#include <vector>
#include <algorithm>

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


bool Spring::Run( Battle& battle )
{
  if ( m_running ) {
    wxLogError( _T("Spring already running!") );
    return false;
  }

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

  wxLogMessage( _T("Path to script: %sscript.txt"), path.c_str() );

  try {

    if ( !wxFile::Access( path + _T("script.txt"), wxFile::write ) ) {
      wxLogError( _T("Access denied to script.txt.") );
    }

    wxFile f( path + _T("script.txt"), wxFile::write );
    f.Write( GetScriptTxt(battle) );
    f.Close();

  } catch (...) {
    wxLogError( _T("Couldn't write script.txt") );
    return false;
  }

  wxString cmd =  _T("\"") + sett().GetSpringUsedLoc() + _T("\" ") + path + _T("script.txt");
  wxLogMessage( _T("cmd: %s"), cmd.c_str() );
  wxSetWorkingDirectory( sett().GetSpringDir() );
  if ( sett().UseOldSpringLaunchMethod() ) {
    if ( m_wx_process == 0 ) m_wx_process = new wxSpringProcess( *this );
    if ( wxExecute( cmd , wxEXEC_ASYNC, m_wx_process ) == 0 ) return false;
  } else {
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
  if ( m_running ) {
    wxLogError( _T("Spring already running!") );
    return false;
  }

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

  try {

    if ( !wxFile::Access( path + _T("script.txt"), wxFile::write ) ) {
      wxLogError( _T("Access denied to script.txt.") );
    }

    wxFile f( path + _T("script.txt"), wxFile::write );
    f.Write( GetSPScriptTxt(battle) );
    f.Close();

  } catch (...) {
    wxLogError( _T("Couldn't write script.txt") );
    return false;
  }

  wxString cmd =  _T("\"") + sett().GetSpringUsedLoc() + _T("\" ") + path + _T("script.txt");
  wxSetWorkingDirectory( sett().GetSpringDir() );
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
  if ( !wxFileName::FileExists( sett().GetSpringUsedLoc() ) ) return false;
  if ( usync()->GetSpringVersion() != _T("")) return true;
  else return false;
}


void Spring::OnTerminated( wxCommandEvent& event )
{
  wxLogDebugFunc( _T("") );
  m_running = false;
  m_process = 0; // NOTE I'm not sure if this should be deleted or not, according to wx docs it shouldn't.
  m_wx_process = 0;
  m_ui.OnSpringTerminated( true );
}


  struct UserOrder{
    int index;/// user number for GetUser
    int order;/// user order (we'll sort by it)
    bool operator<(UserOrder b) const {/// comparison function for sorting
      return order<b.order;
    }
  };


wxString Spring::GetScriptTxt( Battle& battle )
{
  wxLogMessage(_T("0 GetScriptTxt called "));
  wxString s;

  int  NumTeams=0, MyPlayerNum=-1;

  std::vector<UserOrder> ordered_users;
  std::vector<UserOrder> ordered_bots;
  std::vector<int> TeamConv, AllyConv, AllyRevConv;
  /// AllyRevConv.size() gives number of allies

  wxLogMessage(_T("1 numusers: %d"),battle.GetNumUsers() );

  /// Fill ordered_users and sort it
  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ){
    UserOrder tmp;
    tmp.index=i;
    tmp.order=battle.GetUser(i).BattleStatus().order;
    ordered_users.push_back(tmp);
  }
  std::sort(ordered_users.begin(),ordered_users.end());

  /// process ordered users, convert teams and allys
  for(int i=0;i<int(ordered_users.size());++i){
    wxLogMessage(_T("2 order %d index %d"), ordered_users[i].order, ordered_users[i].index);
    User &user = battle.GetUser(ordered_users[i].index);

    if ( &user == &battle.GetMe() ) MyPlayerNum = i;

    UserBattleStatus status = user.BattleStatus();
    if ( status.spectator ) continue;

    if(status.team>int(TeamConv.size())-1){
      TeamConv.resize(status.team+1,-1);
    }
    if ( TeamConv[status.team] == -1 ) TeamConv[status.team] = NumTeams++;

    if(status.ally>int(AllyConv.size())-1){
      AllyConv.resize(status.ally+1,-1);
    }
    if ( AllyConv[status.ally] == -1 ) {
      AllyConv[status.ally] = AllyRevConv.size();
      AllyRevConv.push_back(status.ally);
    }

  }
  wxLogMessage(_T("3"));

  /// sort and process bots now

  for ( std::list<BattleBot*>::size_type i = 0; i < battle.GetNumBots(); i++ ) {
    UserOrder tmp;
    tmp.index=i;
    tmp.order=battle.GetBot(i)->bs.order;
    ordered_bots.push_back(tmp);
  }
  std::sort(ordered_bots.begin(),ordered_bots.end());

  for(int i=0;i<int(ordered_bots.size());++i){

    BattleBot &bot=*battle.GetBot(ordered_bots[i].index);

    if(bot.bs.ally>int(AllyConv.size())-1){
      AllyConv.resize(bot.bs.ally+1,-1);
    }
    if(AllyConv[bot.bs.ally]==-1){
      AllyConv[bot.bs.ally]=AllyRevConv.size();
      AllyRevConv.push_back(bot.bs.ally);
    }
  }

  ///(dmytry aka dizekat) i don't want to change code below, so these vars for compatibility. If you want, refactor it.

  int NumAllys=AllyRevConv.size();
  int NumBots=ordered_bots.size();


  wxLogMessage(_T("7"));


  //BattleOptions bo = battle.opts();

  // Start generating the script.
  s  = wxString::Format( _T("[GAME]\n{\n") );

  //s += wxString::Format( _T("\tMapname=%s;\n"), bo.mapname.c_str() );
  s += _T("\tMapname=") + battle.GetMapName() + _T(";\n");
  s += _T("\tGameType=") + usync()->GetModArchive(usync()->GetModIndex(battle.GetModName())) + _T(";\n");
  unsigned long uhash;
  battle.GetModHash().ToULong(&uhash);
  s += wxString::Format( _T("\tModHash=%d;\n"), (int)uhash );
  wxStringTripleVec optlistEng;
  battle.CustomBattleOptions()->getOptions( &optlistEng, EngineOption );
  for (wxStringTripleVec::iterator it = optlistEng.begin(); it != optlistEng.end(); ++it)
  {
    s += _T("\t") + it->first + _T("=") + it->second.second + _T(";\n");
  }

  if ( battle.IsFounderMe() ) s += wxString::Format( _T("\tHostIP=localhost;\n") );
  else s += _T("\tHostIP=") + battle.GetHostIp() + _T(";\n");

  if ( battle.IsFounderMe() && battle.GetNatType() == NAT_Hole_punching ) {
    s += wxString::Format( _T("\tHostPort=%d;\n"), battle.GetMyInternalUdpSourcePort() );
  }
  else {
    s += wxString::Format( _T("\tHostPort=%d;\n"), battle.GetHostPort() );
    }

  if ( !battle.IsFounderMe() )
  {
    if ( battle.GetNatType() == NAT_Fixed_source_ports )
    {
      s += wxString::Format( _T("\tSourcePort=%d;\n"), FIRST_UDP_SOURCEPORT + MyPlayerNum -1 );
    }
    else if ( battle.GetNatType() == NAT_Hole_punching )
    {
      s += wxString::Format( _T("\tSourcePort=%d;\n"), battle.GetMyInternalUdpSourcePort() );
    }
  }

  s += _T("\n");

  s += wxString::Format( _T("\tMyPlayerNum=%d;\n\n"), MyPlayerNum );

  s += wxString::Format( _T("\tNumPlayers=%d;\n"), battle.GetNumUsers() );
  s += wxString::Format( _T("\tNumTeams=%d;\n"), NumTeams + NumBots );
  s += wxString::Format( _T("\tNumAllyTeams=%d;\n\n"), NumAllys );

  wxLogMessage(_T("8"));
  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    s += wxString::Format( _T("\t[PLAYER%d]\n"), i );
    s += wxString::Format( _T("\t{\n") );
    s += _T("\t\tname=") + battle.GetUser( ordered_users[i].index ).GetNick() + _T(";\n");
    s += _T("\t\tcountryCode=") + battle.GetUser( ordered_users[i].index ).GetCountry().Lower() + _T(";\n");
    s += wxString::Format( _T("\t\tSpectator=%d;\n"), battle.GetUser( ordered_users[i].index ).BattleStatus().spectator?1:0 );
    if ( !(battle.GetUser( ordered_users[i].index ).BattleStatus().spectator) ) {
      s += wxString::Format( _T("\t\tteam=%d;\n"), TeamConv[battle.GetUser( ordered_users[i].index ).BattleStatus().team] );
    }
    s += wxString::Format( _T("\t}\n") );
  }
  wxLogMessage(_T("9"));

  s += _T("\n");


  for ( int i = 0; i < NumTeams; i++ ) {
    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i );

    // Find Team Leader.
    int TeamLeader = -1;

    wxLogMessage(_T("10"));
    for( user_map_t::size_type tlf = 0; tlf < battle.GetNumUsers(); tlf++ ) {/// change: moved check if spectator above use of TeamConv array, coz TeamConv array does not include spectators.

      // Make sure player is not spectator.
      if ( battle.GetUser( ordered_users[tlf].index ).BattleStatus().spectator ) continue;

      // First Player That Is In The Team Is Leader.
      if ( TeamConv[battle.GetUser( ordered_users[tlf].index ).BattleStatus().team] == i ) {



        // Assign as team leader.
        TeamLeader = tlf;
        break;
      }

    }
    wxLogMessage(_T("11"));

    s += wxString::Format( _T("\t\tTeamLeader=%d;\n") ,TeamLeader );
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().ally] );
    const char* old_locale = std::setlocale(LC_NUMERIC, "C");
    s += wxString::Format( _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
           (double)(battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().colour.Red()/255.0),
           (double)(battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().colour.Green()/255.0),
           (double)(battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().colour.Blue()/255.0)
         );
    std::setlocale(LC_NUMERIC, old_locale);
    wxLogMessage( _T("%d"), battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().side );
    s += _T("\t\tSide=") + usync()->GetSideName( battle.GetModName(), battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().side ) + _T(";\n");
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().handicap );
    s +=  _T("\t}\n");
  }
  wxLogMessage( _T("12") );

  for ( int i = 0; i < NumBots; i++ ) {

    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i + NumTeams );

    BattleBot& bot = *battle.GetBot( ordered_bots[i].index );

    // Find Team Leader.
    int TeamLeader;
    for(TeamLeader=0;TeamLeader<int(ordered_users.size());TeamLeader++){
      if(&battle.GetUser(ordered_users[TeamLeader].index)==&battle.GetUser(bot.owner))goto leader_found;
    }
    TeamLeader=0;
    wxLogMessage( _T("Internal error: team leader not found for bot! Using 0") );
    leader_found:
    /*
    if(battle.GetUser( bot.owner ).BattleStatus().spectator){
      TeamLeader=battle.GetUser( bot.owner ).BattleStatus().team;
    }else{
      TeamLeader = TeamConv[ battle.GetUser( bot.owner ).BattleStatus().team ];
    }*/

    s += wxString::Format( _T("\t\tTeamLeader=%d;\n") ,TeamLeader );
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[bot.bs.ally] );
    const char* old_locale = std::setlocale(LC_NUMERIC, "C");
    s += wxString::Format(
      _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
      (double)(bot.bs.colour.Red()/255.0),
      (double)(bot.bs.colour.Green()/255.0),
      (double)(bot.bs.colour.Blue()/255.0)
    );

    std::setlocale(LC_NUMERIC, old_locale);
    s += _T("\t\tSide=") + usync()->GetSideName( battle.GetModName(), bot.bs.side ) + _T(";\n");
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), bot.bs.handicap );

    wxString ai = WX_STRING( bot.aidll );
/*    if ( wxFileName::FileExists( sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("AI") + wxFileName::GetPathSeparator() + _T("Bot-libs") + wxFileName::GetPathSeparator() + ai + _T(".dll") ) ) {
      ai += _T(".dll");
    } else {
      ai += _T(".so");
    }*/

    //s += ("\t\tAIDLL=AI/Bot-libs/" + ai + ";\n");
    s += _T("\t\tAIDLL=") + ai + _T(";\n");
    s +=  _T("\t}\n");
  }
  wxLogMessage( _T("13") );

  long startpostype;
  battle.CustomBattleOptions()->getSingleValue( _T("startpostype"), EngineOption ).ToLong( &startpostype );
  for ( int i = 0; i < NumAllys; i++ ) {
    s += wxString::Format( _T("\t[ALLYTEAM%d]\n\t{\n"), i );

    int NumInAlly = 0;
    s += wxString::Format( _T("\t\tNumAllies=%d;\n"), NumInAlly );

   if ( (battle.GetStartRect(AllyRevConv[i]) != 0) && (startpostype == ST_Choose) ) {
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

  wxLogMessage( _T("14") );

  wxArrayString units = battle.DisabledUnits();

  s += wxString::Format( _T("\tNumRestrictions=%d;\n"), units.GetCount() );
  s += _T("\t[RESTRICT]\n");
  s += _T("\t{\n");

  for ( unsigned int i = 0; i < units.GetCount(); i++) {
    s += wxString::Format(_T("\t\tUnit%d=%s;\n"), i, units[i].c_str() );
    s += wxString::Format( _T("\t\tLimit%d=0;\n"), i );
  }
  s += _T("\t}\n");
  wxLogMessage( _T("15") );

  s += _T("\t[mapoptions]\n");
  s += _T("\t{\n");
  wxStringTripleVec optlistMap;
  battle.CustomBattleOptions()->getOptions( &optlistMap, MapOption );
  for (wxStringTripleVec::iterator it = optlistMap.begin(); it != optlistMap.end(); ++it)
  {
    s += _T("\t\t") + it->first + _T("=") + it->second.second + _T(";\n");
  }
  s += _T("\t}\n");

  wxLogMessage( _T("16") );

  s += _T("\t[modoptions]\n");
  s += _T("\t{\n");
    wxStringTripleVec optlistMod;
  battle.CustomBattleOptions()->getOptions( &optlistMod, ModOption );
  for (wxStringTripleVec::iterator it = optlistMod.begin(); it != optlistMod.end(); ++it)
  {
    s += _T("\t\t") + it->first + _T("=") + it->second.second + _T(";\n");
  }
  s += _T("\t}\n");

  wxLogMessage( _T("17") );

  s += _T("}\n");

  if ( DOS_TXT ) {
    s.Replace( _T("\n"), _T("\r\n"), true );
  }

  wxString ds = _T("[Script End]\n\n----------------------[ Debug info ]-----------------------------\nUsers:\n\n");
  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    User& tmpu = battle.GetUser( i );
    ds += tmpu.GetNick();
    ds += wxString::Format( _T(":\n  team: %d ally: %d spec: %d order: %d side: %d hand: %d sync: %d ready: %d col: %d,%d,%d\n\n"),
      tmpu.BattleStatus().team,
      tmpu.BattleStatus().ally,
      tmpu.BattleStatus().spectator,
      tmpu.BattleStatus().order,
      tmpu.BattleStatus().side,
      tmpu.BattleStatus().handicap,
      tmpu.BattleStatus().sync,
      tmpu.BattleStatus().ready,
      tmpu.BattleStatus().colour.Red(),
      tmpu.BattleStatus().colour.Green(),
      tmpu.BattleStatus().colour.Blue()
    );
  }

  wxLogMessage( _T("18") );
  ds += _T("\n\nBots: \n\n");
  for ( std::list<BattleBot*>::size_type i = 0; i < battle.GetNumBots(); i++ ) {
    BattleBot* bot = battle.GetBot(i);
    if ( bot == 0 ) {
      ds += _T( "NULL" );
      continue;
    }
    ds += bot->name + _T(" (") + bot->owner + _T(")");
    ds += wxString::Format( _T(":\n  team: %d ally: %d spec: %d order: %d side: %d hand: %d sync: %d ready: %d col: %d,%d,%d\n\n"),
      bot->bs.team,
      bot->bs.ally,
      bot->bs.spectator,
      bot->bs.order,
      bot->bs.side,
      bot->bs.handicap,
      bot->bs.sync,
      bot->bs.ready,
      bot->bs.colour.Red(),
      bot->bs.colour.Green(),
      bot->bs.colour.Blue()
    );
  }
  wxLogMessage( _T("19") );

  ds += _T("\n\nPlayerOrder: { ");
  for ( std::vector<UserOrder>::size_type i = 0; i < ordered_users.size(); i++ ) {
    ds += wxString::Format( _T(" %d,"), ordered_users[i].index );
  }
  ds += _T(" }\n\n");

  ds += _T("TeamConv: { ");
  for ( std::vector<int>::size_type i = 0; i < TeamConv.size(); i++ ) {
    ds += wxString::Format( _T(" %d,"), TeamConv[i] );
  }
  ds += _T(" }\n\n");

  ds += _T("AllyConv: { ");
  for ( std::vector<int>::size_type i = 0; i < AllyConv.size(); i++ ) {
    ds += wxString::Format( _T(" %d,"), AllyConv[i] );
  }
  ds += _T(" }\n\n");

  ds += _T("AllyRevConv: { ");
  for ( std::vector<int>::size_type i = 0; i < AllyRevConv.size(); i++ ) {
    ds += wxString::Format( _T(" %d,"), AllyRevConv[i] );
  }
  ds += _T(" }\n\n\n");

  ds += wxString::Format( _T("NumTeams: %d\n\nNumAllys: %d\n\nMyPlayerNum: %d\n\n"), NumTeams, NumAllys, MyPlayerNum );

  if ( DOS_TXT ) {
    ds.Replace( _T("\n"), _T("\r\n"), true );
  }
  wxLogMessage( _T("20") );

  wxString path = wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();

  wxFile f( wxString(path + _T("script_debug.txt")), wxFile::write );
  f.Write( _T("[Script Start]") + s );
  f.Write( ds );
  f.Close();
  wxLogMessage( _T("21") );

  return s;
}


wxString Spring::GetSPScriptTxt( SinglePlayerBattle& battle )
{
  wxString s;
  std::vector<int> AllyConv;

  int NumAllys = 0;
  int PlayerTeam = -1;

  long startpostype;
  battle.CustomBattleOptions()->getSingleValue( _T("startpostype"), EngineOption ).ToLong( &startpostype );

  wxLogMessage( _T("StartPosType=%ld"), startpostype );


  for ( unsigned int i = 0; i < battle.GetNumBots(); i++ ) {
    BattleBot* bot;
    bot = battle.GetBot( i );
    ASSERT_LOGIC( bot != 0, _T("bot == 0") );
    if ( bot->aidll == _T("") ) PlayerTeam = i;
    if(bot->bs.ally>int(AllyConv.size())-1){
      AllyConv.resize(bot->bs.ally+1,-1);
    }
    if( AllyConv[bot->bs.ally] == -1 ) AllyConv[bot->bs.ally] = NumAllys++;
  }

  ASSERT_LOGIC( PlayerTeam != -1, _T("no player found") );

  // Start generating the script.
  s  = wxString::Format( _T("[GAME]\n{\n") );

  //s += wxString::Format( _T("\tMapname=%s;\n"), bo.mapname.c_str() );
  s += _T("\tMapname=") + battle.GetMapName() + _T(";\n");
  s += _T("\tGameType=" )+ usync()->GetModArchive(usync()->GetModIndex(battle.GetModName())) + _T(";\n");
  unsigned long uhash;
  battle.GetModHash().ToULong(&uhash);
  s += wxString::Format( _T("\tModHash=%ld;\n"), (int)uhash );
  wxStringTripleVec optlistEng;
  battle.CustomBattleOptions()->getOptions( &optlistEng, EngineOption );
  for (wxStringTripleVec::iterator it = optlistEng.begin(); it != optlistEng.end(); ++it)
  {
    s += _T("\t") + it->first + _T("=") + it->second.second + _T(";\n");
  }

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
    BattleBot* bot;
    if ( startpostype == ST_Pick) bot = battle.GetBot( i );
    else bot = battle.GetBotByStartPosition( i );
    ASSERT_LOGIC( bot != 0, _T("bot == 0") );
    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i );
    if ( startpostype == ST_Pick ){
      s += wxString::Format( _T("\t\tStartPosX=%d;\n"), bot->posx );
      s += wxString::Format( _T("\t\tStartPosZ=%d;\n"), bot->posy );
    }
    s += _T("\t\tTeamLeader=0;\n");
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[bot->bs.ally] );
    const char* old_locale = std::setlocale(LC_NUMERIC, "C");
    s += wxString::Format( _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
           (double)(bot->bs.colour.Red()/255.0),
           (double)(bot->bs.colour.Green()/255.0),
           (double)(bot->bs.colour.Blue()/255.0)
         );
    std::setlocale(LC_NUMERIC, old_locale);
    s += _T("\t\tSide=") + usync()->GetSideName( battle.GetModName(), bot->bs.side ) + _T(";\n");
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), bot->bs.handicap );
    if ( bot->aidll != _T("") ) {
      wxString ai = bot->aidll;
      /*if ( wxFileName::FileExists( sett().GetSpringDir() + wxFileName::GetPathSeparator() + _T("AI") + wxFileName::GetPathSeparator() + _T("Bot-libs") + wxFileName::GetPathSeparator() + ai + _T(".dll") ) ) {
        ai += _T(".dll");
      } else {
        ai += _T(".so");
      }*/
      s += _T("\t\tAIDLL=") + ai + _T(";\n");
      //s += _T("\t\tAIDLL=AI/Bot-libs/") + ai + _T(";\n");
    }
    s +=  _T("\t}\n");
  }

  for ( int i = 0; i < NumAllys; i++ ) {
    s += wxString::Format( _T("\t[ALLYTEAM%d]\n\t{\n"), i );
      s += _T("\t\tNumAllies=0;\n");
    s += _T("\t}\n");
  }


  wxArrayString units = battle.DisabledUnits();

  s += wxString::Format( _T("\tNumRestrictions=%d;\n"), units.GetCount() );
  s += _T("\t[RESTRICT]\n");
  s += _T("\t{\n");

  for ( unsigned int i = 0; i < units.GetCount(); i++) {
    s += wxString::Format(_T("\t\tUnit%d=%s;\n"), i, units[i].c_str() );
    s += wxString::Format( _T("\t\tLimit%d=0;\n"), i );
  }
  s += _T("\t}\n");

  s += _T("\t[mapoptions]\n");
  s += _T("\t{\n");
  wxStringTripleVec optlistMap;
  battle.CustomBattleOptions()->getOptions( &optlistMap, MapOption );
  for (wxStringTripleVec::iterator it = optlistMap.begin(); it != optlistMap.end(); ++it)
  {
    s += _T("\t\t") + it->first + _T("=") + it->second.second + _T(";\n");
  }
  s += _T("\t}\n");

  s += _T("\t[modoptions]");
  s += _T("\t{\n");
    wxStringTripleVec optlistMod;
  battle.CustomBattleOptions()->getOptions( &optlistMod, ModOption );
  for (wxStringTripleVec::iterator it = optlistMod.begin(); it != optlistMod.end(); ++it)
  {
    s += _T("\t\t") + it->first + _T("=") + it->second.second + _T(";\n");
  }
  s += _T("\t}\n");

  s += _T("}\n");

  if ( DOS_TXT ) {
    s.Replace( _T("\n"), _T("\r\n"), true );
  }

  return s;
}
