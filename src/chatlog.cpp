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

#include "chatlog.h"
#include "settings.h"


ChatLog::ChatLog(const wxString& server,const wxString& room)
{
  m_active = false;
  wxString path = _GetPath()+_T("/")+server+_T("/")+room+_T(".txt");
  if (!(!LogEnabled() or !_CreateFolder(server))) {
    if (wxFileExists(path)) {
      m_logfile = new wxFile(path,wxFile::write_append);
    } else {
      m_logfile = new wxFile(path,wxFile::write);
    }

    if (!m_logfile->IsOpened()) {
      wxMessageBox(_T("Can't open log file. \nBe sure that there isn't a write protection.\n")+path, _T("Warning")) ;
    } else {
      m_active = true;
    }

    wxDateTime now = wxDateTime::Now();
    AddMessage(_T("### Session Start at [") + now.Format( _T("%Y-%m-%d %H:%M") ) + _T("]"));
  }
}


ChatLog::~ChatLog()
{
  if (m_active and m_logfile)
  {
    wxDateTime now = wxDateTime::Now();
    AddMessage(_("### Session Closed at [") + now.Format( _("%Y-%m-%d %H:%M") ) + _("]"));
    AddMessage(_(" \n \n \n"));
    if (m_logfile->IsOpened()) {m_logfile->Close();}
    delete m_logfile;
  }
}


bool ChatLog::AddMessage(const wxString& text)
{
  return (m_active and m_logfile and m_logfile->IsOpened())?_WriteLine(text):false;
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
  if (!(wxDirExists(_GetPath()) or wxMkdir(_GetPath(),0777))) {
    wxMessageBox(_T("Couldn't create folder. \nBe sure that there isn't a write protection.\n")+_GetPath(), _T("Warning")) ;
    return false;
  }
  if (!(wxDirExists(_GetPath()+_("/")+server) or wxMkdir(_GetPath()+_("/")+server,0777) )) {
    wxMessageBox(_T("Couldn't create folder. \nBe sure that there isn't a write protection.\n")+_GetPath()+_("/")+server, _T("Warning")) ;
    return false;
  }
  return true;
}


bool ChatLog::_WriteLine(const wxString& text)
{
  return m_logfile->Write(LogTime()+_(" ")+text+_("\n"),wxConvUTF8);
}

wxString ChatLog::LogTime()
{
  wxDateTime now = wxDateTime::Now();
  return  _("[") + now.Format( _("%H:%M") ) + _("]");
}

