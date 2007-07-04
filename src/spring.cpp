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
  if ( m_running ) return false;
  
  try {

    if ( !wxFile::Access( wxString(_("script_springlobby.txt")), wxFile::write ) ) {
      debug_error( "Access denied to script_springlobby.txt." );
    }

    wxFile f( wxString(_("script_springlobby.txt")), wxFile::write );
    f.Write( GetScriptTxt(battle) );
    f.Close();

  } catch (...) {
    debug_error( "Couldn't write script_springlobby.txt" ); //!@todo Some message to user.
    return false;
  }

  if ( m_process == NULL ) m_process = new SpringProcess( *this );
  if ( wxExecute( WX_STRING(sett().GetSpringUsedLoc()) + _(" script_springlobby.txt"), wxEXEC_ASYNC, m_process ) == 0 ) {
    return false;
  }
  m_running = true;
  return true;
}


void Spring::OnTerminated()
{
  m_ui.OnSpringTerminated( true );
}


wxString Spring::GetScriptTxt( Battle& battle )
{
  wxString s;
  
  int NumTeams=0, NumAllys=0,LastOrder=-1,Lowest=-1;
  int TeamConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  int AllyConv[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  for ( int i = 0; i < battle.GetNumUsers(); i++ ) {
    Lowest = -1;
    for ( int gl = 0; gl < battle.GetNumUsers(); gl++ ) {
      if ( battle.GetUser(gl).GetBattleStatus().order <= LastOrder ) continue;
      if ( Lowest == -1 ) Lowest = gl;
      else if ( battle.GetUser(gl).GetBattleStatus().order < battle.GetUser(Lowest).GetBattleStatus().order ) Lowest = gl;
    }
    LastOrder = battle.GetUser(Lowest).GetBattleStatus().order;
    User& u = battle.GetUser( Lowest );
    UserBattleStatus bs = u.GetBattleStatus();
    if ( bs.spectator ) continue;

    if ( TeamConv[bs.team] == -1 ) {
      TeamConv[bs.team] = NumTeams;
      NumTeams++;
    }

    if ( AllyConv[bs.ally] == -1 ) {
      AllyConv[bs.ally] = NumAllys;
      NumAllys++;
    }

  }  

  BattleOptions bo = battle.opts();
  s  = _("[GAME]") + EL;
  s += _("{") + EL;
  s +=   _("\tMapname=") + WX_STRING(bo.mapname) + _(";") + EL;
  s +=   _("\tStartMetal=") + WX_STRING(i2s( bo.startmetal )) + _(";") + EL;
  s +=   _("\tStartEnergy=") + WX_STRING(i2s( bo.startenergy )) + _(";") + EL;
  s +=   _("\tMaxUnits=") + WX_STRING(i2s( bo.maxunits )) + _(";") + EL;
  s +=   _("\tStartPosType=") + WX_STRING(i2s( bo.starttype )) + _(";") + EL;
  s +=   _("\tGameMode=0;") + EL;
  s +=   _("\tGameType=") + WX_STRING( usync().GetModArchive(usync().GetModIndex(bo.modname)) ) + _(";") + EL;
  s +=   _("\tLimitDGun=") + WX_STRING(i2s( bo.limitdgun?1:0 )) + _(";") + EL;
  s +=   _("\tDiminishingMMs=") + WX_STRING(i2s( bo.dimmms?1:0 )) + _(";") + EL;
  s +=   _("\tGhostedBuildings=") + WX_STRING(i2s( bo.ghostedbuildings?1:0 )) + _(";") + EL + EL;

  if ( battle.IsFounderMe() ) s += _("\tHostIP=localhost;") + EL;
  else s +=   _("\tHostIP=") + WX_STRING( bo.ip ) + _(";") + EL;
  s +=   _("\tHostPort=") + WX_STRING(i2s( bo.port )) + _(";") + EL + EL;

  s +=   _("\tMyPlayerNum=") + WX_STRING(i2s( battle.GetMyPlayerNum() )) + _(";") + EL + EL;

  s +=   _("\tNumPlayers=") + WX_STRING(i2s( battle.GetNumUsers() )) + _(";") + EL;
  s +=   _("\tNumTeams=") + WX_STRING(i2s( NumTeams )) + _(";") + EL;
  s +=   _("\tNumAllyTeams=") + WX_STRING(i2s( NumAllys )) + _(";") + EL + EL;
  
  for ( int i = 0; i < battle.GetNumUsers(); i++ ) {
    s +=   _("\t[PLAYER") + WX_STRING(i2s( i )) + _("]") + EL;
    s +=   _("\t{") + EL;
    s +=     _("\t\tname=") + WX_STRING( battle.GetUser( i ).GetNick() ) + _(";") + EL;
    s +=     _("\t\tSpectator=") + WX_STRING(i2s( battle.GetUser( i ).GetBattleStatus().spectator?1:0 )) + _(";") + EL;
    if ( !battle.GetUser( i ).GetBattleStatus().spectator ) {
      s +=     _("\t\tteam=") + WX_STRING(i2s( TeamConv[battle.GetUser( i ).GetBattleStatus().team] )) + _(";") + EL;
    }
    s +=  _("\t}") + EL + EL;
  }

  return s;
}

/*
                    s += "\t\tname="+u.getPlayername ()+";\n";
                    s += "\t\tSpectator="+BooltoInt (u.isSpec ())+";\n";
                    if (!u.isSpec ()) {
                        s += "\t\tteam="+td.GetTeamNo (Misc.getTeamNoFromBattleStatus (u.getBattlestatus ()))+";\n";
                    }
                    s += "\t}\n";
                }
                
                // TEAMS
                s += "\n";
                //////
                TreeMap<Integer,TeamEntry> orderedTeams = td.getTeamList ();
                for (int i=0;i<td.getTeamCount ();i++) {
                    s += "\t[TEAM"+i+"]\n";
                    s += "\t{\n";
                    
                    
                    int currentKey = orderedTeams.firstKey ();
                    TeamEntry te = orderedTeams.remove (currentKey);
                    
                    
                    s += "\t\tTeamLeader="+te.getTeamLeader ()+";\n";
                    s += "\t\tAllyTeam="+te.getAllyNo ()+";\n";
                    
                    Color col = te.getTeamColor ();
                    String redCol = ""+(col.getRed ()/255d);
                    String greenCol = ""+(col.getGreen ()/255d);
                    String blueCol = ""+(col.getBlue ()/255d);
                    
                    if (redCol.length () > 7) redCol = redCol.substring (0,7);
                    if (greenCol.length () > 7) greenCol = greenCol.substring (0,7);
                    if (blueCol.length () > 7) blueCol = blueCol.substring (0,7);
                    
                    redCol += getMissingZeros (redCol.length ());
                    greenCol += getMissingZeros (greenCol.length ());
                    blueCol += getMissingZeros (blueCol.length ());
                    
                    s += "\t\tRGBColor="+redCol+" "+greenCol+" "+blueCol+";\n";
                    s += "\t\tSide="+JSync.GetSideName (te.getSide ())+";\n";//JUnitSync.GetSideName (te.getSide ()) 
                    s += "\t\tHandicap="+te.getHandicap ()+";\n";
                    
                    s += "\t}\n";
                    
                }
                
                // ALLYS
                
                TreeMap<Integer,AllyEntry> orderedAllys = ad.getAllyList ();
                for (int i=0;i<ad.getAllyCount ();i++) {
                    s += "\t[ALLYTEAM"+i+"]\n";
                    s += "\t{\n";
                    
                    int currentKey = orderedAllys.firstKey ();
                    AllyEntry te = orderedAllys.remove (currentKey);
                    
                    s += "\t\tNumAllies="+0+";\n";
                    RectEntry re = RectHandler.getRectForAlly (ad.untransform (i));
                    if (re != null) {
                        String[] data = re.getRectDataStr ();
                        
                        s += "\t\tStartRectLeft="+data[0]+";\n";
                        s += "\t\tStartRectTop="+data[1]+";\n";
                        s += "\t\tStartRectRight="+data[2]+";\n";
                        s += "\t\tStartRectBottom="+data[3]+";\n";
                    }
                    s += "\t}\n";
                    
                }
                
               
                s += "\n";
                s += "\tNumRestrictions=0;\n";
                s += "}";
            }catch(Exception e){
            }
        }else{
            s="n/a";
        }
        return s;
    }
*/
