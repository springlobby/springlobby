/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef LOG_H
#define LOG_H

#include <string>

class wxString;

class Logger
{
public:
	Logger();
	~Logger();
	enum level{
		LOG_ERROR,
		LOG_WARNING,
		LOG_INFO,
		LOG_DEBUG,
		LOG_TRACE,
	};
	static void Log(level l, const char* format, ...);
	static void Log(level l, const wxString& format, ...);
	static void Log(level l, const char* file, const char* function, const int line, const char* format, ...);

	static void Initialize();
	static void Shutdown();
	static void ShowDebugWindow(bool show);
private:
	static std::string LogName(level l);
	static bool enabled;
	static bool gui;
};

#define slLogError(logmsg, ...) Logger::Log(Logger::LOG_ERROR, logmsg, ##__VA_ARGS__)
#define slLogWarning(logmsg, ...) Logger::Log(Logger::LOG_WARNING, logmsg, ##__VA_ARGS__)
#define slLogMessage(logmsg, ...) Logger::Log(Logger::LOG_INFO, logmsg, ##__VA_ARGS__)
#define slLogTrace(logmsg, ...) Logger::Log(Logger::LOG_TRACE, logmsg, ##__VA_ARGS__)
#define slLogInfo(logmsg, ...) Logger::Log(Logger::LOG_INFO, logmsg, ##__VA_ARGS__)
#define slLogDebug(logmsg, ...) Logger::Log(Logger::LOG_DEBUG, logmsg, ##__VA_ARGS__)

#define slLogDebugFunc(logmsg, ...) Logger::Log(Logger::LOG_DEBUG,__FILE__, __FUNCTION__, __LINE__, logmsg, ##__VA_ARGS__)

#define ASSERT_LOGIC(cond,msg) if(!(cond))\
{slLogError(_T("logic error ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );}

#define ASSERT_EXCEPTION(cond,msg) if(!(cond))\
{slLogMessage(_T("runtime assertion ( %s:%d ): %s"), TowxString(__FILE__).c_str(),__LINE__ , wxString(msg).c_str() );}

#include <stdexcept>
class assert_exception : public std::runtime_error
{
  public:
   assert_exception(std::string msg) : std::runtime_error(msg) {}
};

#endif // LOG_H
