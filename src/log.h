/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef LOG_H
#define LOG_H

#include <string>
#include <wx/log.h>

class wxString;
class wxLogWindow;
class wxWindow;

class Logger
{
public:
	Logger();
	~Logger();
	static wxLogWindow* InitializeLoggingTargets(wxWindow* parent, bool console, const wxString& logfilepath, bool showgui, int verbosity);
	static void Shutdown();
	static void ShowDebugWindow(bool show);

private:
};

#define slLogDebugFunc(format, ...) \
	wxLogDebug(format, ##__VA_ARGS__);

#include <stdexcept>
class assert_exception : public std::runtime_error
{
public:
	assert_exception(std::string msg)
	    : std::runtime_error(msg)
	{
		wxLogWarning("%s", msg.c_str());
	}
};

#define ASSERT_LOGIC(cond, msg)                                                                                                                 \
	if (!(cond)) {                                                                                                                          \
		wxLogError(wxString::Format(_T("logic error ( %s:%d ): %s"), TowxString(__FILE__).c_str(), __LINE__, TowxString(msg).c_str())); \
		throw std::logic_error(msg);                                                                                                    \
	}

#define ASSERT_EXCEPTION(cond, msg)                                                                                                 \
	if (!(cond)) {                                                                                                              \
		wxLogWarning(_T("runtime assertion ( %s:%d ): %s"), TowxString(__FILE__).c_str(), __LINE__, wxString(msg).c_str()); \
		throw assert_exception(std::string(wxString(msg).mb_str()));                                                        \
	}

#endif // LOG_H
