/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/app.h>
#include <list>
#include <map>

#include "globalevents.h"

const wxEventType GlobalEventManager::OnDownloadComplete = wxNewEventType();
const wxEventType GlobalEventManager::OnUnitsyncFirstTimeLoad = wxNewEventType();
const wxEventType GlobalEventManager::OnUnitsyncReloaded = wxNewEventType();
const wxEventType GlobalEventManager::OnLobbyDownloaded = wxNewEventType();
const wxEventType GlobalEventManager::OnSpringTerminated = wxNewEventType();
const wxEventType GlobalEventManager::OnSpringStarted = wxNewEventType();
const wxEventType GlobalEventManager::UpdateFinished = wxNewEventType();
const wxEventType GlobalEventManager::OnQuit = wxNewEventType();
const wxEventType GlobalEventManager::OnLogin = wxNewEventType();
const wxEventType GlobalEventManager::PlasmaResourceListParsed = wxNewEventType();
const wxEventType GlobalEventManager::PlasmaResourceListFailedDownload = wxNewEventType();
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
	if (m_eventsDisabled) {
		return;
	}

	std::list<wxEvtHandler*>& evtlist = m_eventsTable[event.GetEventType()];
	assert(event.GetString() == wxEmptyString); // using strings here isn't thread safe http://docs.wxwidgets.org/trunk/classwx_evt_handler.html#a0737c6d2cbcd5ded4b1ecdd53ed0def3

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
	std::list<wxEvtHandler*>& evtlist = m_eventsTable[id];
	for (auto evt : evtlist) {
		if (evt == evthandler) {
			//			printf("Double Evthandler\n");
			return;
		}
	}
	//	printf("connected event! %lu\n", evthandler);
	evthandler->Connect(id, func);
	evtlist.push_back(evthandler);
	assert(!evtlist.empty());
}

void GlobalEventManager::_Disconnect(wxEvtHandler* evthandler, wxEventType id)
{
	std::map<wxEventType, std::list<wxEvtHandler*> >::iterator it;

	for (it = m_eventsTable.begin(); it != m_eventsTable.end(); ++it) {
		if ((id == ANY_EVENT) || (id == it->first)) {
			for (std::list<wxEvtHandler*>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
				if (*it2 == evthandler) {
					evthandler->Disconnect(it->first);
					it->second.erase(it2);
					break;
				}
			}
		}
	}
}
