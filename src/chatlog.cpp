/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: ChatLog
//
#include <wx/string.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/dir.h>
#include <wx/datetime.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <stdexcept>

#include "chatlog.h"
#include "settings.h"
#include "utils/slconfig.h"
#include "utils/conversion.h"

#include "utils/platform.h"
#include "utils/slpaths.h"

#ifndef TEST
SLCONFIG("/ChatLog/chatlog_enable", true, "Log chat messages");
#endif

ChatLog::ChatLog()
    : m_active(false)
{
}

ChatLog::ChatLog(const wxString& logname)
    : m_logname(logname)
    , m_active(LogEnabled())
    , m_logfile()
{
	wxLogMessage(_T( "ChatLog::ChatLog( %s )" ), logname.c_str());
	SetLogFile(logname);
}

bool ChatLog::SetLogFile(const wxString& logname)
{
	if (logname == wxEmptyString) {
		m_logname = logname;
		CloseSession();
		return true;
	}

	m_logname.Replace(wxT(":"), wxT("_"));
	if (logname != m_logname) {
		if (m_logfile.IsOpened()) {
			CloseSession();
		}
		m_logname = logname;
		OpenLogFile();
	}
	return m_active;
}

ChatLog::~ChatLog()
{
	wxLogMessage(_T( "%s -- ChatLog::~ChatLog()" ), m_logname.c_str());
	CloseSession();
}

void ChatLog::CloseSession()
{
	if (!m_logfile.IsOpened()) {
		return;
	}

	AddMessage(wxEmptyString, _("### Session Closed at [%Y-%m-%d %H:%M]"));
	m_logfile.Flush();
	m_active = false;
	m_logfile.Close();
}

bool ChatLog::AddMessage(const wxString& text, const wxString& timeformat)
{
	if (!LogEnabled()) {
		return true;
	}
	if (!m_active) { //logging is enabled, logfile should be writeable
		return false;
	}
	wxString logtime = wxDateTime::Now().Format(timeformat);
	const bool res = m_logfile.Write(logtime + text + wxTextBuffer::GetEOL(), wxConvUTF8);
	if (!res) {
		wxLogWarning(_T("Couldn't write to %s"), m_logname.c_str());
		m_logfile.Close();
	}
	return res;
}


bool ChatLog::CreateCurrentLogFolder()
{
	const wxString path = wxFileName(GetCurrentLogfilePath()).GetPath();
	if (!wxFileName::Mkdir(path, 0755, wxPATH_MKDIR_FULL)) {
		wxLogWarning(_T( "can't create logging folder: %s" ), path.c_str());
		m_active = false;
		return false;
	}
	return true;
}

bool ChatLog::OpenLogFile()
{
	wxLogMessage(_T( "OpenLogFile( ) %s" ), m_logname.c_str());
	wxString logFilePath(GetCurrentLogfilePath());

	if (!LogEnabled()) {
		return true;
	}

	if (!CreateCurrentLogFolder()) {
		return false;
	}

	if (!wxFile::Exists(logFilePath)) {
		m_logfile.Create(logFilePath);
	} else {
		m_logfile.Open(logFilePath, wxFile::read_write);
	}

	if (!m_logfile.IsOpened()) {
		wxLogWarning(_T( "Can't open log file %s" ), logFilePath.c_str());
		m_active = false;
		return false;
	}

	FillLastLineArray();
	m_active = true;

	return AddMessage(wxEmptyString, _T( "### Session Start at [%Y-%m-%d %H:%M]" ));
}

const wxArrayString& ChatLog::GetLastLines() const
{
	return m_last_lines;
}


wxString ChatLog::GetCurrentLogfilePath() const
{
#ifdef TEST
	return wxFileName::GetTempDir() + _T("/sltest.log");
#else
	const wxString serverdir = SlPaths::SantinizeFilename(STD_STRING(sett().GetDefaultServer()));
	return TowxString(SlPaths::GetChatLogLoc()) + serverdir + wxFileName::GetPathSeparator() + m_logname + _T( ".txt" );
#endif
}

bool ChatLog::LogEnabled()
{
#ifdef TEST
	return true;
#else
	return cfg().ReadBool(_T("/ChatLog/chatlog_enable"));
#endif
}

