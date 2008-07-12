#ifndef SE_UTILS_H_
#define SE_UTILS_H_

#include <wx/string.h>
/** \name SPringSettings Utils
 * these are necessary duplications for standalone mode atm
 * \todo can prolly be avoided if properly use code from SL
 * @{ */
void loadUnitsync();

int fromString(const wxString& s);

void openUrl(const wxString& url);
/** @} */

#endif /*SE_UTILS_H_*/

