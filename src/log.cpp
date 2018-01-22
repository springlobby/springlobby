/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <cstdio>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <sys/time.h>
#include <vector>

#include <wx/log.h>

#include "log.h"
#include "utils/conversion.h"
#include "gui/ui.h"
#include "gui/mainwindow.h"
#include "time.h"

static bool gui = false;

class myLogger : public wxLog
{
public:
	~myLogger()
	{
		for (FILE* lf : m_log_files)
			fclose(lf);
	}
	myLogger(bool console, const wxString& logfilepath, bool /*showgui*/)
	    : wxLog()
	    //		m_gui(showgui),
	{
		if (console)
			m_log_files.push_back (stdout);
		if (!logfilepath.empty()) {
			// even if it returns null, wxLogStderr will switch to stderr logging, so it's fine
			// TODO: it will?
			FILE* log = fopen(C_STRING(logfilepath), "wb+");
			if (nullptr == log)
				wxLogError ("Unable to open log file %s for writing!", logfilepath);
			else
				m_log_files.push_back (log);
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

		// only print the last src_fn_size chars of file name
		const int src_fn_size = 15;
		const size_t src_fn_offset = std::max (static_cast<size_t>(0),
		                                       std::strlen(info.filename) - src_fn_size);
		const char* src_fn = info.filename + src_fn_offset;

		const std::string log_prefix = stdprintf("%s %-7s %15.15s:%-4d",
		  GetTimeString().c_str(), LogLevelToString(loglevel).c_str(),
		  src_fn, info.line);

		size_t sol = 0, eol; // Start-Of-Line, End-Of-Line
		// eol should be one past the end character for substring length = eol - sol to work
		char delim = ' ';
		do { // at least one loop.
			eol = msg.find("\n", sol);
			if (wxString::npos == eol) {
				eol = msg.Len();
				if (sol >= eol) // real end condition
					break;
			}
			wxString line_msg = msg.Mid(sol, eol-sol);

			for (FILE* lf : m_log_files) {
				fwrite(log_prefix.c_str(), log_prefix.length(), 1, lf);
				fwrite(&delim, 1, 1, lf);
				fwrite(line_msg.c_str(), line_msg.length(), 1, lf);
				fwrite("\n", 1, 1, lf);
			}
			sol = ++eol;
			delim = '+'; // not first line any more
		} while (eol < msg.Len());

		Flush();
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
		for (FILE* lf : m_log_files)
			fflush(lf);
	}

private:
	std::mutex m_mutex;
	//	bool m_gui;
	std::vector<FILE*> m_log_files;
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
