#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#if defined(ENABLE_DEBUG_REPORT)

#ifdef __WXMSW__
	#include <windows.h>
	#ifdef __cplusplus
		extern "C" {
	#endif
			 LONG WINAPI TopLevelExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo, const char* report_filename);
	#ifdef __cplusplus
		}
	#endif
#else

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
		#error "No usable stacktrace config, set ENABLE_DEBUG_REPORT=OFF in cmake"
	#endif //#if wxUSE_STACKWALKER
#endif // __WXMSW__

#endif //#if defined(ENABLE_DEBUG_REPORT)

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
