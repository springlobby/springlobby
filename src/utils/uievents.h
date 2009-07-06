#ifndef SPRINGLOBBY_HEADERGUARD_UIEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_UIEVENTS_H

#include "events.h"
#include <wx/arrstr.h>
#include <map>

namespace UiEvents {
    typedef wxString UiEventsTypes;

    typedef wxArrayString UiEventData;

    EventSender<UiEventData> &GetUiEventSender( UiEventsTypes cmd );
}
#endif // SPRINGLOBBY_HEADERGUARD_UIEVENTS_H
