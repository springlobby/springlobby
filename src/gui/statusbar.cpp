/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "statusbar.h"

#include <wx/event.h>

#include "taskbar.h"
#include "utils/conversion.h"
#include "utils/platform.h"
#include "utils/version.h"

Statusbar::Statusbar(wxWindow* parent)
    : wxStatusBar(parent, wxNewId())
    , m_addMessageSink(this, &GetStatusEventSender(UiEvents::addStatusMessage))
    , m_removeMessageSink(this, &GetStatusEventSender(UiEvents::removeStatusMessage))

{
	int w[3] = {460, -1, 120};
	SetFieldsCount(3, w);
	PushStatusText(TowxString(GetSpringlobbyAgent()), 1);
	taskBar = new TaskBar(this);
}

Statusbar::~Statusbar()
{
	wxDELETE(taskBar);
}

DEFINE_EVENT_TYPE(PUSH_STATUS_MSG)
DEFINE_EVENT_TYPE(POP_STATUS_MSG)

BEGIN_EVENT_TABLE(Statusbar, wxStatusBar)
EVT_COMMAND(wxID_ANY, PUSH_STATUS_MSG, Statusbar::OnUpdateMsg)
EVT_COMMAND(wxID_ANY, POP_STATUS_MSG, Statusbar::OnUpdateMsg)
END_EVENT_TABLE()

void Statusbar::OnUpdateMsg(wxCommandEvent& evt)
{
	assert(wxThread::IsMain());
	const int pos = evt.GetInt();
	SetStatusText(evt.GetString(), pos);
}

void Statusbar::OnAddMessage(UiEvents::StatusData data)
{
	assert(wxThread::IsMain());

	wxCommandEvent evt(PUSH_STATUS_MSG, GetId());
	evt.SetEventObject(this);

	evt.SetString(data.first);
	evt.SetInt(data.second);
	wxPostEvent(this, evt);
}

void Statusbar::OnRemoveMessage(UiEvents::StatusData data)
{
	assert(wxThread::IsMain());

	wxCommandEvent evt(POP_STATUS_MSG, GetId());
	evt.SetEventObject(this);

	evt.SetString(data.first);
	evt.SetInt(data.second);
	wxPostEvent(this, evt);
}
