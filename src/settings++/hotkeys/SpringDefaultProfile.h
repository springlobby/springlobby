#ifndef __SPRING_DEFAULT_PROFILE_h__
#define __SPRING_DEFAULT_PROFILE_h__

#include <map>
#include <vector>

#include <wx/string.h>

#include "hotkey_parser.h"


class SpringDefaultProfile
{
public:
	static void initialize();
	static const key_binding_k2c& getAllBindingsK2C();
	static const key_binding_c2k& getAllBindingsC2K();

private:
	SpringDefaultProfile();

	static void addBinding( const wxString& key, const wxString& command );

	static key_binding_k2c			m_mappingK2C;
	static key_binding_c2k			m_mappingC2K;
};

#endif