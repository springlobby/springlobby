//
// Class: BattleList
//

#ifndef _BATTLELIST_H_
#define _BATTLELIST_H_

#include <algorithm>
#include <iterator>
#include <map>
#include <stdexcept>
#include "battle.h"


//! @brief std::map<> list that stores Battle pointers.
typedef std::map<int, Battle*> battle_map_t;
//! @brief battle_map_t iterator.
typedef battle_map_t::iterator battle_iter_t;


class BattleList
{
  public:
    
    BattleList() {}
    
    void AddBattle( Battle& battle ) {
      m_battles[battle.opts().battleid] = &battle;
    }
    
    void RemoveBattle( int const& id ) {
      m_battles.erase(id);
    }
 
    Battle& GetBattle( int const& id ) {
      battle_iter_t b = m_battles.find(id);
      if (b == m_battles.end()) throw std::logic_error("BattleList::GetBattle(): no such battle");
      return *b->second;
    }

    bool BattleExists( int const& id ) {
      return m_battles.find(id) != m_battles.end();
    }

    int GetNumBattles() { return (int)m_battles.size(); }
 
  private:
    battle_map_t m_battles;

};


#endif  //_BATTLELIST_H_

