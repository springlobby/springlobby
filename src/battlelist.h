#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELIST_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELIST_H

#include <map>

class Battle;

typedef unsigned int battle_id_t;

//! @brief mapping from battle id number to battle object
typedef std::map<battle_id_t, Battle*> battle_map_t;
//! @brief iterator for battle map
typedef battle_map_t::iterator battle_iter_t;


class BattleList
{
  friend class BattleList_Iter;
  public:
    BattleList();
    void AddBattle( Battle& battle );
    void RemoveBattle( battle_id_t const& id );
  private:
    battle_map_t m_battles;
};


//BattleList Iter gives us the posibility to get Battles out of the list without
//the rights to change the list

class BattleList_Iter
{
  public:
    BattleList_Iter(BattleList* battlelist) : m_battlelist( battlelist ) {};
    ~BattleList_Iter() {};
    void IteratorBegin();
    Battle& GetBattle();
    bool EOL();
    Battle& GetBattle( battle_id_t const& id );
    //Battle& GetFirstBattle();
    bool BattleExists( battle_id_t const& id );
    battle_map_t::size_type GetNumBattles();
  private:
    battle_iter_t m_iterator;
    BattleList* m_battlelist;
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLELIST_H
