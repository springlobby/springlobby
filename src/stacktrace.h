/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef STACKTRACE_H_INCLUDED
#define STACKTRACE_H_INCLUDED

#include <wx/string.h>
#include <wx/stackwalk.h>

#if wxUSE_STACKWALKER

class StackTrace : public wxStackWalker
{
public:
	StackTrace()
	{
	}
	~StackTrace()
	{
	}
	wxString GetStackTrace() const
	{
		return StackTraceString;
	}

private:
	void OnStackFrame(const wxStackFrame& frame);
	wxString StackTraceString;
	wxString PartToHash;
};
#else
class StackTrace
{
public:
	wxString GetStackTrace() const
	{
		return wxEmptyString;
	}
};

#endif

#endif // SPRINGLOBBY_HEADERGUARD_STACKTRACE_H_INCLUDED
