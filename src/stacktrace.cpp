/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: StackTrace
//


#include "stacktrace.h"


#if wxUSE_STACKWALKER


void StackTrace::OnStackFrame ( const wxStackFrame& frame )
{
  StackTraceString += wxString::Format( _T("(%d) "), frame.GetLevel() ); // (frame_level_number)
  PartToHash += wxString::Format( _T("(%d) "), frame.GetLevel() );
  StackTraceString += wxString::Format( _T("[%p] "), frame.GetAddress() ); // [calling_address]


    StackTraceString += frame.GetName();  // function_name
    PartToHash += frame.GetName() + _T("\n");

  if ( frame.HasSourceLocation() )
  {
    int paramcount = frame.GetParamCount();
    if ( paramcount == 0 ){
      StackTraceString += _T("\n");
    } else {
      StackTraceString += _T("(");
      while ( paramcount > 0 )
      {
        wxString type;
        wxString name;
        wxString value;

        if ( frame.GetParam( paramcount, &type, &name, &value) ) // data_type var_name = value,
        {
          StackTraceString += _T(" ") + type;
          StackTraceString += _T(" ") + name;
          StackTraceString += _T(" = ") + value;
        }

        paramcount--;
        if ( paramcount > 0 ) StackTraceString += _T(",");
      }
    }
    StackTraceString += _T("->") + frame.GetFileName() + wxString::Format( _T(":%d"), frame.GetLine() ); // File: file_name : line_number

  }
  StackTraceString += _T("\n");

}

#else
#ifdef __WXMSW__
/** Convert exception code to human readable string. */
static const char *ExceptionName(DWORD exceptionCode)
{
	switch (exceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:         return "Access violation";
		case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Datatype misalignment";
		case EXCEPTION_BREAKPOINT:               return "Breakpoint";
		case EXCEPTION_SINGLE_STEP:              return "Single step";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array bounds exceeded";
		case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Float denormal operand";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Float divide by zero";
		case EXCEPTION_FLT_INEXACT_RESULT:       return "Float inexact result";
		case EXCEPTION_FLT_INVALID_OPERATION:    return "Float invalid operation";
		case EXCEPTION_FLT_OVERFLOW:             return "Float overflow";
		case EXCEPTION_FLT_STACK_CHECK:          return "Float stack check";
		case EXCEPTION_FLT_UNDERFLOW:            return "Float underflow";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer divide by zero";
		case EXCEPTION_INT_OVERFLOW:             return "Integer overflow";
		case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged instruction";
		case EXCEPTION_IN_PAGE_ERROR:            return "In page error";
		case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal instruction";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception";
		case EXCEPTION_STACK_OVERFLOW:           return "Stack overflow";
		case EXCEPTION_INVALID_DISPOSITION:      return "Invalid disposition";
		case EXCEPTION_GUARD_PAGE:               return "Guard page";
		case EXCEPTION_INVALID_HANDLE:           return "Invalid handle";
	}
	return "Unknown exception";
}


#include <wx/arrstr.h>
/** Print out a stacktrace. */
wxArrayString Stacktrace(LPEXCEPTION_POINTERS e) {
	PIMAGEHLP_SYMBOL pSym;
	STACKFRAME sf;
	HANDLE process, thread;
	DWORD dwModBase, Disp;
	BOOL more = FALSE;
	int count = 0;
	char modname[MAX_PATH] = "FUCKSHIT"; //got compile errors below
    wxArrayString out;

	pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 16384);

	ZeroMemory(&sf, sizeof(sf));
	sf.AddrPC.Offset = e->ContextRecord->Eip;
	sf.AddrStack.Offset = e->ContextRecord->Esp;
	sf.AddrFrame.Offset = e->ContextRecord->Ebp;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	process = GetCurrentProcess();
	thread = GetCurrentThread();

	while(1) {
		more = StackWalk(
			IMAGE_FILE_MACHINE_I386, // TODO: fix this for 64 bit windows?
			process,
			thread,
			&sf,
			e->ContextRecord,
			NULL,
			SymFunctionTableAccess,
			SymGetModuleBase,
			NULL
		);
		if(!more || sf.AddrFrame.Offset == 0) {
			break;
		}

		dwModBase = SymGetModuleBase(process, sf.AddrPC.Offset);
//
//		if(dwModBase) {
//			GetModuleFileName((HINSTANCE)dwModBase, modname, MAX_PATH);
//		} else {
//			strcpy(modname, "Unknown");
//		}

		pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		pSym->MaxNameLength = MAX_PATH;

		if(SymGetSymFromAddr(process, sf.AddrPC.Offset, &Disp, pSym)) {
			// This is the code path taken on VC if debugging syms are found.
			out.Add( wxString::Format( _T("(%d) %s(%s+%#0x) [0x%08X]\n") , count, modname, pSym->Name, Disp, sf.AddrPC.Offset) );
		} else {
			// This is the code path taken on MinGW, and VC if no debugging syms are found.
			out.Add( wxString::Format( _T("(%d) %s [0x%08X]\n"), count, modname, sf.AddrPC.Offset) );
		}
		++count;
	}
	GlobalFree(pSym);

	return out;
}

#endif //__MSW
#endif //useStacktrace
