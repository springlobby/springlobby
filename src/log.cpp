/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "log.h"
//#include "ui.h"
//#include "mainwindow.h"
//#include "mainchattab.h"
//#include "chatpanel.h"
#include "utils/conversion.h"
#include "helper/slconfig.h"
#include <lslutils/globalsmanager.h>
#include <wx/log.h>
#include <wx/thread.h>

bool Logger::gui = false;
bool Logger::enabled = false;

Logger::Logger()
{
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

void Logger::Initialize()
{
	//enabled = true;
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
