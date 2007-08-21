#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H

#include <wx/thread.h>
#include <wx/string.h>
#include <wx/event.h>

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE( wxEVT_SPRING_EXIT, -1 )
END_DECLARE_EVENT_TYPES()

class Spring;

class SpringProcess: public wxThread
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
};

const int PROC_SPRING = wxID_HIGHEST;

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGPROCESS_H
