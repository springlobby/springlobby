/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "uievents.h"

#include "utils/conversion.h"

namespace UiEvents
{
static std::map<UiEventsTypes, EventSender<UiEventData> > UiEvents;

EventSender<UiEventData>& GetUiEventSender(UiEventsTypes cmd)
{
	return UiEvents[cmd];
}

static EventSender<NotficationData> NotificationEventSender;
EventSender<NotficationData>& GetNotificationEventSender()
{
	return NotificationEventSender;
}

static std::map<StatusType, EventSender<StatusData> > StatusEvents;
EventSender<StatusData>& GetStatusEventSender(StatusType cmd)
{
	return StatusEvents[cmd];
}

inline void ScopedSend(const wxString& msg, unsigned int slot)
{
	StatusData data(msg, slot);
	GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(data);
}

ScopedStatusMessage::ScopedStatusMessage(const wxString& msg, unsigned int slot)
    : m_slot(slot)
{
	ScopedSend(msg, slot);
}

ScopedStatusMessage::ScopedStatusMessage(const std::string& msg, unsigned int slot)
    : m_slot(slot)
{
	ScopedSend(TowxString(msg), slot);
}

ScopedStatusMessage::~ScopedStatusMessage()
{
	StatusData data(m_slot);
	GetStatusEventSender(UiEvents::removeStatusMessage).SendEvent(data);
}

OnBattleActionData::OnBattleActionData(const wxString& nick, const wxString& msg)
{
	Add(nick);
	Add(msg);
}
}
