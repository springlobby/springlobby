/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include "singleplayerbattle.h"


SinglePlayerBattle::SinglePlayerBattle(Ui& ui):
  m_ui(ui)
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

