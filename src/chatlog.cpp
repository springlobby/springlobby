/* Copyright (C) 2007-2010 The SpringLobby Team. All rights reserved. */
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
#include "utils/conversion.h"
#include "ui.h"
#include "defines.h"

#include "utils/customdialogs.h"
#include "utils/platform.h"
#include "utils/misc.h"


ChatLog::ChatLog( const wxString& server, const wxString& room ):
    m_server( server ),
    m_room( room ),
    m_active ( LogEnabled() ),
    m_logfile ( )
{
	m_server.Replace( wxT( ":" ), wxT( "_" ) ) ;
	wxLogMessage( _T( "ChatLog::ChatLog( %s, %s )" ), m_server.c_str(), m_room.c_str() ) ;
	m_active = OpenLogFile();
}


ChatLog::~ChatLog() {
    wxLogMessage( _T( "%s -- ChatLog::~ChatLog()" ), m_room.c_str() );
    if ( m_logfile.IsOpened() ) {
	CloseSession();
    }
}

void ChatLog::CloseSession() {
    wxDateTime now = wxDateTime::Now();
    wxString newline ( wxTextBuffer::GetEOL() );
    WriteLine( _( "### Session Closed at [" ) + now.Format( _T( "%Y-%m-%d %H:%M" ) ) + _( "]" ) );
    WriteLine(_T(" ") + newline + _T(" ") + newline + _T(" ") + newline);
    m_logfile.Flush();
    m_logfile.Close();
}

bool ChatLog::AddMessage( const wxString& text )
{
	if ( !LogEnabled() || ! m_active ) {
		return true;
	}
	else if ( !m_logfile.IsOpened() ) {
	    m_active = OpenLogFile();
	}
	if ( m_active )
	{
	    return WriteLine( LogTime() + _T( " " ) + text + wxTextBuffer::GetEOL() );
	}
	else return false;
}


bool ChatLog::CreateCurrentLogFolder()
{
    wxString path ( wxFileName(GetCurrentLogfilePath()).GetPath() );
    if ( !tryCreateDirectory( path ) ) {
        wxLogWarning( _T( "can't create logging folder: %s" ), path.c_str() );
        customMessageBox( SL_MAIN_ICON,
                  _( "Couldn't create folder. \nBe sure that there isn't a write protection.\n" ) + path
                  + _( "Log function is disabled until restart SpringLobby." ), _( "Log Warning" ) );
        m_active = false;
        return false;
    }
    return true;
}


bool ChatLog::WriteLine( const wxString& text )
{
	if ( !m_logfile.Write( text, wxConvUTF8 ) ) {
		m_active = false;
		wxLogWarning( _T( "can't write message to log (%s)" ),  wxString( m_server + _T( "::" ) + m_room ).c_str() );
		customMessageBox( SL_MAIN_ICON, _( "Couldn't write message to log.\nLogging will be disabled for room " )
				  + m_server + _T( "::" ) + m_room + _( ".\n\nRejoin room to reactivate logging." ),
				  _( "Log Warning" ) );
		return false;
	}
	return true;
}

bool ChatLog::OpenLogFile()
{
    wxLogMessage( _T( "OpenLogFile( ) server = %s, room = %s" ), m_server.c_str(), m_room.c_str() ) ;
    wxString logFilePath ( GetCurrentLogfilePath() );

    if ( LogEnabled() && CreateCurrentLogFolder() ) {
	m_logfile.Open( logFilePath, wxFile::write_append );

	if ( !m_logfile.IsOpened() ) {
	     wxLogWarning( _T( "Can't open log file %s" ), logFilePath.c_str() ) ;
	     customMessageBox( SL_MAIN_ICON,
			       _( "Can't open log file \"" ) + logFilePath + _("\".\nBe sure that there isn't a write protection.\n" ),
			       _( "Log Warning" ) ) ;
	     m_active = false;
	}
	else {
	    FillLastLineArray();

	    wxDateTime now = wxDateTime::Now();
	    wxString text = _T( "### Session Start at [" ) + now.Format( _T( "%Y-%m-%d %H:%M" ) ) + _T( "]" ) + wxTextBuffer::GetEOL();
	    return WriteLine( text );
	}
    }
    return false;
}


wxString ChatLog::LogTime()
{
	wxDateTime now = wxDateTime::Now();
	return  _T( "[" ) + now.Format( _T( "%H:%M" ) ) + _T( "]" );
}

void ChatLog::OpenInEditor()
{
    ui().OpenFileInEditor( GetCurrentLogfilePath());
}

const wxArrayString& ChatLog::GetLastLines( ) const
{
    return m_last_lines;
}


wxString ChatLog::GetCurrentLogfilePath() const {
    return sett().GetChatLogLoc()
	+ wxFileName::GetPathSeparator() + m_server
	+ wxFileName::GetPathSeparator() + m_room + _T( ".txt" );

}

bool ChatLog::LogEnabled()
{
	return sett().GetChatLogEnable();
}


