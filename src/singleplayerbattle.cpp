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


int SinglePlayerBattle::GetNumBots()
{
  return 0;
}


SinglePlayerBot& SinglePlayerBattle::GetBot(int index)
{
  static SinglePlayerBot bot;
  return bot;
}


void SinglePlayerBattle::UpdateBot(int index, int ally, int position, int side)
{

}


void SinglePlayerBattle::RemoveBot(int index)
{

}


int SinglePlayerBattle::AddBot(int ally, int position, const wxString& aidll)
{
  return -1;
}


void SinglePlayerBattle::SendHostInfo( HostInfo update )
{
  if ( (update && HI_StartType) != 0 ) m_sptab.UpdateMinimap();
}

