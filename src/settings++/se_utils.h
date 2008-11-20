#ifndef SE_UTILS_H_
#define SE_UTILS_H_

#include <wx/string.h>
#include <sstream>
/** \name SPringSettings Utils
 * these are necessary duplications for standalone mode atm
 * \todo can prolly be avoided if properly use code from SL
 * @{ */
void loadUnitsync();

int fromString(const wxString& s);

void openUrl(const wxString& url);

template<class T>
wxString towxString(T arg){
  std::stringstream s;
  s << arg;
  return wxString(s.str().c_str(),wxConvUTF8);
}
/** @} */

bool IsSettingsStandAlone();
void SetSettingsStandAlone( bool value );

#endif /*SE_UTILS_H_*/

