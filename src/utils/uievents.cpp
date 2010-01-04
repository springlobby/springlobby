#include "uievents.h"

namespace UiEvents {
    static std::map< UiEventsTypes, EventSender<UiEventData> > UiEvents;

    EventSender<UiEventData> &GetUiEventSender( UiEventsTypes cmd )
    {
       return UiEvents[cmd];
    }

	static std::map< StatusType, EventSender<StatusData> > StatusEvents;
	EventSender<StatusData> &GetStatusEventSender( StatusType cmd )
	{
		return StatusEvents[cmd];
	}

	ScopedStatusMessage::ScopedStatusMessage( const wxString& msg, unsigned int slot )
		: m_slot( slot )
	{
		StatusData data( msg, m_slot );
		GetStatusEventSender( UiEvents::addStatusMessage ).SendEvent( data );
	}

	ScopedStatusMessage::~ScopedStatusMessage()
	{
		StatusData data( m_slot );
		GetStatusEventSender( UiEvents::removeStatusMessage ).SendEvent( data );
	}
}
