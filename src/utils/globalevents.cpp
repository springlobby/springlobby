#include "globalevents.h"

static std::map< GlobalEventsTypes, EventSender<GlobalEventData> > GlobalEvents;

EventSender<GlobalEventData> &GetGlobalEventSender( GlobalEventsTypes cmd )
{
   return GlobalEvents[cmd];
}
