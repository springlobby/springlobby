/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEEVENTS_H

#include "events.h"
#include "ibattle.h"
#include <utility>

namespace BattleEvents {
	enum BattleEventsTypes {
		AddBattle = 0,
		RemoveBattle = 1,
		UpdateBattle = 2,
		BattleInfoUpdate = 3
	};

	typedef std::pair<IBattle*,std::string> BattleEventData;

	EventSender<BattleEventData> &GetBattleEventSender( BattleEventsTypes cmd );

}
#endif // SPRINGLOBBY_HEADERGUARD_BATTLEEVENTS_H
