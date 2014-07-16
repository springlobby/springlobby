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


bool Logger::gui = false;
bool Logger::enabled = false;

static void AssertHandler(const wxString &file, int line, const wxString &func, const wxString &cond, const wxString &msg)
{
	assert(false);
}

Logger::Logger()
{
	wxSetAssertHandler(AssertHandler);
}

Logger::~Logger()
{
	gui = false;
}

std::string Logger::LogName(level l)
{
	switch(l){
		case LOG_ERROR: return "error";
		case LOG_WARNING: return "warning";
		case LOG_INFO: return "info";
		case LOG_DEBUG: return "debug";
		case LOG_TRACE: return "trace";
	}
	return "";
}

static bool isinlog = false;
void Logger::Log(level l, const char* format, ...)
{
	if (!enabled) {
		return;
	}
	char buf[1024];
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(buf, 1024, format, args);
	if (len == 0) {
		return;
	}
	va_end(args);

	const std::string msg(buf, len);
	if (!wxThread::IsMain()) {
		wxLogInfo(_T("thread %ld: %s %s"),wxThread::GetCurrentId(), LogName(l).c_str(), msg.c_str());
		return;
	}
	if (isinlog || !gui) {
		wxLogInfo(_T("%s"), msg.c_str());
		return;
	}
	isinlog = true; //the following calls could trigger a log, too, protect this
/*	ChatPanel* p = ui().mw().GetChatTab().AddChatPanel();
	if (p == NULL) {
		wxLogInfo("%s", msg.c_str());
		return;
	}
	p->StatusMessage(TowxString(LogName(l)) + _T(" ") +TowxString(msg));
*/
	wxLogDebug(_T("%s"), msg.c_str());
	isinlog = false;
}

void Logger::Log(level l, const wxString& format, ...)
{
	va_list args;
	va_start(args, format);
	const wxString tmp = wxString::FormatV(format, args);
	va_end(args);
	Log(l, STD_STRING(tmp).c_str());
}

void Logger::Log(level l, const char* file, const char* function, const int line, const char* format, ...)
{
	char msg[1024];
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(msg, 1024, format, args);
	if (len == 0) {
		return;
	}
	va_end(args);
	char fullmsg[1024];
	const int fullen = snprintf(fullmsg, 1024, "%s %s():%d: %s", file, function, line, msg);
	if (fullen == 0) {
		return;
	}
	Log(l, "%s", fullmsg);
}

	//! @brief Initializes the logging functions.
///initializes logging in an hidden stream and std::cout/gui messages
wxLogWindow* Logger::InitializeLoggingTargets( wxWindow* parent, bool console, const wxString&  logfilepath, bool showgui, int verbosity, wxLogChain* logChain )
{
	wxLogWindow* loggerwin = NULL;
	if ( console ) {
#if wxUSE_STD_IOSTREAM
		///std::cout logging
		logChain = new wxLogChain( new wxLogStream( &std::cout ) );
#else
		///std::cerr logging
		logChain = new wxLogChain( new  wxLogStderr( 0 ) );
#endif
	}

	if (showgui) {
		///gui window logging
		loggerwin = new wxLogWindow(parent, IdentityString( _("%s error console") ), showgui );
		logChain = new wxLogChain( loggerwin );
	}

	if (!console && !showgui) {
		new wxLogNull(); //memleak but disables logging as no log target exists
		//FIXME: there should be a cleaner way (like just not showing message boxes)
	}

	if (!logfilepath.empty()) {
		FILE* logfile = fopen(C_STRING(logfilepath), "w"); // even if it returns null, wxLogStderr will switch to stderr logging, so it's fine
		logChain = new wxLogChain( new  wxLogStderr( logfile ) );
	}

#if wxUSE_STD_IOSTREAM
	///hidden stream logging for crash reports
	wxLog *loggercrash = new wxLogStream( &CrashReport::instance().crashlog );
	logChain = new wxLogChain( loggercrash );

//	logCrashChain->SetLogLevel( wxLOG_Trace );
//	logCrashChain->SetVerbose( true );
#endif

	if (logChain!=NULL) {
		switch (verbosity) {
			case 0: case 1:
				logChain->SetLogLevel( wxLOG_FatalError ); break;
			case 2:
				logChain->SetLogLevel( wxLOG_Error ); break;
			case 4:
				logChain->SetLogLevel( wxLOG_Message ); break;
			case 5:
				logChain->SetLogLevel( wxLOG_Trace );
				logChain->SetVerbose( true ); break;
			case 3:
			default: {//meaning loglevel < 0 or > 5 , == 0 is handled seperately
			}
		}
	}
	return loggerwin;
}

void Logger::Shutdown()
{
}

void Logger::ShowDebugWindow(bool show)
{
	gui = show;
//	enabled = show;
	Log(LOG_INFO, "enabled debug window");
}
