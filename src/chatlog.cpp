/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
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
#include <stdexcept>

#include "chatlog.h"
#include "settings.h"
#include "utils.h"

#include "settings++/custom_dialogs.h"

bool ChatLog::m_parent_dir_exists = true;

ChatLog::ChatLog(const wxString& server,const wxString& room):
  m_logfile( 0 ),
  m_server( server ),
  m_room( room ),
  m_active( OpenLogFile(server,room) )
{

}


ChatLog::~ChatLog()
{
  if ( m_active && m_logfile != 0 && m_logfile->IsOpened() ) {
    wxDateTime now = wxDateTime::Now();
    WriteLine( _("### Session Closed at [") + now.Format( _T("%Y-%m-%d %H:%M") ) + _("]") );
    WriteLine( _T(" \n \n \n") );
    if ( m_logfile->IsOpened() ) m_logfile->Close();
    m_logfile = 0;
  }
  if (m_logfile != 0)
  {
  	delete m_logfile;
	m_logfile = 0;
  }
}


bool ChatLog::AddMessage(const wxString& text)
{
  if ( !LogEnabled() ) {
    return true;
  }
  else if ( !m_logfile) {
    m_active = OpenLogFile(m_server,m_room);
  }
  return (m_active)? WriteLine(LogTime()+_T(" ")+text+_T("\n")) : false;
}


bool ChatLog::LogEnabled()
{
  return sett().GetChatLogEnable();
}


wxString ChatLog::_GetPath()
{
  return sett().GetChatLogLoc();
}


bool ChatLog::CreateFolder(const wxString& server)
{
  if ( !( wxDirExists( _GetPath() ) || wxMkdir( _GetPath(), 0777) ) ) {
    wxLogWarning( _T("can't create logging folder: %s"), _GetPath().c_str() );
    customMessageBox(SL_MAIN_ICON, _("Couldn't create folder. \nBe sure that there isn't a write protection.\n") + _GetPath()+ _("Log function is disabled until restart SpringLobby."), _("Log Warning") );
    m_parent_dir_exists = false;
    return false;
  }
  if ( !( wxDirExists( _GetPath() + wxFileName::GetPathSeparator() + server ) || wxMkdir( _GetPath() + wxFileName::GetPathSeparator() + server, 0777 ) ) ) {
    wxLogWarning( _T("can't create logging folder: %s"), wxString(_GetPath() + wxFileName::GetPathSeparator() + server).c_str() );
    customMessageBox(SL_MAIN_ICON, _("Couldn't create folder. \nBe sure that there isn't a write protection.\n") + _GetPath() + wxFileName::GetPathSeparator() + server + _("Log function is disabled until restart SpringLobby."), _T("Log Warning") );
    m_parent_dir_exists = false;
    return false;
  }
  return true;
}


bool ChatLog::WriteLine(const wxString& text)
{
  try
  {
    ASSERT_LOGIC( m_logfile, _T("m_logfile = 0") );
  } catch(...) {return false;}
  if ( !m_logfile->Write( text, wxConvUTF8 ) ) {
    m_active = false;
    wxLogWarning( _T("can't write message to log (%s)"),  wxString(m_server + _T("::") + m_room).c_str() );
    customMessageBox(SL_MAIN_ICON, _("Couldn't write message to log.\nLogging will be disabled for room ") + m_server + _T("::") + m_room + _(".\n\nRejoin room to reactivate logging."), _("Log Warning") );
    return false;
  }
  return true;
}

bool ChatLog::OpenLogFile(const wxString& server,const wxString& room)
{
  wxString path = _GetPath() + wxFileName::GetPathSeparator() + server + wxFileName::GetPathSeparator() + room + _T(".txt");
  if ( m_parent_dir_exists && LogEnabled() && CreateFolder(server) ) {
    if ( wxFileExists( path ) ) {
      m_logfile = new wxFile( path, wxFile::write_append );
    } else {
      m_logfile = new wxFile( path, wxFile::write );
    }
    if ( !m_logfile->IsOpened() ) {
      wxLogWarning( _T("Can't open log file %s"), path.c_str() ) ;
      customMessageBox(SL_MAIN_ICON, _("Can't open log file. \nBe sure that there isn't a write protection.\n") + path, _("Log Warning") ) ;
      delete m_logfile;
      m_logfile = 0;
    }
    else {
      wxDateTime now = wxDateTime::Now();
      WriteLine( _T("### Session Start at [") + now.Format( _T("%Y-%m-%d %H:%M") ) + _T("]\n") );
      return true;
    }
  }
  return false;
}


wxString ChatLog::LogTime()
{
  wxDateTime now = wxDateTime::Now();
  return  _T("[") + now.Format( _T("%H:%M") ) + _T("]");
}

