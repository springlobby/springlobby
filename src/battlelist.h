#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELIST_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELIST_H

#include <map>
#include "utils/mixins.hh"

class Battle;

/** \brief encapsulates a <battle_id_t, Battle*> map
 */
class BattleList
{
  friend class BattleList_Iter;
  public:
    BattleList();

    typedef unsigned int battle_id_t;

    void AddBattle( Battle& battle );
    void RemoveBattle( battle_id_t const& id );

    //! @brief mapping from battle id number to battle object
    typedef std::map<battle_id_t, Battle*> battle_map_t;
    //! @brief iterator for battle map
    typedef battle_map_t::iterator battle_iter_t;

  private:
    battle_map_t m_battles;
};

/** BattleList_Iter gives us the posibility to get Battles out of the list without the rights to change the list */
class BattleList_Iter : public SL::NonCopyable
{
  public:
	BattleList_Iter(BattleList* battlelist) : m_battlelist( battlelist ) {}
	~BattleList_Iter() {}
    void IteratorBegin();
    Battle* GetBattle();
    bool EOL();
    Battle& GetBattle( BattleList::battle_id_t const& id );
    //Battle& GetFirstBattle();
    bool BattleExists( BattleList::battle_id_t const& id );
    BattleList::battle_map_t::size_type GetNumBattles();
  private:
    BattleList::battle_iter_t m_iterator;
    BattleList* m_battlelist;
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLELIST_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

