/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_TASKBAR_H
#define SPRINGLOBBY_HEADERGUARD_TASKBAR_H

#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/event.h>
#include <wx/timer.h>

class TaskBar : public wxPanel
{
public:
	TaskBar(wxWindow* parent);
	~TaskBar();
	void OnTimer(wxTimerEvent& /*event*/);

private:
	void OnDownloadStarted();
	void OnDownloadComplete();
	void UpdateDisplay();
	void UpdateProgress();

	wxTimer* timer;
	wxStaticText* text;
	wxGauge* gauge;
	int state;
	wxString downloadName;
	int overalSize;
	unsigned int overalProgress;
	int unfinishedTasks;
	int finishedCounter;
	DECLARE_EVENT_TABLE()
};

#endif // TASKBAR_H
