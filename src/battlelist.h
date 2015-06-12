/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELIST_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELIST_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/container/battlelist.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include <map>
#include "utils/mixins.h"

class IBattle;

/** \brief encapsulates a <battle_id_t, Battle*> map
 */
class BattleList
{
	friend class BattleList_Iter;

public:
	BattleList();

	typedef unsigned int battle_id_t;

	void AddBattle(IBattle& battle);
	void RemoveBattle(battle_id_t const& id);

	//! @brief mapping from battle id number to battle object
	typedef std::map<battle_id_t, IBattle*> battle_map_t;
	//! @brief iterator for battle map
	typedef battle_map_t::iterator battle_iter_t;

private:
	battle_map_t m_battles;
};

/** BattleList_Iter gives us the posibility to get Battles out of the list without the rights to change the list */
class BattleList_Iter : public SL::NonCopyable
{
public:
	BattleList_Iter(BattleList* battlelist)
	    : m_battlelist(battlelist)
	{
	}
	~BattleList_Iter();
	void IteratorBegin();
	IBattle* GetBattle();
	bool EOL() const;
	IBattle& GetBattle(BattleList::battle_id_t const& id);
	bool BattleExists(BattleList::battle_id_t const& id);
	BattleList::battle_map_t::size_type GetNumBattles() const;

private:
	BattleList::battle_iter_t m_iterator;
	BattleList* m_battlelist;
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLELIST_H
