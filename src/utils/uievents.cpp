#include "uievents.h"

namespace UiEvents {
    static std::map< UiEventsTypes, EventSender<UiEventData> > UiEvents;

    EventSender<UiEventData> &GetUiEventSender( UiEventsTypes cmd )
    {
       return UiEvents[cmd];
    }
}
