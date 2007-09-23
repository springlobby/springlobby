/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: StackTrace
//

#include "stacktrace.h"


void StackTrace::OnStackFrame ( const wxStackFrame& frame )
{
  StackFrame = wxString::Format( _T("(#%d) "), frame.GetLevel() ); // (#frame_level_number)
  StackFrame += wxString::Format( _T("% -> %: "), frame.GetAddress(), frame.GetOffset() ); // calling_add -> return_addr

  if ( frame.HasSourceLocation() ) StackFrame += _T("File: ") + frame.GetFileName() + wxString::Format( _T(" @Line : %d -"), frame.GetLine() ); // File: file_name @Line: line_number

  StackFrame += _T("Function: ") +  frame.GetName(); // Function: function_name


  int paramcount = frame.GetParamCount();
  StackFrame += _T("(");
  while ( paramcount > 0 )
  {
    wxString type;
    wxString name;
    wxString value;

    if ( frame.GetParam( paramcount, &type, &name, &value) ) // data_type var_name = value,
    {
      StackFrame += _T(" ") + type;
      StackFrame += _T(" ") + name;
      StackFrame += _T(" = ") + value;
    }

    paramcount--;
    if ( paramcount > 0 ) StackFrame += _T(",");
  }

  StackFrame += _T(" )\n");

}
