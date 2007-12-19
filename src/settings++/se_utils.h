#ifndef SE_UTILS_H_
#define SE_UTILS_H_

#include <wx/string.h>

#define _S(v) wxString(v.c_str(),wxConvUTF8)

void loadUnitsync();

void toString(std::string& s, int t);

int fromString(const std::string& s);

void openUrl(const wxString& url);


#endif /*SE_UTILS_H_*/

