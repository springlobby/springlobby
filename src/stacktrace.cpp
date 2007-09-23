/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: StackTrace
//

#include "stacktrace.h"
#include "boost/md5.hpp"

StackTrace& stacktrace() { static StackTrace trace; return trace; };

void StackTrace::OnStackFrame ( const wxStackFrame& frame )
{
  StackTraceString += wxString::Format( _T("(%d) "), frame.GetLevel() ); // (#frame_level_number)
  PartToHash += wxString::Format( _T("(%d) "), frame.GetLevel() );
  StackTraceString += wxString::Format( _T("%p -> %p: "), frame.GetAddress(), frame.GetOffset() ); // calling_add -> return_addr
  PartToHash += wxString::Format( _T("%p -> %p: "), frame.GetAddress(), frame.GetOffset() );

  if ( frame.HasSourceLocation() ) StackTraceString += _T("File: ") + frame.GetFileName() + wxString::Format( _T(" @Line : %d -"), frame.GetLine() ); // File: file_name @Line: line_number
  if ( frame.HasSourceLocation() ) PartToHash += _T("File: ") + frame.GetFileName() + wxString::Format( _T(" @Line : %d -"), frame.GetLine() );

  StackTraceString += _T("Function: ") +  frame.GetName(); // Function: function_name
  PartToHash += _T("Function: ") +  frame.GetName();


  int paramcount = frame.GetParamCount();
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

  StackTraceString += _T(" )\n");

}

wxString StackTrace::GetStackTraceHash()
{
  wxString hash;
  return hash;
}
