#include "HotkeyException.h"

HotkeyException::HotkeyException( const wxString& message ) : m_message( message )
{
}

const wxString& HotkeyException::getMessage() const
{
	return this->m_message;
}
