#ifndef __HOTKEYEXCEPTION_h__
#define __HOTKEYEXCEPTION_h__

#include <stdexcept>

#include <wx/string.h>
#include "../../utils/conversion.h"

class HotkeyException : public std::runtime_error
{
public:
	HotkeyException( const wxString& message ): std::runtime_error( STD_STRING(message) ) {}
};

#endif
