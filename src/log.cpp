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

void Logger::Error(const wxString& message)
{
	if (!enabled) {
		return;
	}
	ChatPanel* p = ui().mw().GetChatTab().AddChatPanel();
	if (p == NULL) {
		printf("%s\n", STD_STRING(message).c_str());
		return;
	}
	p->StatusMessage(message);
}

void Logger::Warning(const wxString& message)
{
	Error(message); //FIXME
}

void Logger::Info(const wxString& message)
{
	Error(message); //FIXME
}

Logger& logger(){
	static LSL::Util::LineInfo<Logger> m( AT );
	static LSL::Util::GlobalObjectHolder<Logger, LSL::Util::LineInfo<Logger> > m_sett( m );
	return m_sett;
}

void Logger::Error(const char* message)
{
	Error(TowxString(message));
}

void Logger::Warning(const char* message)
{
	Warning(TowxString(message));

}

void Logger::Info(const char* message)
{
	Info(TowxString(message));
}

void Logger::Enable(bool enable)
{
	enabled = enable;
	cfg().Write(_T("/debug"), enabled);
	if (enable) {
		Info(_T("Debug window enabled!"));
	}
}
