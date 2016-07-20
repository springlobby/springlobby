/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CHATLOG_H_INCLUDED
#define CHATLOG_H_INCLUDED

#include <wx/string.h>
#include <wx/file.h>
#include <wx/arrstr.h>

/** Handles chat-log operations for a single chat room on a server.
 */
class ChatLog
{
public:
	/** Constructor.  Prepares the %ChatLog to accept log messages for
	 * the given chat room or channel on the given server.
	 *
	 * @param logname Name of the logfile this object will
	 * accept messages for.
	 */
	explicit ChatLog(const wxString& logname);
	ChatLog();


	/** Destructor.  Calls CloseSession if the log file is currently
	 * opened.
	 */
	~ChatLog();

	/** Append a time-stamped message to the log file.  Retrieves a
	 * time-stamp string from LogTime.
	 *
	 * @note This does nothing, successfully, if chat logging is
	 * disabled.
	 *
	 * @param text Message text to log.
	 *
	 * @return @c false if an error was encountered while writing to
	 * the log file, and @c true otherwise.
	 *
	 * @see LogEnabled LogTime
	 */
	bool AddMessage(const wxString& text);

	/** Check if chat logging is enabled.
	 *
	 * @return @c true if chat logging is enabled, and @c false if it
	 * is not.
	 */
	bool LogEnabled();

	const wxArrayString& GetLastLines() const;

	bool SetLogFile(const wxString& logname);

	/** Get the path (filename) to the current log file.
	 *
	 * @return A platform-specific logfile path string.
	 */
	wxString GetCurrentLogfilePath() const;

private:
	/** Closes the current "session" in the log file by appending a
	 * session-closed notice to end before closing the file.
	 *
	 * @post The file is closed, and any further calls to AddMessage
	 * will reopen the log file, starting a new session.
	 */
	void CloseSession();

	/** Create the directory in which the current logfile is saved.
	 *
	 * @return @c true on success, @c false on failure.
	 */
	bool CreateCurrentLogFolder();
	bool OpenLogFile();

	wxString m_logname;

	bool m_active;
	wxFile m_logfile;

	wxArrayString m_last_lines;

	void FillLastLineArray();
};

#endif // CHATLOG_H_INCLUDED
