#ifndef SPRINGLOBBY_HEADERGUARD_NETWORKEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_NETWORKEVENTS_H

#include "events.h"

#include <map>
#include <wx/arrstr.h>

typedef wxString NetworkEventsTypes;

typedef wxArrayString NetworkEventData;

EventSender<NetworkEventData> &GetNetworkEventSender( NetworkEventsTypes cmd );

#endif // SPRINGLOBBY_HEADERGUARD_NETWORKEVENTS_H

