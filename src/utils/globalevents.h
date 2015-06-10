/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H

#include <map>
#include <list>

#include <wx/wx.h>
#include <wx/event.h>


class GlobalEventManager
{
private:
	GlobalEventManager();
	~GlobalEventManager();

public:
	static GlobalEventManager* Instance();
	
public:
	void Subscribe(wxEvtHandler* evh, wxEventType id, wxObjectEventFunction func);
	void UnSubscribe(wxEvtHandler* evh, wxEventType id = 0);
	void Send(wxEventType type);
	void Send(wxCommandEvent event);
	void Send(wxEventType type, void *clientData);
	
private:
	void _Connect(wxEvtHandler* evthandler, wxEventType id, wxObjectEventFunction func);
	void _Disconnect(wxEvtHandler* evthandler, wxEventType id = 0);
	
public:
	static const wxEventType OnDownloadComplete;
	static const wxEventType OnUnitsyncFirstTimeLoad;
	static const wxEventType OnUnitsyncReloaded;
	static const wxEventType OnSpringTerminated;
	static const wxEventType OnSpringStarted;
	static const wxEventType UpdateFinished;
	static const wxEventType OnQuit;
	static const wxEventType OnLogin;
	static const wxEventType PlasmaResourceListParsed;
	static const wxEventType PlasmaResourceListFailedDownload;
	static const wxEventType OnUpdateFinished;
	static const wxEventType OnLobbyDownloaded;
	
	static const wxEventType BattleSyncReload;
	static const wxEventType BattleStartedEvent;
	static const wxEventType UserBattleStatusChangedEvent;
	
private:
	static GlobalEventManager* m_Instance;
	static bool m_eventsDisabled;

private:
	std::map<wxEventType, std::list<wxEvtHandler*> > m_eventsTable;
};


#endif // SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
