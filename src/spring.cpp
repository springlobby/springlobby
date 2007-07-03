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

    if ( !wxFile::Access( (wxChar*)"script.txt", wxFile::write ) ) {
      debug_error( "Access denied to script.txt." );
    }

    wxFile f( (wxChar*)"script.txt", wxFile::write );
    f.Write( GetScriptTxt(battle) );
    f.Close();

  } catch (...) {
    debug_error( "Couldn't write script.txt" ); //!@todo Some message to user.
    return false;
  }

  if ( m_process == NULL ) m_process = new SpringProcess( *this );
  if ( wxExecute( WX_STRING(sett().GetSpringUsedLoc()) + _(" script.txt"), wxEXEC_ASYNC, m_process ) == 0 ) {
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
  BattleOptions bo = battle.opts();
s  = _("[GAME]\n");    
  s += _("{\n");
  s +=   _("\tMapname=") + WX_STRING(bo.mapname) + _(";\n");
  s +=   _("\tStartMetal=") + WX_STRING(i2s( bo.startmetal )) + _(";\n");
  s +=   _("\tStartEnergy=") + WX_STRING(i2s( bo.startenergy )) + _(";\n");
  s +=   _("\tMaxUnits=") + WX_STRING(i2s( bo.maxunits )) + _(";\n");
  s +=   _("\tStartPosType=") + WX_STRING(i2s( bo.starttype )) + _(";\n");
  s +=   _("\tGameMode=0;\n");
  s +=   _("\tGameType=") + WX_STRING(i2s( bo.isreplay?1:0 )) + _(";\n");
  s +=   _("\tLimitDGun=") + WX_STRING(i2s( bo.limitdgun?1:0 )) + _(";\n");
  s +=   _("\tDiminishingMMs=") + WX_STRING(i2s( bo.dimmms?1:0 )) + _(";\n");
  s +=   _("\tGhostedBuildings=") + WX_STRING(i2s( bo.ghostedbuildings?1:0 )) + _(";\n\n");

  if ( battle.IsFounderMe() ) s += _("\tHostIP=localhost;\n");
  else s +=   _("\tHostIP=") + WX_STRING( bo.ip ) + _(";\n");
  s +=   _("\tHostPort=") + WX_STRING(i2s( bo.port )) + _(";\n\n");

  s +=   _("\tMyPlayerNum=") + WX_STRING(i2s( battle.GetMyPlayerNum() )) + _(";\n");
  s +=   _("\tNumPlayers=") + WX_STRING(i2s( battle.GetNumUsers() )) + _(";\n");
  s +=   _("\tNumTeams=") + WX_STRING(i2s( battle.GetNumTeams() )) + _(";\n");
  s +=   _("\tNumAllyTeams=") + WX_STRING(i2s( battle.GetNumAllyTeams() )) + _(";\n\n");
  
  for ( int i = 0; i < battle.GetNumUsers(); i++ ) {
    s +=   _("\t[PLAYER") + WX_STRING(i2s( i )) + _("]\n");
    s +=   _("\t{\n");
    s +=     _("\t\tname=") + WX_STRING( battle.GetUser( i ).GetNick() ) + _(";\n");
    s +=     _("\t\tSpectator=") + WX_STRING(i2s( battle.GetUser( i ).GetBattleStatus().spectator?1:0 )) + _(";\n");
    if ( !battle.GetUser( i ).GetBattleStatus().spectator ) {
      s +=     _("\t\tteam=") + WX_STRING(i2s( battle.GetUser( i ).GetBattleStatus().spectator?1:0 )) + _(";\n");
    }
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
