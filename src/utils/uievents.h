/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_UIEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_UIEVENTS_H

#include <wx/arrstr.h>
#include <wx/string.h>
#include <map>
#include <utility>
#include "events.h"
namespace UiEvents
{
typedef wxString UiEventsTypes;

typedef wxArrayString UiEventData;

struct OnBattleActionData : public wxArrayString
{
	OnBattleActionData(const wxString& nick, const wxString& msg);
};

static const wxString OnBattleActionEvent = _T("OnBattleActionEvent");

EventSender<UiEventData>& GetUiEventSender(UiEventsTypes cmd);

enum EventType {
	PrivateMessage,
	ServerConnection,
	Battleroom,
	UserAction,
	GamePromoted
};

typedef std::pair<EventType, wxString>
    NotficationData;

EventSender<NotficationData>& GetNotificationEventSender();

enum StatusType {
	addStatusMessage = 0,
	removeStatusMessage = 1
};

//! a message - slot number pair
class StatusData : public std::pair<wxString, unsigned int>
{
	typedef std::pair<wxString, unsigned int>
	    BaseType;

public:
	//! this is to be used for adding a message
	StatusData(const wxString& text, unsigned int slot = 0)
	    : BaseType(text, slot)
	{
	}
	//! this is to be used for removing a message (we wont care about the message here)
	explicit StatusData(unsigned int slot = 0)
	    : BaseType(wxEmptyString, slot)
	{
	}
};

//! sends add message in the ctor and remove in the dtor
class ScopedStatusMessage
{
public:
	ScopedStatusMessage(const wxString& msg, unsigned int slot);
	ScopedStatusMessage(const std::string& msg, unsigned int slot);
	~ScopedStatusMessage();

private:
	const unsigned int m_slot;
};

EventSender<StatusData>& GetStatusEventSender(StatusType cmd);
}
#endif // SPRINGLOBBY_HEADERGUARD_UIEVENTS_H
