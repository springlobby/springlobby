/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "singleplayerbattle.h"
#include "mainsingleplayertab.h"
#include "server.h"
#include "utils.h"


SinglePlayerBattle::SinglePlayerBattle(Ui& ui, MainSinglePlayerTab& msptab):
  m_ui(ui),
  m_sptab(msptab)
{
  AddBot( 0, 2000, 3000, _T("") );
  AddBot( 0, 3000, 2000, _T("AAI") );
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
  delete m_bots[index];
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

