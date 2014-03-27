/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef DEBUG_H
#define DEBUG_H

class wxString;

class Logger
{
public:
	Logger();
	~Logger();
	void Error(const wxString& message);
	void Warning(const wxString& message);
	void Info(const wxString& message);
	void Error(const char* message);
	void Warning(const char* message);
	void Info(const char* message);

};

Logger& logger();

//shortcuts
#define logError(msg) logger().Error(msg)
#define logWarning(msg) logger().Warning(msg)
#define logInfo(msg) logger().Info(msg)

#endif // DEBUG_H
