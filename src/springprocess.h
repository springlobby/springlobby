/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/spring/springprocess.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/

#include <wx/thread.h>
#include <wx/string.h>
#include <wx/process.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE(wxEVT_SPRING_EXIT, 1)
END_DECLARE_EVENT_TYPES()

class Spring;

class SpringProcess : public wxThread
{
public:
	explicit SpringProcess(Spring& sp);
	~SpringProcess();
	void OnExit();
	void SetCommand(const wxString& cmd, const wxArrayString& params);
	void* Entry();

private:
	Spring& m_sp;
	wxString m_cmd;
	wxArrayString m_params;
	int m_exit_code;
};

const int PROC_SPRING = wxID_HIGHEST;

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
