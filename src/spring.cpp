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
    if ( m_running ) {
        wxLogError( _T("Spring already running!") );
        return false;
    }

  wxLogMessage( _T("launching spring with replay: ") + filename );

  wxString cmd =  _T("\"") + sett().GetCurrentUsedSpringBinary() + _T("\" \"") + filename + _T("\"") ;
  wxLogMessage( _T("cmd: %s"), cmd.c_str() );
  wxSetWorkingDirectory( sett().GetCurrentUsedDataDir() );
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

bool Spring::Run( Battle& battle )
{
  if ( m_running ) {
    wxLogError( _T("Spring already running!") );
    return false;
  }

  wxString path = sett().GetCurrentUsedDataDir() + wxFileName::GetPathSeparator();

  wxLogMessage( _T("Path to script: %sscript.txt"), path.c_str() );

  try {

    if ( !wxFile::Access( path +  _T("script.txt"), wxFile::write ) ) {
      wxLogError( _T("Access denied to script.txt.") );
    }



    wxFile f( path + _T("script.txt"), wxFile::write );
    battle.DisableHostStatusInProxyMode( true );
    f.Write( WriteScriptTxt(battle) );
    battle.DisableHostStatusInProxyMode( false );
    f.Close();

  } catch (...) {
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
    f.Write( WriteSPScriptTxt(battle) );
    f.Close();

  } catch (...) {
    wxLogError( _T("Couldn't write script.txt") );
    return false;
  }

  wxString cmd =  _T("\"") + sett().GetCurrentUsedSpringBinary() + _T("\" \"") + path + _T("script.txt\"");
  wxSetWorkingDirectory( sett().GetCurrentUsedDataDir() );
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


wxString Spring::WriteScriptTxt( Battle& battle )
{
  wxLogMessage(_T("0 WriteScriptTxt called "));

  wxString ret;

  TDFWriter tdf(ret);

  int  NumTeams=0, MyPlayerNum=-1;

  std::vector<UserOrder> ordered_users;
  std::vector<UserOrder> ordered_bots;
  std::vector<int> TeamConv, AllyConv, AllyRevConv;
  /// AllyRevConv.size() gives number of allies

  wxLogMessage(_T("numusers: %d"),battle.GetNumUsers() );

  /// Fill ordered_users and sort it
  for ( UserList::user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ){
    UserOrder tmp;
    tmp.index=i;
    tmp.order=battle.GetUser(i).BattleStatus().order;
    ordered_users.push_back(tmp);
  }
  std::sort(ordered_users.begin(),ordered_users.end());

  /// process ordered users, convert teams and allys
  for(int i=0;i<int(ordered_users.size());++i){
    wxLogMessage(_T("order %d index %d"), ordered_users[i].order, ordered_users[i].index);
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



  //BattleOptions bo = battle.opts();

  // Start generating the script.
  tdf.EnterSection(_T("GAME"));

  tdf.Append(_T("Mapname"),battle.GetHostMapName());
  tdf.Append(_T("GameType"),usync().GetModArchive(usync().GetModIndex(battle.GetHostModName())));


  unsigned long uhash;
  battle.LoadMod().hash.ToULong(&uhash);

  tdf.Append(_T("ModHash"),int(uhash));


  OptionsWrapper::wxStringTripleVec optlistEng = battle.CustomBattleOptions().getOptions( OptionsWrapper::EngineOption );
  for (OptionsWrapper::wxStringTripleVec::const_iterator it = optlistEng.begin(); it != optlistEng.end(); ++it)
  {
    tdf.Append(it->first,it->second.second);
  }

  if ( battle.IsFounderMe() ){
    tdf.Append(_T("HostIP"),_T("localhost"));
  }else{
    tdf.Append(_T("HostIP"),battle.GetHostIp());
  }

  if ( battle.IsFounderMe() && battle.GetNatType() == IBattle::NAT_Hole_punching ) {
    tdf.Append(_T("HostPort"),battle.GetMyInternalUdpSourcePort());
  } else {
    tdf.Append(_T("HostPort"),battle.GetHostPort());
  }
  tdf.Append(_T("IsHost"),battle.IsFounderMe());

  tdf.AppendLineBreak();

  if ( !battle.IsFounderMe() )
  {
    if ( battle.GetNatType() == IBattle::NAT_Fixed_source_ports )
    {
      tdf.Append(_T("SourcePort"), FIRST_UDP_SOURCEPORT + MyPlayerNum -1);
    }
    else if ( battle.GetNatType() == IBattle::NAT_Hole_punching )
    {
      tdf.Append(_T("SourcePort"), battle.GetMyInternalUdpSourcePort());
    }
  }

  tdf.AppendLineBreak();

  tdf.Append(_T("MyPlayerNum"),MyPlayerNum);
  tdf.Append(_T("MyPlayerName"), battle.GetMe().GetNick() );

  tdf.Append(_T("NumPlayers"),battle.GetNumUsers());
  tdf.Append(_T("NumTeams"), NumTeams + NumBots);
  tdf.Append(_T("NumAllyTeams"), NumAllys);

  for ( UserList::user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    tdf.EnterSection(_T("PLAYER")+i2s(i));

    tdf.Append(_T("name"),battle.GetUser( ordered_users[i].index ).GetNick());

    tdf.Append(_T("countryCode"),battle.GetUser( ordered_users[i].index ).GetCountry().Lower());
    tdf.Append(_T("Spectator"), battle.GetUser( ordered_users[i].index ).BattleStatus().spectator?1:0);

    if ( !(battle.GetUser( ordered_users[i].index ).BattleStatus().spectator) ) {
      tdf.Append(_T("team"), TeamConv[battle.GetUser( ordered_users[i].index ).BattleStatus().team]);

    }
    tdf.LeaveSection();
  }

  //s += _T("\n");
  tdf.AppendLineBreak();


  for ( int i = 0; i < NumTeams; i++ ) {
    tdf.EnterSection(_T("TEAM")+i2s(i));

    // Find Team Leader.
    int TeamLeader = -1;

    for( UserList::user_map_t::size_type tlf = 0; tlf < battle.GetNumUsers(); tlf++ ) {/// change: moved check if spectator above use of TeamConv array, coz TeamConv array does not include spectators.

      // Make sure player is not spectator.
      if ( battle.GetUser( ordered_users[tlf].index ).BattleStatus().spectator ) continue;

      // First Player That Is In The Team Is Leader.
      if ( TeamConv[battle.GetUser( ordered_users[tlf].index ).BattleStatus().team] == i ) {



        // Assign as team leader.
        TeamLeader = tlf;
        break;
      }

    }

    tdf.Append(_T("TeamLeader"),TeamLeader);
    tdf.Append(_T("AllyTeam"),AllyConv[battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().ally]);

    wxString colourstring =
      TowxString( battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().colour.Red()/255.0 ) + _T(' ') +
      TowxString( battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().colour.Green()/255.0 ) + _T(' ') +
      TowxString( battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().colour.Blue()/255.0 );
    tdf.Append(_T("RGBColor"), colourstring);

    wxLogMessage( _T("%d"), battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().side );

    tdf.Append(_T("Side"),usync().GetSideName( battle.GetHostModName(), battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().side ));
    tdf.Append(_T("Handicap"), battle.GetUser( ordered_users[TeamLeader].index ).BattleStatus().handicap);

    tdf.LeaveSection();
  }

  for ( int i = 0; i < NumBots; i++ ) {

    tdf.EnterSection(_T("TEAM")+i2s(i + NumTeams));

    BattleBot& bot = *battle.GetBot( ordered_bots[i].index );

    // Find Team Leader.
    int TeamLeader;
    for(TeamLeader=0;TeamLeader<int(ordered_users.size());TeamLeader++){
      if(&battle.GetUser(ordered_users[TeamLeader].index)==&battle.GetUser(bot.owner))goto leader_found;
    }
    TeamLeader=0;
    wxLogMessage( _T("Internal error: team leader not found for bot! Using 0") );
    leader_found:

    tdf.Append(_T("TeamLeader"),TeamLeader);

    tdf.Append(_T("AllyTeam"),AllyConv[bot.bs.ally]);

    wxString colourstring =
      TowxString( bot.bs.colour.Red()/255.0f ) + _T(' ') +
      TowxString( bot.bs.colour.Green()/255.0f ) + _T(' ') +
      TowxString( bot.bs.colour.Blue()/255.0f );
    tdf.Append(_T("RGBColor"), colourstring);

    tdf.Append(_T("Side"),usync().GetSideName( battle.GetHostModName(), bot.bs.side ));


    tdf.Append(_T("Handicap"),bot.bs.handicap);
    wxString ai = WX_STRING( bot.aidll );
    tdf.Append(_T("AIDLL"),ai);
    tdf.LeaveSection();
  }

  long startpostype;
  battle.CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ).ToLong( &startpostype );
  for ( int i = 0; i < NumAllys; i++ ) {
    tdf.EnterSection(_T("ALLYTEAM")+i2s(i));

    int NumInAlly = 0;    // ???
    //s += wxString::Format( _T("\t\tNumAllies=%d;\n"), NumInAlly );
    tdf.Append(_T("NumAllies"),NumInAlly);

   if ( ( battle.GetStartRect(AllyRevConv[i]).exist ) && (startpostype == IBattle::ST_Choose) ) {
      BattleStartRect sr = battle.GetStartRect(AllyRevConv[i]);
      if ( sr.IsOk() )
      {
          const char* old_locale = std::setlocale(LC_NUMERIC, "C");

          tdf.Append(_T("StartRectLeft"),wxString::Format( _T("%.3f"), sr.left / 200.0 ));
          tdf.Append(_T("StartRectTop"),wxString::Format( _T("%.3f"), sr.top / 200.0 ));
          tdf.Append(_T("StartRectRight"),wxString::Format( _T("%.3f"), sr.right / 200.0 ));
          tdf.Append(_T("StartRectBottom"),wxString::Format( _T("%.3f"), sr.bottom / 200.0 ));

          std::setlocale(LC_NUMERIC, old_locale);
      }
    }

    tdf.LeaveSection();
  }

  wxArrayString units = battle.DisabledUnits();

  tdf.Append(_T("NumRestrictions"),units.GetCount());
  tdf.EnterSection(_T("RESTRICT"));
  for ( unsigned int i = 0; i < units.GetCount(); i++) {
    tdf.Append(_T("Unit")+i2s(i),units[i].c_str());
    tdf.Append(_T("Limit")+i2s(i),_T("0"));
  }
  tdf.LeaveSection();

  tdf.EnterSection(_T("mapoptions"));

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

  tdf.Append(_T("MyPlayerNum"),"0");
  tdf.Append(_T("MyPlayerName"), _T("Player") );

  tdf.Append(_T("NumPlayers"),"1");

  tdf.Append(_T("IsHost"), battle.IsFounderMe());

  tdf.Append(_T("NumTeams"),battle.GetNumBots());

  tdf.Append(_T("NumAllyTeams"),NumAllys);

  tdf.EnterSection(_T("PLAYER0"));
    tdf.Append(_T("name"),"Player");
    tdf.Append(_T("Spectator"),"0");
    tdf.Append(_T("team"),PlayerTeam);
  tdf.LeaveSection();

  for ( unsigned int i = 0; i < battle.GetNumBots(); i++ ) {
    BattleBot* bot;
    if ( startpostype == IBattle::ST_Pick) bot = battle.GetBot( i );
    else bot = battle.GetBotByStartPosition( i );
    ASSERT_LOGIC( bot != 0, _T("bot == 0") );

    tdf.EnterSection(_T("TEAM")+i2s(i));

    if ( startpostype == IBattle::ST_Pick ){
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

    if ( bot->aidll != _T("") ) {
      wxString ai = bot->aidll;
      tdf.Append(_T("AIDLL"),ai);
    }
    tdf.LeaveSection();
  }

  for ( int i = 0; i < NumAllys; i++ ) {
    tdf.EnterSection(_T("ALLYTEAM")+i2s(i));
    tdf.Append(_T("NumAllies"),"0");
    tdf.LeaveSection();
  }
  wxArrayString units = battle.DisabledUnits();
  tdf.Append(_T("NumRestrictions"),units.GetCount());

  tdf.EnterSection(_T("RESTRICT"));

  for ( unsigned int i = 0; i < units.GetCount(); i++) {
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
