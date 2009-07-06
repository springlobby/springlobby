#include "networkevents.h"

static std::map< NetworkEventsTypes, EventSender<NetworkEventData> > NetworkEvents;

EventSender<NetworkEventData> &GetNetworkEventSender( NetworkEventsTypes cmd )
{
   return NetworkEvents[cmd];
}
