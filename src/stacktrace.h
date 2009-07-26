#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#include <wx/string.h>
#include <wx/stackwalk.h>

#if wxUSE_STACKWALKER

class StackTrace : public wxStackWalker
{
  public:
    StackTrace() {}
    ~StackTrace() {}

    wxString GetStackTrace() { return StackTraceString; }

  private:

    void OnStackFrame( const wxStackFrame& frame );

    wxString StackTraceString;
    wxString PartToHash;

};
#else
    #ifdef __WXMSW__
    #include <windows.h>
#include <process.h>
#include <imagehlp.h>
#include <signal.h>
    class wxArrayString;
    /** Print out a stacktrace. */
    wxArrayString Stacktrace(LPEXCEPTION_POINTERS e);
    #endif
#endif

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
