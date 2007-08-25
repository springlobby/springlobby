/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "singleplayerbattle.h"
#include "mainsingleplayertab.h"
#include "server.h"


SinglePlayerBattle::SinglePlayerBattle(Ui& ui, MainSinglePlayerTab& msptab):
  m_ui(ui),
  m_sptab(msptab)
{

}


SinglePlayerBattle::~SinglePlayerBattle()
{

}


wxPoint SinglePlayerBattle::GetStartPos(int index)
{
  return wxPoint( -1, -1 );
}


void SinglePlayerBattle::SetStartPos(int index, wxPoint pos)
{

}


unsigned int SinglePlayerBattle::GetNumBots()
{
  return 0;
}


BattleBot* SinglePlayerBattle::GetBot(unsigned int index)
{
  return 0;
}


void SinglePlayerBattle::UpdateBot(unsigned int index, int ally, int position, int side)
{

}


void SinglePlayerBattle::RemoveBot(unsigned int index)
{

}


unsigned int SinglePlayerBattle::AddBot(int ally, int position, const wxString& aidll)
{
  return -1;
}


void SinglePlayerBattle::SendHostInfo( HostInfo update )
{
  if ( (update && HI_StartType) != 0 ) m_sptab.UpdateMinimap();
}

