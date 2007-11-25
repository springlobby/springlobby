/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: ChatLog
//
#include <wx/string.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/dir.h>
#include <wx/datetime.h>
#include <wx/msgdlg.h>
#include <wx/intl.h>
#include <wx/filename.h>
#include <stdexcept>

#include "chatlog.h"
#include "settings.h"
#include "utils.h"

bool ChatLog::m_parent_dir_exists = true;

ChatLog::ChatLog(const wxString& server,const wxString& room):
  m_logfile( 0 ),
  m_server( server ),
  m_room( room ),
  m_active( _OpenLogFile(server,room) )
{
}


ChatLog::~ChatLog()
{
  if ( m_active && m_logfile ) {
    wxDateTime now = wxDateTime::Now();
    _WriteLine( _("### Session Closed at [") + now.Format( _("%Y-%m-%d %H:%M") ) + _("]") );
    _WriteLine( _(" \n \n \n") );
    if ( m_logfile->IsOpened() ) m_logfile->Close();
  }
  delete m_logfile;
}


bool ChatLog::AddMessage(const wxString& text)
{
  if ( !LogEnabled() ) {
    return true;
  }
  else if ( !m_logfile) {
    m_active = _OpenLogFile(m_server,m_room);
  }
  return (m_active)? _WriteLine(LogTime()+_(" ")+text+_("\n")) : false;
}


bool ChatLog::LogEnabled()
{
  return sett().GetChatLogEnable();
}


wxString ChatLog::_GetPath()
{
  return sett().GetChatLogLoc();
}


bool ChatLog::_CreateFolder(const wxString& server)
{
  if ( !( wxDirExists( _GetPath() ) || wxMkdir( _GetPath(), 0777) ) ) {
    wxMessageBox( _T("Couldn't create folder. \nBe sure that there isn't a write protection.\n") + _GetPath()+ _T("Log function is disabled until restart SpringLobby."), _T("Warning") );
    m_parent_dir_exists = false;
    return false;
  }
  if ( !( wxDirExists( _GetPath() + wxFileName::GetPathSeparator() + server ) || wxMkdir( _GetPath() + wxFileName::GetPathSeparator() + server, 0777 ) ) ) {
    wxMessageBox( _T("Couldn't create folder. \nBe sure that there isn't a write protection.\n") + _GetPath() + wxFileName::GetPathSeparator() + server + _T("Log function is disabled until restart SpringLobby."), _T("Warning") );
    m_parent_dir_exists = false;
    return false;
  }
  return true;
}


bool ChatLog::_WriteLine(const wxString& text)
{
  ASSERT_LOGIC( m_logfile, "m_logfile = 0" );
  if ( !m_logfile->Write( text, wxConvUTF8 ) ) {
    m_active = false;
    wxMessageBox( _T("Couldn't write message to log.\nLogging will be disabled for room ") + m_server + _T("::") + m_room + _T(".\n\nRejoin room to reactivate logging."), _T("Warning") );
    return false;
  }
  return true;
}

bool ChatLog::_OpenLogFile(const wxString& server,const wxString& room)
{
  wxString path = _GetPath() + wxFileName::GetPathSeparator() + server + wxFileName::GetPathSeparator() + room + _T(".txt");
  if ( m_parent_dir_exists && LogEnabled() && _CreateFolder(server) ) {
    if ( wxFileExists( path ) ) {
      m_logfile = new wxFile( path, wxFile::write_append );
    } else {
      m_logfile = new wxFile( path, wxFile::write );
    }
    if ( !m_logfile->IsOpened() ) {
      wxMessageBox( _T("Can't open log file. \nBe sure that there isn't a write protection.\n") + path, _T("Warning") ) ;
      delete m_logfile;
      m_logfile = 0;
    }
    else {
      wxDateTime now = wxDateTime::Now();
      _WriteLine( _T("### Session Start at [") + now.Format( _T("%Y-%m-%d %H:%M") ) + _T("]\n") );
      return true;
    }
  }
  return false;
}


wxString ChatLog::LogTime()
{
  wxDateTime now = wxDateTime::Now();
  return  _("[") + now.Format( _("%H:%M") ) + _("]");
}

