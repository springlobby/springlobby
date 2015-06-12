/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/container/battlelist.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include <stdexcept>

#include "battlelist.h"
#include "ibattle.h"


BattleList::BattleList()
{
}


void BattleList::AddBattle(IBattle& battle)
{
	m_battles[battle.GetBattleId()] = &battle;
}


void BattleList::RemoveBattle(battle_id_t const& id)
{
	m_battles.erase(id);
}


BattleList::battle_map_t::size_type BattleList_Iter::GetNumBattles() const
{
	return (m_battlelist) ? (m_battlelist->m_battles.size()) : 0;
}

BattleList_Iter::~BattleList_Iter()
{
	m_battlelist = NULL;
}


void BattleList_Iter::IteratorBegin()
{
	if (m_battlelist)
		m_iterator = m_battlelist->m_battles.begin();
}

IBattle* BattleList_Iter::GetBattle()
{
	IBattle* battle = m_iterator->second;
	if (m_battlelist && m_iterator != m_battlelist->m_battles.end())
		++m_iterator;
	return battle;
}

bool BattleList_Iter::EOL() const
{
	return (m_battlelist && m_iterator == m_battlelist->m_battles.end()) ? true : false;
}


IBattle& BattleList_Iter::GetBattle(BattleList::battle_id_t const& id)
{
	if (!m_battlelist)
		throw std::logic_error("BattleList_Iter::GetBattle(): no battlelist");
	BattleList::battle_iter_t b = m_battlelist->m_battles.find(id);
	if (b == m_battlelist->m_battles.end())
		throw std::runtime_error("BattleList_Iter::GetBattle(): no such battle");
	return *b->second;
}


bool BattleList_Iter::BattleExists(BattleList::battle_id_t const& id)
{
	if (!m_battlelist)
		throw std::logic_error("BattleList_Iter::BattleExists(): no battlelist");
	return m_battlelist->m_battles.find(id) != m_battlelist->m_battles.end();
}
