/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_TASKBAR_H
#define SPRINGLOBBY_HEADERGUARD_TASKBAR_H

#include <wx/panel.h>

class wxTimer;
class wxStaticText;
class wxGauge;
class wxTimerEvent;

class TaskBar : public wxPanel
{
public:
	TaskBar(wxWindow* parent);
	~TaskBar();

private:
	void OnDownloadStarted(wxCommandEvent& event);
	void OnDownloadComplete(wxCommandEvent& event);
	void OnDownloadFailed(wxCommandEvent& event);
	void OnDownloadProgress(wxCommandEvent& event);

	void UpdateProgress();
	void OnTimer(wxTimerEvent&);
	void EnsureTimerRemoved();

	wxTimer* timer;
	wxStaticText* text;
	wxGauge* gauge;

	DECLARE_EVENT_TABLE()
};

#endif // TASKBAR_H
