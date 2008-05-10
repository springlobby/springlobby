/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>

#include "singleplayerbattle.h"
#include "mainsingleplayertab.h"
#include "server.h"
#include "utils.h"
#include "uiutils.h"
#include "ui.h"


SinglePlayerBattle::SinglePlayerBattle(Ui& ui, MainSinglePlayerTab& msptab):
  m_ui(ui),
  m_sptab(msptab)
{
  CustomBattleOptions()->setSingleOption( _T("startpostype"), wxString::Format(_T("%d"), 3), EngineOption );
  wxColour col = GetFreeColour( NULL );
  int i = AddBot( 0, 0, 0, 0, _T("") );
  BattleBot* bot = GetBot( i );
  ASSERT_LOGIC( bot != 0, _T("bot == 0") );
  bot->bs.colour = col;
}


SinglePlayerBattle::~SinglePlayerBattle()
{

}


unsigned int SinglePlayerBattle::GetNumBots()
{
  return m_bots.size();
}


BattleBot* SinglePlayerBattle::GetBotByStartPosition( unsigned int startpos )
{
  const UnitSyncMap& map = LocalMap();
  ASSERT_LOGIC( ((int)startpos < map.info.posCount) && (startpos >= 0), _T("Invalid startpos") );
  for ( unsigned int bi = 0; bi < GetNumBots(); bi++ ) {
    BattleBot* bot = GetBot( bi );
    ASSERT_LOGIC( bot != 0, _T("Bot == 0") );
    if ( ( map.info.positions[startpos].x == bot->posx ) && ( map.info.positions[startpos].y == bot->posy ) ) {
      return bot;
    }
  }
  return 0;
}


BattleBot* SinglePlayerBattle::GetBot(unsigned int index)
{
  return m_bots[index];
}


void SinglePlayerBattle::UpdateBot(unsigned int index, int ally, int posx, int posy, int side)
{

}


void SinglePlayerBattle::RemoveBot(unsigned int index)
{
  if ( m_bots[index] != 0 ) {
    if ( m_bots[index]->aidll == _T("") ) return;
  }
  delete m_bots[index];
  m_bots[index] = 0;
  std::vector<BattleBot*>::iterator i = m_bots.begin();
  i += index;
  m_bots.erase( i );
}


unsigned int SinglePlayerBattle::AddBot(int ally, int posx, int posy, int handicap, const wxString& aidll)
{
  BattleBot* bot = new BattleBot;
  bot->bs.ally = ally;
  bot->bs.side = 0;
  bot->posx = posx;
  bot->posy = posy;
  bot->handicap = handicap;
  bot->aidll = aidll;

  m_bots.push_back( bot );
  return m_bots.size() - 1;
}


void SinglePlayerBattle::SendHostInfo( HostInfo update )
{
  if ( (update & HI_StartType) != 0 ) m_sptab.UpdateMinimap();
  if ( (update & HI_Restrictions) != 0 ) m_sptab.ReloadRestrictions();
  if ( (update & HI_Map_Changed) != 0 )
  {
    CustomBattleOptions()->loadOptions( MapOption, GetHostMapName() );
    m_sptab.ReloadMapOptContrls();
  }
  if ( (update & HI_Mod_Changed) != 0 )
  {
    CustomBattleOptions()->loadOptions( ModOption, GetHostModName() );
    m_sptab.ReloadModOptContrls();
  }
}


int SinglePlayerBattle::GetFreeAlly()
{
  int lowest = 0;
  bool changed = true;
  while ( changed ) {
    changed = false;
    for ( unsigned int i = 0; i < GetNumBots(); i++ ) {
      BattleBot* bot = GetBot( i );
      ASSERT_LOGIC( bot != 0, _T("Bot == 0") );
      if ( bot->bs.ally == lowest ) {
        lowest++;
        changed = true;
      }
    }
  }
  return lowest;
}


void SinglePlayerBattle::GetFreePosition( int& x, int& y )
{
  UnitSyncMap map = LocalMap();
  for ( int i = 0; i < map.info.posCount; i++ ) {
    bool taken = false;
    for ( unsigned int bi = 0; bi < GetNumBots(); bi++ ) {
      BattleBot* bot = GetBot( bi );
      ASSERT_LOGIC( bot != 0, _T("Bot == 0") );
      if ( ( map.info.positions[i].x == bot->posx ) && ( map.info.positions[i].y == bot->posy ) ) {
        taken = true;
        break;
      }
    }
    if ( !taken ) {
      x = CLAMP(map.info.positions[i].x, 0, map.info.width);
      y = CLAMP(map.info.positions[i].y, 0, map.info.height);
      return;
    }
  }
  x = map.info.width / 2;
  y = map.info.height / 2;
}


wxColour SinglePlayerBattle::GetFreeColour( User *for_whom )
{
  unsigned int lowest = 0;
  bool changed = true;
  while (changed) {
    changed = false;
    if ( lowest >= m_bots.size() ) break;
    for( unsigned int i = 0; i < m_bots.size(); i++ ) {
      BattleBot* bot = GetBot( i );
      ASSERT_LOGIC( bot != 0, _T("bot == 0"));
      if ( AreColoursSimilar( bot->bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) ) {
        lowest++;
        changed = true;
        if ( lowest >= m_bots.size() ) break;
      }
    }
  }

  return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );
}

