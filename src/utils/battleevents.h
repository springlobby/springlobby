#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEEVENTS_H

#include "events.h"
#include "../ibattle.h"
#include <utility>

namespace BattleEvents {
	enum BattleEventsTypes {
		AddBattle = 0,
		RemoveBattle = 1,
		UpdateBattle = 2,
		BattleInfoUpdate = 3
	};

	typedef std::pair<IBattle*,wxString> BattleEventData;

	EventSender<BattleEventData> &GetBattleEventSender( BattleEventsTypes cmd );

}
#endif // SPRINGLOBBY_HEADERGUARD_BATTLEEVENTS_H
