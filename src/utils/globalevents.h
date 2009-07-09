#ifndef SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H

#include "events.h"

#include <map>
namespace GlobalEvents {
    enum GlobalEventsTypes
    {
      OnUnitsyncFirstTimeLoad,
      OnUnitsyncReloaded,
      UnitSyncReloadRequest,
      UpdateFinished,

      OnTimerUpdates
    };

    typedef int GlobalEventData;

    EventSender<GlobalEventData> &GetGlobalEventSender( GlobalEventsTypes cmd );
}
#endif // SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
