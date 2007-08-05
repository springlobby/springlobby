//
// Class: Spring
//

#include <wx/file.h>
#include <wx/intl.h>
#include <wx/arrstr.h>
#include <clocale>

#include "spring.h"
#include "springprocess.h"
#include "ui.h"
#include "utils.h"
#include "settings.h"
#include "userlist.h"
#include "battle.h"
#include "user.h"
#include "iunitsync.h"
#include "nonportable.h"

Spring::Spring( Ui& ui ) :
  m_ui(ui),
  m_process(0),
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

  if ( m_process == 0 ) m_process = new SpringProcess( *this );
  if ( wxExecute( WX_STRING(sett().GetSpringUsedLoc()) + _T(" script_springlobby.txt"), wxEXEC_ASYNC, m_process ) == 0 ) {
    return false;
  }
  m_running = true;
  return true;
}


bool Spring::TestSpringBinary()
{
  wxArrayString res;
  wxArrayString err;
  wxString bin = WX_STRING(sett().GetSpringUsedLoc()) + _T(" ");

  /*
   * This hack is necessary, because spring returns -1 on purpose in 0.75b2, when ever you query it's version. I have sent a patch, and hopefully this will change in future versions of spring. Because of the exact return value, wxExecute fails to record the single line of standard output we need. This seems to be a bug in wxWidgets 2.8 series in windows, but I have not yet sent a bug report. If either project fixes their bug, this can be done portably again.
   */
#ifdef WIN32
  bin += _T("//V");
  int ret = wxExecute( bin, res, err, wxEXEC_SYNC );
  return (ret != -1);
#else
  bin += SPRING_VERSION_PARAM;
  int ret = wxExecute( bin, res, err, wxEXEC_SYNC );
  if ( res.GetCount() == 0 ) return false;
  return res[0].Contains(_T("Spring"));
#endif

  //wxString foo = wxString::Format(_T("%d %d %d %d"), ret , res.GetCount() == 1 , err.GetCount() == 0 , res[0] == _T("Spring 0.74b3"));
  //debug(STD_STRING(foo));
  // we can't trust ret value at all :(
  // FIXME check against something else than a hardcoded spring version, f.ex. the one tasserver gives
  //return /* ret == 1 && */ res.GetCount() == 1 && err.GetCount() == 0 /* && res[0].Contains(_T("Spring")) == _T("Spring 0.74b3") */;
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

  int NumTeams=0, NumAllys=0, LastOrder=-1,Lowest=-1,MyPlayerNum=-1;
  int PlayerOrder[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int TeamConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyRevConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  for ( user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ ) {
    Lowest = -1;
    // Find next player in the order they were sent from the server.
    for ( user_map_t::size_type gl = 0; gl < battle.GetNumUsers(); gl++ ) {
      if ( battle.GetUser(gl).BattleStatus().order <= LastOrder ) continue;
      if ( Lowest == -1 ) Lowest = gl;
      else if ( battle.GetUser(gl).BattleStatus().order < battle.GetUser(Lowest).BattleStatus().order ) Lowest = gl;
    }
    LastOrder = battle.GetUser(Lowest).BattleStatus().order;
    User& u = battle.GetUser( Lowest );

    PlayerOrder[i] = Lowest;
    if ( &battle.GetUser(Lowest) == &battle.GetMe() ) MyPlayerNum = i;

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

  BattleOptions bo = battle.opts();

  // Start generating the script.
  s  = wxString::Format( _T("[GAME]\n{\n") );

  //s += wxString::Format( _T("\tMapname=%s;\n"), bo.mapname.c_str() );
  s += WX_STRING(("\tMapname=" + bo.mapname + ";\n"));
  s += wxString::Format( _T("\tStartMetal=%d;\n"), bo.startmetal );
  s += wxString::Format( _T("\tStartEnergy=%d;\n"), bo.startenergy );
  s += wxString::Format( _T("\tMaxUnits=%d;\n"), bo.maxunits );
  s += wxString::Format( _T("\tStartPosType=%d;\n"), bo.starttype );
  s += wxString::Format( _T("\tGameMode=%d;\n"), bo.gametype );
  s += WX_STRING(("\tGameType=" + usync()->GetModArchive(usync()->GetModIndex(bo.modname)) + ";\n"));
  s += wxString::Format( _T("\tLimitDGun=%d;\n"), bo.limitdgun?1:0 );
  s += wxString::Format( _T("\tDiminishingMMs=%d;\n"), bo.dimmms?1:0 );
  s += wxString::Format( _T("\tGhostedBuildings=%d;\n\n"), bo.ghostedbuildings?1:0 );

  if ( battle.IsFounderMe() ) s += wxString::Format( _T("\tHostIP=localhost;\n") );
  else s += WX_STRING(("\tHostIP=" + bo.ip + ";\n"));
  s += wxString::Format( _T("\tHostPort=%d;\n\n"), bo.port );

  s += wxString::Format( _T("\tMyPlayerNum=%d;\n\n"), MyPlayerNum );

  s += wxString::Format( _T("\tNumPlayers=%d;\n"), battle.GetNumUsers() );
  s += wxString::Format( _T("\tNumTeams=%d;\n"), NumTeams );
  s += wxString::Format( _T("\tNumAllyTeams=%d;\n\n"), NumAllys );

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

  s += _T("\n");

  for ( int i = 0; i < NumTeams; i++ ) {
    s += wxString::Format( _T("\t[TEAM%d]\n\t{\n"), i );

    // Find Team Leader.
    int TeamLeader = -1;

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
    s += WX_STRING(("\t\tSide=" + usync()->GetSideName( battle.opts().modname, battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().side ) + ";\n"));
    s += wxString::Format( _T("\t\tHandicap=%d;\n"), battle.GetUser( PlayerOrder[TeamLeader] ).BattleStatus().handicap );
    s +=  _T("\t}\n");
  }

  for ( int i = 0; i < NumAllys; i++ ) {
    s += wxString::Format( _T("\t[ALLYTEAM%d]\n\t{\n"), i );

    int NumInAlly = 0;
    s += wxString::Format( _T("\t\tNumAllies=%d;\n"), NumInAlly );

   if ( (battle.GetStartRect(AllyRevConv[i]) != 0) && (bo.starttype == ST_Choose) ) {
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

  wxFile f( wxString(_T("script_springlobby_debug.txt")), wxFile::write );
  f.Write( _T("[Script Start]") + s );
  f.Write( ds );
  f.Close();
 
  return s;
}
