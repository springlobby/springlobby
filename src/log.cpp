/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/log.h>
#include <wx/thread.h>
#include <wx/intl.h>

#include "log.h"
#include "utils/conversion.h"
#include "helper/slconfig.h"
#include "utils/platform.h"
#include "crashreport.h"

#include <lslutils/globalsmanager.h>

#if wxUSE_STD_IOSTREAM
#include <iostream>
#endif


class myLogger: public wxLog
{
public:
	~myLogger()
	{
		if (m_logfile != NULL) {
			fclose(m_logfile);
		}
	}
	myLogger(bool console, const wxString&  logfilepath, bool showgui):
		wxLog(),
		m_console(console),
		m_gui(showgui),
		m_logfile(NULL)
	{
		if (!logfilepath.empty()) {
			m_logfile = fopen(C_STRING(logfilepath), "wb+"); // even if it returns null, wxLogStderr will switch to stderr logging, so it's fine
		}

	}

	virtual void DoLogText(const wxString &msg)
	{
		const std::string std_msg = (STD_STRING(msg) + "\n").c_str();
		if (m_console) {
			printf("%s",std_msg.c_str());
		}
		if (m_logfile != NULL) {
			fwrite(std_msg.c_str(), std_msg.length(), 1, m_logfile);
		}
/*
		if (m_gui) {
			ChatPanel* p = ui().mw().GetChatTab().AddChatPanel();
			if (p!=NULL) {
				p->StatusMessage(TowxString(LogName(l)) + _T(" ") +TowxString(msg));
			}
		}
*/

	}
	void Flush()
	{
		if (m_logfile != NULL) {
			fflush(m_logfile);
		}
	}
private:
	bool m_console;
	bool m_gui;
	FILE* m_logfile;
};


bool Logger::gui = false;

Logger::Logger()
{
}

Logger::~Logger()
{
	gui = false;
}

	//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
wxLogWindow* Logger::InitializeLoggingTargets( wxWindow* /*parent*/, bool console, const wxString&  logfilepath, bool showgui, int verbosity)
{
	delete wxLog::SetActiveTarget(new myLogger(console, logfilepath, showgui));
	switch (verbosity) {
		case 1:
			wxLog::SetLogLevel(wxLOG_FatalError); break;
		case 2:
			wxLog::SetLogLevel(wxLOG_Error); break;
		case 3:
			wxLog::SetLogLevel(wxLOG_Warning); break;
		case 4:
			wxLog::SetLogLevel(wxLOG_Message); break;
		case 5:
			wxLog::SetLogLevel(wxLOG_Status); break;
		case 6:
			wxLog::SetLogLevel(wxLOG_Info); break;
		case 7:
			wxLog::SetLogLevel(wxLOG_Debug); break;
		case 8:
			wxLog::SetLogLevel(wxLOG_Trace); break;
		default: {//meaning loglevel < 0 or > 5 , == 0 is handled seperately
		}
	}
	return NULL; //FIXME
}

void Logger::Shutdown()
{
	gui = false;
}

void Logger::ShowDebugWindow(bool show)
{
	gui = show;
	if (show) {
		wxLogDebug("enabled debug window");
	}
}

extern void lsllogerror(const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(buf, 1024, format, args);
	va_end(args);
	if (len > 0) {
		const std::string msg(buf, len);
		wxLogError(TowxString(msg));
	}
}
