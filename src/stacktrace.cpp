/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
//
// Class: StackTrace
//
#include "stacktrace.h"

#if wxUSE_STACKWALKER
#include "utils/conversion.h"

void StackTrace::OnStackFrame(const wxStackFrame& frame)
{
	//  StackTraceString += wxString::Format( _T("(%d) "), frame.GetLevel() ); // (frame_level_number)
	//  PartToHash += wxString::Format( _T("(%d) "), frame.GetLevel() );
	StackTraceString += wxString::Format(_T("%p "), frame.GetAddress()); // [calling_address]


	StackTraceString += frame.GetName(); // function_name
					     //    PartToHash += frame.GetName() + _T("\n");

	if (frame.HasSourceLocation()) {
		int paramcount = frame.GetParamCount();
		if (paramcount == 0) {
			StackTraceString += _T(" ");
		} else {
			StackTraceString += _T("(");
			while (paramcount > 0) {
				wxString type;
				wxString name;
				wxString value;

				if (frame.GetParam(paramcount, &type, &name, &value)) // data_type var_name = value,
				{
					StackTraceString += _T(" ") + type;
					StackTraceString += _T(" ") + name;
					StackTraceString += _T(" = ") + value;
				}

				paramcount--;
				if (paramcount > 0)
					StackTraceString += _T(",");
			}
		}
		StackTraceString += _T(" ") + frame.GetFileName() + wxString::Format(_T(":%ld"), frame.GetLine()); // File: file_name : line_number
	}
	StackTraceString += _T("\n");
}

#endif
