/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: StackTrace
//

#include "stacktrace.h"

#if defined(ENABLE_DEBUG_REPORT)

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

namespace StackTrace {
}//end namespace StackTrace
#endif //__MSW

#endif //#if defined(ENABLE_DEBUG_REPORT)
#endif //useStacktrace
