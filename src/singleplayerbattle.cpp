/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>

#include "singleplayerbattle.h"
#include "mainsingleplayertab.h"
#include "server.h"
#include "utils.h"


SinglePlayerBattle::SinglePlayerBattle(Ui& ui, MainSinglePlayerTab& msptab):
  m_ui(ui),
  m_sptab(msptab)
{
  AddBot( 0, 0, 0, _T("") );
}


SinglePlayerBattle::~SinglePlayerBattle()
{

}


unsigned int SinglePlayerBattle::GetNumBots()
{
  return m_bots.size();
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
    if ( m_bots[index]->aidll == "" ) return;
  }
  delete m_bots[index];
  m_bots[index] = 0;
  std::vector<BattleBot*>::iterator i = m_bots.begin();
  i += index;
  m_bots.erase( i );
}


unsigned int SinglePlayerBattle::AddBot(int ally, int posx, int posy, const wxString& aidll)
{
  BattleBot* bot = new BattleBot;
  bot->bs.ally = ally;
  bot->bs.side = 0;
  bot->posx = posx;
  bot->posy = posy;
  bot->aidll = STD_STRING(aidll);

  m_bots.push_back( bot );
  return m_bots.size() - 1;
}


void SinglePlayerBattle::SendHostInfo( HostInfo update )
{
  if ( (update && HI_StartType) != 0 ) m_sptab.UpdateMinimap();
}


int SinglePlayerBattle::GetFreeAlly()
{
  int lowest = 0;
  bool changed = true;
  while ( changed ) {
    changed = false;
    for ( int i = 0; i < GetNumBots(); i++ ) {
      BattleBot* bot = GetBot( i );
      ASSERT_LOGIC( bot != 0, "Bot == 0" );
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
  UnitSyncMap map = Map();
  for ( int i = 0; i < map.info.posCount; i++ ) {
    bool taken = false;
    for ( int bi = 0; bi < GetNumBots(); bi++ ) {
      BattleBot* bot = GetBot( bi );
      ASSERT_LOGIC( bot != 0, "Bot == 0" );
      if ( ( map.info.positions[i].x == bot->posx ) && ( map.info.positions[i].y == bot->posy ) ) {
        taken = true;
        break;
      }
    }
    if ( !taken ) {
      x = map.info.positions[i].x;
      y = map.info.positions[i].y;
      return;
    }
  }
  x = -1;
  y = -1;
}

