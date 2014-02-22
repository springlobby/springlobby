/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/spring/springprocess.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/



#include <wx/thread.h>
#include <wx/string.h>
typedef wxThread SpringProcessBase;

#include <wx/process.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_LOCAL_EVENT_TYPE( wxEVT_SPRING_EXIT, 1 )
END_DECLARE_EVENT_TYPES()

class Spring;

class SpringProcess: public SpringProcessBase
{
  public:
    SpringProcess( Spring& sp );
    ~SpringProcess();

    void OnExit();

    void SetCommand( const wxString& cmd );

    void* Entry();

  protected:
    Spring& m_sp;
    wxString m_cmd;
    int m_exit_code;
};

class wxSpringProcess: public wxProcess
{
  public:
    wxSpringProcess( Spring& sp );
    ~wxSpringProcess();

    void OnTerminate( int pid, int status );

  protected:
    Spring& m_sp;
};

const int PROC_SPRING = wxID_HIGHEST;

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