/* read block at possition offset from file */
static inline ssize_t readblock(wxFile& fd, void* buffer, size_t size, off_t offset)
{
	assert(offset >= 0);
	if (offset < 0) {
		return -1;
	}

	if (fd.Seek(offset) != offset) {
		return -1;
	}

	return fd.Read(buffer, size);
}


/** Calculate the next read position for find_tail_sequences (below) */
static inline off_t next_read_position(off_t last_read_position, size_t read_size, size_t read_overlap)
{
	return std::max(static_cast<long signed int>(last_read_position - read_size - read_overlap), (long signed int)0);
}

/** Find an arbitrary number of delimited strings at the end of a file.  This
 * function provides the core of our last-lines-loader implementation.
 *
 * @param fd File descriptor to operate on.  This must be opened for reading.
 *
 * @param bytes Delimiter sequence.
 *
 * @param bytes_length Number of significant bytes in @p bytes.
 *
 * @param count Number of tail sequences to find.
 *
 * @param out Destination string array.
 */
static size_t find_tail_sequences(wxFile& fd, const char* bytes, size_t bytes_length, size_t count, wxArrayString& out)
{
	size_t count_added(0);

	/* We overlap the file reads a little to avoid splitting (and thus missing) the
	   delimiter sequence.  */
	const size_t read_overlap = bytes_length - 1;
	const size_t read_size = BUFSIZ;

	const off_t log_length = fd.SeekEnd();
	bool have_last_pos = false;
	char buf[read_size];
	off_t last_found_pos = 0;
	off_t last_read_position = log_length + read_overlap;

	/* We read `read_size'-byte blocks of the file, starting at the end and working backwards. */
	while (count_added < count && last_read_position > 0) {
		const off_t read_position = next_read_position(last_read_position, read_size, read_overlap);
		const size_t bytes_read = readblock(fd, buf, std::min(static_cast<off_t>(read_size), last_read_position - read_position), read_position);

		/* In each block, we search for `bytes', starting at the end.  */
		for (ssize_t i = bytes_read - read_overlap - 1; i >= 0; i--) {
			if (strncmp((buf + i), bytes, bytes_length) == 0) {
				const off_t this_found_pos = read_position + i;

				if (have_last_pos) {
					const size_t line_length = last_found_pos - this_found_pos - bytes_length;

					if (line_length > 0) {
						char* source = NULL;

						if (last_found_pos >= read_position + (off_t)bytes_read) {
							source = new char[line_length + 1];
							memset(source, 0, line_length + 1);

							if (readblock(fd, source, line_length, this_found_pos + bytes_length) < (ssize_t)line_length) {
								wxLogWarning(_T("ChatLog::find_tail_sequences: Read-byte count less than expected"));
							}
						} else {
							source = buf + i + bytes_length;
						}
						source[line_length] = 0;
						wxString tmp = wxString::FromUTF8(source);
						out.Insert(tmp, 0);
						if (last_found_pos >= read_position + (off_t)bytes_read)
							delete[] source;

						count_added++;
						if (count_added >= count)
							break;
					}
				}
				last_found_pos = this_found_pos;
				have_last_pos = true;
				i -= bytes_length - 1;
			}
		}

		last_read_position = read_position;
	}

	return count_added;
}


void ChatLog::FillLastLineArray()
{
	m_last_lines.Clear();

	if (!m_logfile.IsOpened()) {
		wxLogError(_T("%s: failed to open log file."), __PRETTY_FUNCTION__);
		return;
	}

	if (m_logfile.Length() <= 0) {
		return;
	}

#ifdef TEST
	const size_t num_lines = 6;
#else
	const size_t num_lines = sett().GetAutoloadedChatlogLinesCount();
#endif

	m_last_lines.Alloc(num_lines);

	const wxChar* wc_EOL(wxTextBuffer::GetEOL());
	const size_t eol_num_chars(wxStrlen(wc_EOL));
#ifndef WIN32
	char* eol(static_cast<char*>(alloca(eol_num_chars)));
#else
	char* eol(new char[eol_num_chars]);
#endif
	wxConvUTF8.WC2MB(eol, wc_EOL, eol_num_chars);

	const size_t lines_added = find_tail_sequences(m_logfile, eol, eol_num_chars, num_lines, m_last_lines);
	wxLogMessage(_T("ChatLog::FillLastLineArray: Loaded %lu lines from %s."), lines_added, GetCurrentLogfilePath().c_str());

#ifdef WIN32
	delete[] eol;
#endif
}
