/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/app.h>

#include "globalevents.h"

const wxEventType GlobalEventManager::OnDownloadStarted = wxNewEventType();
const wxEventType GlobalEventManager::OnDownloadComplete = wxNewEventType();
const wxEventType GlobalEventManager::OnDownloadFailed = wxNewEventType();
const wxEventType GlobalEventManager::OnDownloadProgress = wxNewEventType();

const wxEventType GlobalEventManager::OnUnitsyncFirstTimeLoad = wxNewEventType();
const wxEventType GlobalEventManager::OnUnitsyncReloaded = wxNewEventType();
const wxEventType GlobalEventManager::OnLobbyDownloaded = wxNewEventType();
const wxEventType GlobalEventManager::OnSpringTerminated = wxNewEventType();
const wxEventType GlobalEventManager::OnSpringStarted = wxNewEventType();
const wxEventType GlobalEventManager::UpdateFinished = wxNewEventType();
const wxEventType GlobalEventManager::OnQuit = wxNewEventType();
const wxEventType GlobalEventManager::OnLogin = wxNewEventType();
const wxEventType GlobalEventManager::BattleSyncReload = wxNewEventType();
const wxEventType GlobalEventManager::BattleStartedEvent = wxNewEventType();
const wxEventType GlobalEventManager::UserBattleStatusChangedEvent = wxNewEventType();
const wxEventType GlobalEventManager::OnUpdateFinished = wxNewEventType();
const wxEventType GlobalEventManager::GamePromotedEvent = wxNewEventType();
const wxEventType GlobalEventManager::ApplicationSettingsChangedEvent = wxNewEventType();

bool GlobalEventManager::m_eventsDisabled = false;

GlobalEventManager* GlobalEventManager::m_Instance = nullptr;

GlobalEventManager::GlobalEventManager()
{
}

GlobalEventManager::~GlobalEventManager()
{
	assert(m_eventsTable.empty()); // all subcribers must have unsubscribed before exit (else it possible crashes)
}

GlobalEventManager* GlobalEventManager::Instance()
{
	if (m_Instance == nullptr) {
		m_Instance = new GlobalEventManager();
	}
	return m_Instance;
}

void GlobalEventManager::Release()
{
	if (m_Instance != nullptr) {
		delete m_Instance;
	}
	m_Instance = nullptr;
}

void GlobalEventManager::Send(wxEventType type)
{
	wxCommandEvent evt = wxCommandEvent(type);
	Send(evt);
}

void GlobalEventManager::Send(wxEventType type, void* clientData)
{
	wxCommandEvent evt = wxCommandEvent(type);
	evt.SetClientData(clientData);
	Send(evt);
}

void GlobalEventManager::Send(wxCommandEvent event)
{
	if (m_eventsDisabled)
		return;

	if (m_eventsTable.find(event.GetEventType()) == m_eventsTable.end())
		return;

	std::set<wxEvtHandler*>& evtlist = m_eventsTable[event.GetEventType()];
//	assert(event.GetString() == wxEmptyString); // using strings here isn't thread safe http://docs.wxwidgets.org/trunk/classwx_evt_handler.html#a0737c6d2cbcd5ded4b1ecdd53ed0def3

	for (auto evt : evtlist) {
		evt->AddPendingEvent(event);
	}

	if (event.GetEventType() == GlobalEventManager::OnQuit) {
		m_eventsDisabled = true;
	}
}

void GlobalEventManager::Subscribe(wxEvtHandler* evh, wxEventType id, wxObjectEventFunction func)
{
	GlobalEventManager::_Connect(evh, id, func);
}

void GlobalEventManager::UnSubscribe(wxEvtHandler* evh, wxEventType id)
{
	GlobalEventManager::_Disconnect(evh, id);
}

void GlobalEventManager::UnSubscribeAll(wxEvtHandler* evh)
{
	_Disconnect(evh, ANY_EVENT);
}

void GlobalEventManager::_Connect(wxEvtHandler* evthandler, wxEventType id, wxObjectEventFunction func)
{
	assert(evthandler != NULL);
	std::set<wxEvtHandler*>& evtlist = m_eventsTable[id];
	if (evtlist.find(evthandler) != evtlist.end())
		return;
	//	printf("connected event! %lu\n", evthandler);
	evthandler->Connect(id, func);
	evtlist.insert(evthandler);
	assert(!evtlist.empty());
}


// removes the given eventhandler for the specified event type
void GlobalEventManager::_Disconnect(wxEvtHandler* evthandler, wxEventType id)
{

	for (auto it : m_eventsTable) {
		if ((id != it.first) && (id != ANY_EVENT))
			continue;
		auto pos = it.second.find(evthandler);
		if (pos == it.second.end())
			continue;
		evthandler->Disconnect(it.first);
		it.second.erase(pos);
		if (it.second.empty()) {
			m_eventsTable.erase(it.first);
		}
	}
}
