/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H

#include <map>
#include <set>

#include <wx/event.h>


class GlobalEventManager
{
private:
	GlobalEventManager();
	~GlobalEventManager();

public:
	static GlobalEventManager* Instance();
	static void Release();

public:
	void Subscribe(wxEvtHandler* evh, wxEventType id, wxObjectEventFunction func);
	void UnSubscribe(wxEvtHandler* evh, wxEventType id = 0);
	void UnSubscribeAll(wxEvtHandler* evh);

	void Send(wxEventType type);
	void Send(wxCommandEvent event);

private:
	void _Connect(wxEvtHandler* evthandler, wxEventType id, wxObjectEventFunction func);
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

	static const wxEventType GamePromotedEvent;
	static const wxEventType ApplicationSettingsChangedEvent;

private:
	static GlobalEventManager* m_Instance;

private:
	bool m_eventsDisabled;
	std::map<wxEventType, std::set<wxEvtHandler*> > m_eventsTable;
	const int ANY_EVENT = 0;
};


#endif // SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
