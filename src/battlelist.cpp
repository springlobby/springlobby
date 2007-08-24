/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <stdexcept>

#include "battlelist.h"
#include "battle.h"


BattleList::BattleList()
{
}


void BattleList::AddBattle( Battle& battle )
{
  m_battles[battle.GetBattleId()] = &battle;
}


void BattleList::RemoveBattle( battle_id_t const& id ) {
  m_battles.erase(id);
}


Battle& BattleList::GetBattle( battle_id_t const& id ) {
  battle_iter_t b = m_battles.find(id);
  if (b == m_battles.end()) throw std::logic_error("BattleList::GetBattle(): no such battle");
  return *b->second;
}


Battle& BattleList::GetFirstBattle()
{
  return *m_battles.begin()->second;
}


bool BattleList::BattleExists( battle_id_t const& id ) {
  return m_battles.find(id) != m_battles.end();
}


battle_map_t::size_type BattleList::GetNumBattles()
{
  return m_battles.size();
}

