#include "battleevents.h"
#include <map>

namespace BattleEvents {
	static std::map< BattleEventsTypes, EventSender<BattleEventData> > BattleEvents;

	EventSender<BattleEventData> &GetBattleEventSender( BattleEventsTypes cmd )
    {
	   return BattleEvents[cmd];
    }
}
