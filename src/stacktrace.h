/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#ifdef __WXMSW__
	#include <windows.h>
	#ifdef __cplusplus
		extern "C" {
	#endif
			 LONG WINAPI DrMingwGenerateStacktrace(PEXCEPTION_POINTERS pExceptionInfo, const char* report_filename);
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
		#error "No usable stacktrace config, wxUSE_STACKWALKER is disabled"
	#endif //#if wxUSE_STACKWALKER
#endif // __WXMSW__

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
