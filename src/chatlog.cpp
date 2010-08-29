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

#include "utils/customdialogs.h"


ChatLog::ChatLog( const wxString& server, const wxString& room ):
    m_server( server ),
    m_room( room ),
    m_active ( LogEnabled() ),
    m_logfile ( )
{
#ifdef __WXMSW__
	m_server.Replace( wxT( ":" ), wxT( "_" ) ) ;
#endif
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
    if ( !( wxDirExists(path) || wxMkdir(path, 0777) ) ) {
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

void ChatLog::FillLastLineArray()
{
    wxTextFile tmp( GetCurrentLogfilePath() );
    tmp.Open();
    if ( !tmp.IsOpened() )
        return;

    m_last_lines = wxArrayString();
    int load_lines  = sett().GetAutoloadedChatlogLinesCount();
    const size_t line_no = tmp.GetLineCount();
    const size_t first_line_no = line_no - load_lines ;

    for ( size_t i = first_line_no; i < line_no; ++i ) {
        wxString tmp_str = tmp[i];
        if ( ! tmp_str.StartsWith( _T("###") ) )
            m_last_lines.Add( tmp_str );
    }
}

bool ChatLog::OpenLogFile()
{
    FillLastLineArray();
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
