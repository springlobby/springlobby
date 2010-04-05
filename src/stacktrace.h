#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#if defined(ENABLE_DEBUG_REPORT)

#if wxUSE_STACKWALKER
#include <wx/string.h>
#include <wx/stackwalk.h>


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
//		namespace StackTrace {
			#include <windows.h>
//			static std::string report_filename;
//			long Stacktrace(LPEXCEPTION_POINTERS e);
		#ifdef __cplusplus
			extern "C" {
		#endif
				 LONG WINAPI TopLevelExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo, const char* report_filename);
		#ifdef __cplusplus
			}
		#endif
	#else
		#error "No usable stacktrace config, set ENABLE_DEBUG_REPORT=OFF in cmake"
    #endif
#endif //#if wxUSE_STACKWALKER

#endif //#if defined(ENABLE_DEBUG_REPORT)

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
