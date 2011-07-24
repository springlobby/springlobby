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
     * @param server Alias or hostname for the server on which the
     * chat room or channel is located.
     *
     * @param room Name of the chat room or channel this object will
     * accept messages for.
     */
    ChatLog(const wxString& server,const wxString& room);

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

    /** Get a string, representing the current time-of-day, suitable
     * for prepending to messages.  This is used by AddMessage.
     *
     * @return a string including hours and minutes.
     *
     * @see AddMessage
     */
    wxString LogTime();

    void OpenInEditor();
    const wxArrayString& GetLastLines( ) const;

  protected:
    /** Closes the current "session" in the log file by appending a
     * session-closed notice to end before closing the file.
     *
     * @post The file is closed, and any further calls to AddMessage
     * will reopen the log file, starting a new session.
     */
    void CloseSession();


    /** Get the path (filename) to the current log file.
     *
     * @return A platform-specific logfile path string.
     */
    wxString GetCurrentLogfilePath() const;


    /** Write a line of text to the output file, without any further
     * formatting.
     *
     * @param text The line to write.
     *
     * @return @c true if the line was successfully written, and @c
     * false otherwise.
     */
    bool WriteLine(const wxString& text);

    /** Create the directory in which the current logfile is saved.
     *
     * @return @c true on success, @c false on failure.
     */
    bool CreateCurrentLogFolder();
    bool OpenLogFile();

    wxString m_server;
    wxString m_room;

    bool m_active;
    wxFile m_logfile;

    wxArrayString m_last_lines;

    void FillLastLineArray();

};

#endif // CHATLOG_H_INCLUDED

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

