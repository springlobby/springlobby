/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SE_UTILS_H_
#define SE_UTILS_H_

#include <wx/string.h>
#include <sstream>
/** \name SPringSettings Utils
 * these are necessary duplications for standalone mode atm
 * \todo can prolly be avoided if properly use code from SL
 * @{ */

int fromString(const wxString& s);

template <class T>
wxString towxString(T arg)
{
	std::stringstream s;
	s << arg;
	return wxString(s.str().c_str(), wxConvUTF8);
}
/** @} */

bool IsSettingsStandAlone();
void SetSettingsStandAlone(bool value);

#endif /*SE_UTILS_H_*/
