/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "exception.h"

#include <wx/log.h>

Exception::Exception()
{
	// TODO Auto-generated constructor stub
}

Exception::Exception(const wxString& reason)
{
	this->reason = reason;
	wxLogWarning(reason);
}

Exception::~Exception()
{
	// TODO Auto-generated destructor stub
}

wxString Exception::Reason() const
{
	return reason;
}
