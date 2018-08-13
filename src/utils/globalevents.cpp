/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "globalevents.h"

#include <wx/app.h>
#include <vector>

#include "log.h"

const wxEventType GlobalEventManager::OnDownloadStarted = wxNewEventType();
const wxEventType GlobalEventManager::OnDownloadComplete = wxNewEventType();
const wxEventType GlobalEventManager::OnDownloadFailed = wxNewEventType();
const wxEventType GlobalEventManager::OnDownloadProgress = wxNewEventType();

const wxEventType GlobalEventManager::OnUnitsyncFirstTimeLoad =
    wxNewEventType();
const wxEventType GlobalEventManager::OnUnitsyncReloaded = wxNewEventType();
const wxEventType GlobalEventManager::OnLobbyDownloaded = wxNewEventType();
const wxEventType GlobalEventManager::OnSpringTerminated = wxNewEventType();
const wxEventType GlobalEventManager::OnSpringStarted = wxNewEventType();
const wxEventType GlobalEventManager::UpdateFinished = wxNewEventType();
const wxEventType GlobalEventManager::OnQuit = wxNewEventType();
const wxEventType GlobalEventManager::OnLogin = wxNewEventType();
const wxEventType GlobalEventManager::BattleSyncReload = wxNewEventType();
const wxEventType GlobalEventManager::BattleStartedEvent = wxNewEventType();
const wxEventType GlobalEventManager::OnUpdateFinished = wxNewEventType();
const wxEventType GlobalEventManager::ApplicationSettingsChangedEvent =
    wxNewEventType();

GlobalEventManager* GlobalEventManager::m_Instance = nullptr;

GlobalEventManager::GlobalEventManager()
    : m_eventsDisabled(false)
{
	slLogDebugFunc("");
}

GlobalEventManager::~GlobalEventManager()
{
	if (!m_eventsTable.empty()) {
		wxLogWarning("GlobalEventManager::~GlobalEventManager(): not all subscribers had unsubscibed (expect a crash after this!)");
		for (const auto& evts : m_eventsTable) {
			for (const auto& evt : evts.second) {
				wxLogWarning("%s", evt.second.c_str());
			}
		}
		m_eventsTable.clear();
	}
	m_Instance = nullptr;
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
	slLogDebugFunc("");

	if (m_Instance != nullptr) {
		delete m_Instance;
	}
	m_Instance = nullptr;
}

void GlobalEventManager::Send(wxEventType type)
{
	Send(wxCommandEvent(type));
}

void GlobalEventManager::Send(wxCommandEvent event)
{
	slLogDebugFunc("");

	assert(event.GetString().empty()); // using strings here isn't thread safe http://docs.wxwidgets.org/trunk/classwx_evt_handler.html#a0737c6d2cbcd5ded4b1ecdd53ed0def3
	if (m_eventsDisabled)
		return;

	if (event.GetEventType() == GlobalEventManager::OnQuit) {
		m_eventsDisabled = true;
	}

	if (m_eventsTable.find(event.GetEventType()) == m_eventsTable.end())
		return;

	const auto& evtlist = m_eventsTable[event.GetEventType()];
	for (const auto evt : evtlist) {
		evt.first->QueueEvent(event.Clone());
	}
}

void GlobalEventManager::Send(wxEventType type, void* clientData)
{
	wxCommandEvent evt(type);
	evt.SetClientData(clientData);
	Send(evt);
}

void GlobalEventManager::Subscribe(wxEvtHandler* evh, wxEventType id, wxObjectEventFunction func, const std::string& debuginfo)
{
	slLogDebugFunc("");

	GlobalEventManager::_Connect(evh, id, func, debuginfo);
}

void GlobalEventManager::UnSubscribe(wxEvtHandler* evh, wxEventType id)
{
	slLogDebugFunc("");

	GlobalEventManager::_Disconnect(evh, id);
}

void GlobalEventManager::UnSubscribeAll(wxEvtHandler* evh)
{
	slLogDebugFunc("");

	assert(m_eventsTable.size() != 0);

	_Disconnect(evh, ANY_EVENT);
}

void GlobalEventManager::_Connect(wxEvtHandler* evthandler, wxEventType id, wxObjectEventFunction func, const std::string& debuginfo)
{
	assert(evthandler != nullptr);
	assert(func != nullptr);

	std::map<wxEvtHandler*, const std::string>& evtlist = m_eventsTable[id];
	if (evtlist.find(evthandler) != evtlist.end()) {
		assert(false);
		return;
	}
	//	printf("connected event! %lu\n", evthandler);
	evthandler->Connect(id, func);
	evtlist.insert(std::make_pair(evthandler, debuginfo));
	assert(!evtlist.empty());
}

// removes the given eventhandler for the specified event type
void GlobalEventManager::_Disconnect(wxEvtHandler* evthandler, wxEventType id)
{
	//Unlink event handler from all kinds of event types
	if (id == ANY_EVENT) {
		std::vector<wxEventType> removalCollection;

		for (auto& it : m_eventsTable) {
			auto pos = it.second.find(evthandler);
			if (pos == it.second.end())
				continue;
			evthandler->Disconnect(it.first);
			it.second.erase(pos);
			if (it.second.empty()) {
				removalCollection.push_back(it.first);
			}
		}
		//Clear m_eventsTable from empty records
		//those having Id but contain no event handlers
		for (auto it : removalCollection) {
			m_eventsTable.erase(it);
		}
	} else {
		//Unlink event handler from only specified event type

		auto it = m_eventsTable.find(id);
		if (it == m_eventsTable.end()) {
			assert(false);
			return;
		}
		auto pos = it->second.find(evthandler);
		if (pos == it->second.end()) {
			assert(false);
			return;
		}
		evthandler->Disconnect(it->first);
		it->second.erase(pos);
		if (it->second.empty()) {
			m_eventsTable.erase(id);
		}
	}
}
