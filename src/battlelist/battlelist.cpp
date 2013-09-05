/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/container/battlelist.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/



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


/*
Battle& BattleList::GetFirstBattle()
{
  return *m_battles.begin()->second;
}
*/


BattleList::battle_map_t::size_type BattleList_Iter::GetNumBattles() const
{
  return (m_battlelist)?(m_battlelist->m_battles.size()):0;
}


void BattleList_Iter::IteratorBegin()
{
  if (m_battlelist) m_iterator = m_battlelist->m_battles.begin();
}

Battle* BattleList_Iter::GetBattle()
{
  Battle* battle = m_iterator->second;
  if ( m_battlelist && m_iterator != m_battlelist->m_battles.end() ) ++m_iterator;
  return battle;
}

bool BattleList_Iter::EOL() const
{
  return ( m_battlelist && m_iterator == m_battlelist->m_battles.end() )?true:false;
}


Battle& BattleList_Iter::GetBattle( BattleList::battle_id_t const& id ) {
  if (!m_battlelist) throw std::logic_error("BattleList_Iter::GetBattle(): no battlelist");
  BattleList::battle_iter_t b = m_battlelist->m_battles.find(id);
  if (b == m_battlelist->m_battles.end()) throw std::runtime_error("BattleList_Iter::GetBattle(): no such battle");
  return *b->second;
}


bool BattleList_Iter::BattleExists( BattleList::battle_id_t const& id ) {
  if (!m_battlelist) throw std::logic_error("BattleList_Iter::BattleExists(): no battlelist");
  return m_battlelist->m_battles.find(id) != m_battlelist->m_battles.end();
}

