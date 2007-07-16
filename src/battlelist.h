//
// Class: BattleList
//

#ifndef _BATTLELIST_H_
#define _BATTLELIST_H_

#include <map>

class Battle;

typedef unsigned int battle_id_t;

//! @brief mapping from battle id number to battle object
typedef std::map<battle_id_t, Battle*> battle_map_t;
//! @brief iterator for battle map
typedef battle_map_t::iterator battle_iter_t;

class BattleList
{
  public:
    BattleList();
    void AddBattle( Battle& battle );
    void RemoveBattle( battle_id_t const& id );
    Battle& GetBattle( battle_id_t const& id );
    bool BattleExists( battle_id_t const& id );
    battle_map_t::size_type GetNumBattles();

  private:
    battle_map_t m_battles;
};

#endif  //_BATTLELIST_H_

