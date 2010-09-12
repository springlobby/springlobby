#ifndef __HOTKEYEXCEPTION_h__
#define __HOTKEYEXCEPTION_h__

#include <stdexcept>

#include <wx/string.h>


class HotkeyException : private std::exception
{
public:
	HotkeyException( const wxString& message );

	const wxString& getMessage() const;

private:
	const wxString		m_message;
};

#endif