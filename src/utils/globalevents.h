/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H


#include <wx/event.h>

class wxEvtHandler;

class GlobalEvent
{
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
	static const wxEventType BattleSyncReload;
	static const wxEventType OnUpdateFinished;
	static const wxEventType OnLobbyDownloaded;
	static void Send(wxEventType type);
	static void Send(wxCommandEvent event);

	GlobalEvent();
	~GlobalEvent();

	void ConnectGlobalEvent(wxEvtHandler* evh, wxEventType id, wxObjectEventFunction func);

private:
	static void _Connect(wxEvtHandler* evthandler, wxEventType id, wxObjectEventFunction func);
	static void _Disconnect(wxEvtHandler* evthandler, wxEventType id = 0);
	wxEvtHandler* m_handler;
};


#endif // SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
