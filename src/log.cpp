/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <mutex>
#include <sys/time.h>
#include <wx/log.h>

#include "log.h"
#include "utils/conversion.h"
#include "gui/ui.h"
#include "gui/mainwindow.h"
#include "time.h"

#if wxUSE_STD_IOSTREAM
#include <iostream>
#endif

static bool gui = false;

class myLogger : public wxLog
{
public:
	~myLogger()
	{
		if (m_logfile != NULL) {
			fclose(m_logfile);
		}
	}
	myLogger(bool console, const wxString& logfilepath, bool /*showgui*/)
	    : wxLog()
	    , m_console(console)
	    ,
	    //		m_gui(showgui),
	    m_logfile(NULL)
	{
		if (!logfilepath.empty()) {
			m_logfile = fopen(C_STRING(logfilepath), "wb+"); // even if it returns null, wxLogStderr will switch to stderr logging, so it's fine
		}
	}

	static std::string GetTimeString()
	{
		char buffer[512];
		char buffer2[512];
		struct timeval tv;

		gettimeofday(&tv, NULL);

		const struct tm* tm_info = localtime(&tv.tv_sec);
		const size_t len = strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
		if (len <= 0)
			return "";
		const int res = snprintf(buffer2, sizeof(buffer2), "%s.%03ld", buffer, tv.tv_usec / 1000);
		if (res > 0)
			return std::string(buffer2, res);
		return "";
	}

	// catch and process all log messages
	void DoLogRecord(wxLogLevel loglevel, const wxString& msg, const wxLogRecordInfo& info) override
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (gui && (loglevel == wxLOG_Error || loglevel == wxLOG_FatalError)) // show user only errors
		{
			wxCommandEvent* event = new wxCommandEvent(wxEVT_SHOW, ui().mw().mySHOW_ERROR_MESSAGE);
			event->SetString(wxString(msg).c_str());
			ui().mw().GetEventHandler()->QueueEvent(event);
		}


		const std::string std_msg = stdprintf("%s %s %s:%d %s\n", GetTimeString().c_str(), LogLevelToString(loglevel).c_str(), info.filename, info.line, (STD_STRING(wxString(msg))).c_str());
		if (m_console) {
			std::cout << std_msg;
		}
		if (m_logfile != NULL) {
			fwrite(std_msg.c_str(), std_msg.length(), 1, m_logfile);
			fflush(m_logfile);
		}
		/*
	  if (m_gui) {
		  ChatPanel* p = ui().mw().GetChatTab().AddChatPanel();
		  if (p!=NULL) {
			  p->StatusMessage(TowxString(LogName(l)) + _T(" ") +TowxString(msg));
		  }
	  }*/
	}

	std::string LogLevelToString(wxLogLevel level)
	{
		assert(level < 8); // just in case

		const char* levelName[] = {
		    "Fatal",
		    "Error",
		    "Warning",
		    "Message",
		    "Status",
		    "Info",
		    "Debug",
		    "Trace"};

		return std::string(levelName[(int)level]);
	}

	void Flush() override
	{
		if (m_logfile != NULL) {
			fflush(m_logfile);
		}
	}

private:
	bool m_console;
	//	bool m_gui;
	FILE* m_logfile;
	std::mutex m_mutex;
};


Logger::Logger()
{
}

Logger::~Logger()
{
	gui = false;
}

//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
wxLogWindow* Logger::InitializeLoggingTargets(wxWindow* /*parent*/, bool console, const wxString& logfilepath, bool showgui, int verbosity)
{
	delete wxLog::SetActiveTarget(new myLogger(console, logfilepath, showgui));
	wxLog::SetVerbose(verbosity > 5);
	switch (verbosity) {
		case 1:
			wxLog::SetLogLevel(wxLOG_FatalError);
			break;
		case 2:
			wxLog::SetLogLevel(wxLOG_Error);
			break;
		case 3:
			wxLog::SetLogLevel(wxLOG_Warning);
			break;
		case 4:
			wxLog::SetLogLevel(wxLOG_Message);
			break;
		case 5:
			wxLog::SetLogLevel(wxLOG_Status);
			break;
		case 6:
			wxLog::SetLogLevel(wxLOG_Info);
			break;
		case 7:
			wxLog::SetLogLevel(wxLOG_Debug);
			break;
		case 8:
			wxLog::SetLogLevel(wxLOG_Trace);
			break;
		default: { //meaning loglevel < 0 or > 5 , == 0 is handled seperately
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
	/*
	if (show) {
		wxLogDebug("enabled debug window");
	}
*/
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
		wxLogWarning("Error: FIXME: %s", msg.c_str()); //FIXME: lsl throws a lot of errors
	}
}

extern void lsllogdebug(const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(buf, 1024, format, args);
	va_end(args);
	if (len > 0) {
		const std::string msg(buf, len);
		wxLogDebug("%s", msg.c_str());
	}
}

extern void lsllogwarning(const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(buf, 1024, format, args);
	va_end(args);
	if (len > 0) {
		const std::string msg(buf, len);
		wxLogWarning("%s", msg.c_str());
	}
}

extern void lslloginfo(const char* format, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(buf, 1024, format, args);
	va_end(args);
	if (len > 0) {
		const std::string msg(buf, len);
		wxLogInfo("%s", msg.c_str());
	}
}
