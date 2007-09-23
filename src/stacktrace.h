#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#include <wx/string.h>
#include <wx/stackwalk.h>

class StackTrace : public wxStackWalker
{
  public:
    StackTrace() {}
    ~StackTrace() {}

  private:

    void OnStackFrame( const wxStackFrame& frame );

    wxString StackFrame;

};

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
