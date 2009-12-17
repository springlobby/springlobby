#ifndef SPRINGLOBBY_HEADERGUARD_UIEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_UIEVENTS_H

#include "events.h"
#include <wx/arrstr.h>
#include <wx/bitmap.h>
#include <map>
#include <utility>

namespace UiEvents {
    typedef wxString UiEventsTypes;

    typedef wxArrayString UiEventData;

    EventSender<UiEventData> &GetUiEventSender( UiEventsTypes cmd );

    typedef std::pair< wxBitmap, wxString >
        NotficationData;

    EventSender<NotficationData> &GetNotificationEventSender();
}
#endif // SPRINGLOBBY_HEADERGUARD_UIEVENTS_H
