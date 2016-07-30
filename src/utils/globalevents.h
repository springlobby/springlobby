/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H

#include <map>
#include <set>

#include <wx/event.h>
#include "utils/conversion.h"

class GlobalEventManager
{
private:
	GlobalEventManager();
	~GlobalEventManager();

public:
	static GlobalEventManager* Instance();
	static void Release();

public:
	void Subscribe(wxEvtHandler* evh, wxEventType id, wxObjectEventFunction func, const std::string& debuginfo);
	void UnSubscribe(wxEvtHandler* evh, wxEventType id = 0);
	void UnSubscribeAll(wxEvtHandler* evh);

	void Send(wxEventType type);
	void Send(wxCommandEvent event);
	void Send(wxEventType type, void* clientData);

private:
	void _Connect(wxEvtHandler* evthandler, wxEventType id, wxObjectEventFunction func, const std::string& debuginfo);
	void _Disconnect(wxEvtHandler* evthandler, wxEventType id = 0);

public:
	static const wxEventType OnDownloadStarted;
	static const wxEventType OnDownloadComplete;
	static const wxEventType OnDownloadFailed;
	static const wxEventType OnDownloadProgress;
	static const wxEventType OnUnitsyncFirstTimeLoad;
	static const wxEventType OnUnitsyncReloaded;
	static const wxEventType OnSpringTerminated;
	static const wxEventType OnSpringStarted;
	static const wxEventType UpdateFinished;
	static const wxEventType OnQuit;
	static const wxEventType OnLogin;
	static const wxEventType OnUpdateFinished;
	static const wxEventType OnLobbyDownloaded;

	static const wxEventType BattleSyncReload;
	static const wxEventType BattleStartedEvent;
	static const wxEventType ApplicationSettingsChangedEvent;

private:
	static GlobalEventManager* m_Instance;

private:
	bool m_eventsDisabled;
	std::map<wxEventType, std::map<wxEvtHandler*, const std::string> > m_eventsTable;
	const int ANY_EVENT = 0;
};

#define SUBSCRIBE_GLOBAL_EVENT(event, callbackfunc) \
	GlobalEventManager::Instance()->Subscribe(this, event, wxObjectEventFunction(&callbackfunc), stdprintf("%s:%d %s()", __FILE__, __LINE__, __func__))


#endif // SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
