/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <cstdio>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <sys/time.h>
#include <vector>
#include <time.h>

#include <wx/log.h>

#include "log.h"
#include "gui/ui.h"
#include "gui/mainwindow.h"
#include "utils/conversion.h"

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
		const int src_fn_offset = std::max(0, static_cast<int>(std::strlen(info.filename)) - src_fn_size);
		const char* src_fn = info.filename + src_fn_offset;

		const std::string log_prefix = stdprintf("%s %-7s %15.15s:%-4d",
		  GetTimeString().c_str(), LogLevelToString(loglevel),
		  src_fn, info.line);

		char delim = ' '; // indicates that this is a new message, continuations use '+'
		// sol=Start-Of-Line, eol=End-Of-Line. We have at least one execution of the below loop.
		// eol should be one past the end character for substring length = eol - sol to work
		for (size_t sol = 0, eol = 0; eol < msg.Len(); sol = ++eol) {
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
			delim = '+'; // not first line any more
		}

		Flush();
		/*
	  if (m_gui) {
		  ChatPanel* p = ui().mw().GetChatTab().AddChatPanel();
		  if (p!=NULL) {
			  p->StatusMessage(TowxString(LogName(l)) + _T(" ") +TowxString(msg));
		  }
	  }*/
	}

	const char* LogLevelToString(wxLogLevel level)
	{
		assert(level < 8); // just in case

		static const char* levelNames[] = {
		    "Fatal",
		    "Error",
		    "Warning",
		    "Message",
		    "Status",
		    "Info",
		    "Debug",
		    "Trace"};

		return levelNames[static_cast<int>(level)];
	}

	void Flush() override
	{
		for (FILE* lf : m_log_files)
			fflush(lf);
	}

private:
	//	bool m_gui;
	std::vector<FILE*> m_log_files;
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

extern void lslLogError(const char* fileName, int line, const char* funcName,
                        const char* format, ...)
{
	va_list args;
	va_start(args, format);
	// Warning instead of Error as it is not yet known whether lsl FIXMEs have been fixed.
	wxLogger(wxLOG_Error, fileName, line, funcName, "lsl").LogV(format, args);
	va_end(args);
}

extern void lslLogWarning(const char* fileName, int line, const char* funcName,
                          const char* format, ...)
{
	va_list args;
	va_start(args, format);
	wxLogger(wxLOG_Warning, fileName, line, funcName, "lsl").LogV(format, args);
	va_end(args);
}

extern void lslLogInfo(const char* fileName, int line, const char* funcName,
                       const char* format, ...)
{
	va_list args;
	va_start(args, format);
	wxLogger(wxLOG_Info, fileName, line, funcName, "lsl").LogV(format, args);
	va_end(args);
}

extern void lslLogDebug(const char* fileName, int line, const char* funcName,
                        const char* format, ...)
{
	va_list args;
	va_start(args, format);
	wxLogger(wxLOG_Debug, fileName, line, funcName, "lsl").LogV(format, args);
	va_end(args);
}


extern void prdLogRaw(const char/*fileName*/, int /*line*/, const char* /*funcName*/,
                      const char* /*format*/, va_list /*args*/)
{
	// used for e.g. progress bars, we do not want that (yet)
}
extern void prdLogError(const char* fileName, int line, const char* funcName,
                        const char* format, va_list args)
{
	wxLogger(wxLOG_Error, fileName, line, funcName, "prd").LogV(format, args);
}
extern void prdLogInfo(const char* fileName, int line, const char* funcName,
                       const char* format, va_list args)
{
	wxLogger(wxLOG_Info, fileName, line, funcName, "prd").LogV(format, args);
}
extern void prdLogDebug(const char* fileName, int line, const char* funcName,
                        const char* format, va_list args)
{
	wxLogger(wxLOG_Debug, fileName, line, funcName, "prd").LogV(format, args);
}
