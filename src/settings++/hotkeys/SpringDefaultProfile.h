#ifndef __SPRING_DEFAULT_PROFILE_h__
#define __SPRING_DEFAULT_PROFILE_h__

#include <map>
#include <vector>

#include <wx/string.h>

#include "hotkey_parser.h"


class SpringDefaultProfile : public key_binding
{
public:
	static void initialize();
	static const key_binding& getBindings();
	
private:
	SpringDefaultProfile();

	static void addBinding( const wxString& key, const wxString& command );

	static key_binding			m_bindings;
};

#endif
