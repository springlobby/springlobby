/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SPRING_H
#define SPRINGLOBBY_HEADERGUARD_SPRING_H

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/spring/spring.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include <wx/event.h>

class wxCommandEvent;
class IBattle;
class SpringProcess;
class wxString;


class Spring : public wxEvtHandler
{
public:
	explicit Spring();
	~Spring();

	bool IsRunning() const;
	bool Run(IBattle& battle);
	std::string WriteScriptTxt(IBattle& battle) const;

private:
	void OnTerminated(wxCommandEvent& event);
	bool LaunchEngine();

	int crash_count;
	SpringProcess* m_process;
	bool m_running;

	std::string engine_path;
	wxArrayString engine_params;

	DECLARE_EVENT_TABLE()
};

Spring& spring();

#endif // SPRINGLOBBY_HEADERGUARD_SPRING_H
