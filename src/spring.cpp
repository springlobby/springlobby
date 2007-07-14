//
// Class: Spring
//


#include "ui.h"


void SpringProcess::OnTerminate( int pid, int status )
{
  m_sp.OnTerminated();
}


bool Spring::Run( Battle& battle )
{
  if ( m_running ) {
    debug_error( "Spring allready running!" );
    return false;
  }
  
  try {

    if ( !wxFile::Access( wxString(_T("script_springlobby.txt")), wxFile::write ) ) {
      debug_error( "Access denied to script_springlobby.txt." );
    }

    wxFile f( wxString(_T("script_springlobby.txt")), wxFile::write );
    f.Write( GetScriptTxt(battle) );
    f.Close();

  } catch (...) {
    debug_error( "Couldn't write script_springlobby.txt" ); //!@todo Some message to user.
    return false;
  }

  if ( m_process == NULL ) m_process = new SpringProcess( *this );
  if ( wxExecute( WX_STRING(sett().GetSpringUsedLoc()) + _T(" script_springlobby.txt"), wxEXEC_ASYNC, m_process ) == 0 ) {
    return false;
  }
  m_running = true;
  return true;
}


void Spring::OnTerminated()
{
  m_running = false;
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
  
  int NumTeams=0, NumAllys=0, LastOrder=-1,Lowest=-1;
  int PlayerOrder[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int TeamConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyRevConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  for ( int i = 0; i < battle.GetNumUsers(); i++ ) {
    Lowest = -1;
    // Find next player in the order they were sent from the server.
    for ( int gl = 0; gl < battle.GetNumUsers(); gl++ ) {
      if ( battle.GetUser(gl).GetBattleStatus().order <= LastOrder ) continue;
      if ( Lowest == -1 ) Lowest = gl;
      else if ( battle.GetUser(gl).GetBattleStatus().order < battle.GetUser(Lowest).GetBattleStatus().order ) Lowest = gl;
    }
    LastOrder = battle.GetUser(Lowest).GetBattleStatus().order;
    User& u = battle.GetUser( Lowest );

    PlayerOrder[i] = Lowest;

    UserBattleStatus bs = u.GetBattleStatus();
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

  BattleOptions bo = battle.opts();

  // Start generating the script.
  s  = wxString::Format( _T("[GAME]\n{\n") );

  //s += wxString::Format( _T("\tMapname=%s;\n"), bo.mapname.c_str() );
  s += WX_STRING(("\tMapname=" + bo.mapname + ";\n"));
  s += wxString::Format( _T("\tStartMetal=%d;\n"), bo.startmetal );
  s += wxString::Format( _T("\tStartEnergy=%d;\n"), bo.startenergy );
  s += wxString::Format( _T("\tMaxUnits=%d;\n"), bo.maxunits );
  s += wxString::Format( _T("\tStartPosType=%d;\n"), bo.starttype );
  s += wxString::Format( _T("\tGameMode=0;\n") );
  s += WX_STRING(("\tGameType=" + usync().GetModArchive(usync().GetModIndex(bo.modname)) + ";\n"));
  s += wxString::Format( _T("\tLimitDGun=%d;\n"), bo.limitdgun?1:0 );
  s += wxString::Format( _T("\tDiminishingMMs=%d;\n"), bo.dimmms?1:0 );
  s += wxString::Format( _T("\tGhostedBuildings=%d;\n\n"), bo.ghostedbuildings?1:0 );

  if ( battle.IsFounderMe() ) s += wxString::Format( _T("\tHostIP=localhost;\n") );
  else s += WX_STRING(("\tHostIP=" + bo.ip + ";\n"));
  s += wxString::Format( _T("\tHostPort=%d;\n\n"), bo.port );

  s += wxString::Format( _T("\tMyPlayerNum=%d;\n\n"), battle.GetMyPlayerNum() );

  s += wxString::Format( _T("\tNumPlayers=%d;\n"), battle.GetNumUsers() );
  s += wxString::Format( _T("\tNumTeams=%d;\n"), NumTeams );
  s += wxString::Format( _T("\tNumAllyTeams=%d;\n\n"), NumAllys );
  
  for ( int i = 0; i < battle.GetNumUsers(); i++ ) {
    s += wxString::Format( _T("\t[PLAYER%d]\n"), i );
    s += wxString::Format( _T("\t{\n") );
    s += WX_STRING(("\t\tname=" + battle.GetUser( PlayerOrder[i] ).GetNick() + ";\n"));
    s += wxString::Format( _T("\t\tSpectator=%d;\n"), battle.GetUser( PlayerOrder[i] ).GetBattleStatus().spectator?1:0 );
    if ( !battle.GetUser( PlayerOrder[i] ).GetBattleStatus().spectator ) {
      s += wxString::Format( _T("\t\tteam=%d;\n"), TeamConv[battle.GetUser( PlayerOrder[i] ).GetBattleStatus().team] );
    }
    s += wxString::Format( _T("\t}\n") );
  }

  s += _T("\n");

  for ( int i = 0; i < NumTeams; i++ ) {
    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i );

    // Find Team Leader.
    int TeamLeader = -1;
    for( int tlf = 0; tlf < battle.GetNumUsers(); tlf++ ) {
      // First Player That Is In The Team Is Leader.
      if ( TeamConv[battle.GetUser( PlayerOrder[tlf] ).GetBattleStatus().team] == i ) {
        TeamLeader = tlf;
        break;
      }
    }

    s += wxString::Format( _T("\t\tTeamLeader=%d;\n") ,TeamLeader );
    s += wxString::Format( _T("\t\tAllyTeam=%d;\n"), AllyConv[battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().ally] );
    s += wxString::Format( _T("\t\tRGBColor=%.5f %.5f %.5f;\n"),
           (double)(battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().color_r/255.0),
           (double)(battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().color_g/255.0),
           (double)(battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().color_b/255.0)
         );
    debug( i2s(battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().side) );
    s += WX_STRING(("\t\tSide=" + usync().GetSideName( battle.opts().modname, battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().side ) + ";\n"));
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), battle.GetUser( PlayerOrder[TeamLeader] ).GetBattleStatus().handicap );
    s +=  _T("\t}\n");
  }

  for ( int i = 0; i < NumAllys; i++ ) {
    s += wxString::Format( _T("\t[ALLYTEAM%d]\n\t{\n"), i );

    int NumInAlly = 0;
    s += wxString::Format( _T("\t\tNumAllies=%d;\n"), NumInAlly );

   if ( (battle.GetStartRect(AllyRevConv[i]) != NULL) && (bo.starttype == ST_Choose) ) {
      BattleStartRect* sr = (BattleStartRect*)battle.GetStartRect(AllyRevConv[i]);
      s += wxString::Format( _T("\t\tStartRectLeft=%.3f;\n"), sr->left / 200.0 );
      s += wxString::Format( _T("\t\tStartRectTop=%.3f;\n"), sr->top / 200.0 );
      s += wxString::Format( _T("\t\tStartRectRight=%.3f;\n"), sr->right / 200.0 );
      s += wxString::Format( _T("\t\tStartRectBottom=%.3f;\n"), sr->bottom / 200.0 );
    }

    s +=  _T("\t}\n");
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
