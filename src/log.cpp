/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "log.h"
#include "ui.h"
#include "mainwindow.h"
#include "mainchattab.h"
#include "chatpanel.h"
#include "utils/conversion.h"
#include "helper/slconfig.h"
#include <lslutils/globalsmanager.h>


SLCONFIG("/debug", false, "Show debug window");


Logger::Logger()
{
	enabled = cfg().ReadBool(_T("/debug"));
}

Logger::~Logger()
{
	//dtor
}

wxString Logger::LogName(level l)
{
	switch(l){
		case LOG_ERROR: return _T("error");
		case LOG_WARNING: return _T("warning");
		case LOG_INFO: return _T("info");
	}
	return wxEmptyString;
}

void Logger::Log(level l, const char* format, ...) const
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
		printf("Log from non-main: %s\n", msg.c_str());
		return;
	}
	ChatPanel* p = ui().mw().GetChatTab().AddChatPanel();
	if (p == NULL) {
		printf("%s\n", msg.c_str());
		return;
	}
	p->StatusMessage(LogName(l) + _T(" ") +TowxString(msg));
}

void Logger::Log(level l, const wxString& format, ...) const
{
	va_list args;
	va_start(args, format);
	const wxString tmp = wxString::FormatV(format, args);
	va_end(args);
	Log(l, STD_STRING(tmp).c_str());
}


Logger& logger(){
	static LSL::Util::LineInfo<Logger> m( AT );
	static LSL::Util::GlobalObjectHolder<Logger, LSL::Util::LineInfo<Logger> > m_sett( m );
	return m_sett;
}

void Logger::Enable(bool enable)
{
	enabled = enable;
	cfg().Write(_T("/debug"), enabled);
	if (enable) {
		Log(LOG_INFO, "Debug window enabled!");
	}
}
