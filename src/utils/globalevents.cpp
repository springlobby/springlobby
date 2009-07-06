#include "globalevents.h"

namespace GlobalEvents {

    static std::map< GlobalEventsTypes, EventSender<GlobalEventData> > GlobalEvents;

    EventSender<GlobalEventData> &GetGlobalEventSender( GlobalEventsTypes cmd )
    {
       return GlobalEvents[cmd];
    }
}
