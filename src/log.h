/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef DEBUG_H
#define DEBUG_H

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
	};
	void Log(level l, const char* format, ...) const;
	void Log(level l, const wxString& format, ...) const;

	void Enable(bool enable);
private:
	static wxString LogName(level l);
	bool enabled;
};

Logger& logger();

//shortcuts
#define logError(msg, ...) logger().Log(LOG_ERROR, msg, ##__VA_ARGS__)
#define logWarning(msg, ...) logger().Warning(LOG_WARNING, msg, ##__VA_ARGS__)
#define logInfo(msg, ...) logger().Info(LOG_INFO, msg, ##__VA_ARGS__)

#endif // DEBUG_H
