/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/log.h>

#include <iostream>
#include <string>

class TestWxLogger : public wxLog
{
public:
	TestWxLogger(bool totalSilence)
	    : wxLog()
	{
		this->totalSilence = totalSilence;
	}

	void DoLogRecord(wxLogLevel loglevel, const wxString& msg, const wxLogRecordInfo& info) override
	{
		if (!totalSilence) {
			const std::string msg_str = msg.ToStdString();
			const std::string std_msg = wxString::Format(_T("%s %s:%d %s\n"), LogLevelToString(loglevel).c_str(), info.filename, info.line, msg_str.c_str()).ToStdString();
			std::cout << std_msg;
		}
	}

	std::string LogLevelToString(wxLogLevel level)
	{
		const char* levelName[] = {
		    "Fatal   ",
		    "Error   ",
		    "Warning ",
		    "Message ",
		    "Status  ",
		    "Info    ",
		    "Debug   ",
		    "Trace   "};

		return std::string(levelName[(int)level]);
	}

private:
	bool totalSilence;
};

//Sets current wxLog logger
void InitWxLogger(bool totalSilent = true)
{
	delete wxLog::SetActiveTarget(new TestWxLogger(totalSilent));
	wxLog::SetLogLevel(wxLOG_Trace);
}