#ifdef WIN32
/* MSVC apparently doesn't support POSIX.
 *
 * ...well, I guess we all saw that coming.  Quoth the GNU C Library
 * Reference Manual:
 *
 *     "The function is an extension defined in the Unix Single
 *      Specification version 2."
 *
 * So it can -- and must be -- #ifdef'd out on UNIX systems.
 */
static inline ssize_t
pread(int fd, void* buffer, size_t size, off_t offset)
{
    errno = 0;
    if ( offset < 0 )
    {
	errno = EINVAL;
	return -1;
    }
    else if ( lseek(fd, offset, SEEK_SET) != offset )
    {
	return -1;
    }
    else
    {
	return read(fd, buffer, size);
    }
}
#endif	/* WIN32 */


/** Calculate the next read position for find_tail_sequences (below) */
static inline off_t
next_read_position(off_t last_read_position, size_t read_size, size_t read_overlap)
{
    return std::max(static_cast<long signed int>(last_read_position - read_size - read_overlap), (long signed int) 0);
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
static inline size_t
find_tail_sequences(int fd, const char* bytes, size_t bytes_length, size_t count, wxArrayString& out)
{
    size_t count_added ( 0 );

    /* We overlap the file reads a little to avoid splitting (and thus missing) the
       delimiter sequence.  */
    const size_t read_overlap ( bytes_length - 1 );
    const size_t read_size ( BUFSIZ );

    const off_t log_length ( lseek(fd, 0, SEEK_END) );
    bool have_last_pos ( false );
    char buf[read_size];
    off_t last_found_pos ( 0 );
    off_t last_read_position ( log_length + read_overlap );

    /* We read `read_size'-byte blocks of the file, starting at the end and working backwards. */
    while ( count_added < count && last_read_position > 0 ) {
	off_t read_position ( next_read_position(last_read_position, read_size, read_overlap) );
	size_t bytes_read ( pread(fd, buf, std::min(static_cast<off_t>(read_size), last_read_position - read_position), read_position) );

	/* In each block, we search for `bytes', starting at the end.  */
	for ( ssize_t i = bytes_read - read_overlap - 1; i >= 0; i-- ) {
	    if ( !strncmp((buf + i), bytes, bytes_length) ) {
		off_t this_found_pos ( read_position + i );

		if ( have_last_pos && count_added < count ) {
		    size_t line_length ( last_found_pos - this_found_pos - bytes_length );

		    if ( line_length > 0 ) {
			char* source ( NULL );

			if ( last_found_pos >= read_position + (off_t) bytes_read ) {
			    source = new char[ line_length + 1];
			    memset(source, 0, line_length + 1);

				if ( pread(fd, source, line_length, this_found_pos + bytes_length) < (ssize_t) line_length ) {
					wxLogWarning(_T("ChatLog::find_tail_sequences: Read-byte count less than expected"));
				}
			} else {
			    source = buf + i + bytes_length;
			}

			if (  strncmp(source, "##", 2) != 0 ) {
			    out.Insert(wxString(L'\0', 0), 0);
			    wxLogMessage(_T("ChatLog::find_tail_sequences: fetching write buffer for %lu bytes"), sizeof(wxChar) * (line_length + 1));
				#if !defined(HAVE_WX28) || wxUSE_STL
			    	wxStringBufferLength outzero_buf(out[0], sizeof(wxChar) * (line_length + 1));
			    	wxConvUTF8.ToWChar(outzero_buf, line_length, source);
			    	outzero_buf.SetLength(line_length);
				#else
			    	wxConvUTF8.MB2WC(out[0].GetWriteBuf(sizeof(wxChar) * (line_length + 1)), source, line_length);
			    	out[0].UngetWriteBuf(line_length);
				#endif

			    ++count_added;
			}

			if ( last_found_pos >= read_position + (off_t) bytes_read )
			    delete[] source;

			if ( count_added >= count )
			    i = -1; /* short-circuit the `for' loop. */
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
	int fd ( open(GetCurrentLogfilePath().mb_str(), O_RDONLY) );
    if ( fd < 0 )
    {
	wxLogError(_T("%s: failed to open log file."), __PRETTY_FUNCTION__);
        return;
    }
    size_t num_lines ( sett().GetAutoloadedChatlogLinesCount() );

    m_last_lines.Clear();
    m_last_lines.Alloc(num_lines);

    const wxChar* wc_EOL ( wxTextBuffer::GetEOL() );
    size_t eol_num_chars ( wxStrlen(wc_EOL) );
#ifndef WIN32
    char* eol ( static_cast<char*>( alloca(eol_num_chars) ) );
#else
    char* eol ( new char[eol_num_chars] );
#endif
    wxConvUTF8.WC2MB(eol, wc_EOL, eol_num_chars);

    size_t lines_added ( find_tail_sequences(fd, eol, eol_num_chars, num_lines, m_last_lines) );
    wxLogMessage(_T("ChatLog::FillLastLineArray: Loaded %lu lines from %s."), lines_added, GetCurrentLogfilePath().c_str());
    close(fd);

#ifdef WIN32
    delete[] eol;
#endif
}

