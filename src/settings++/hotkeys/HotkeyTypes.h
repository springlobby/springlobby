#ifndef __TAB_HOTKEY_TYPES_h__
#define __TAB_HOTKEY_TYPES_h__

#include <map>
#include <vector>
#include <set>

#include <wx/string.h>


typedef std::set<wxString>				key_set;
typedef std::map<wxString, key_set>		key_binding;
typedef std::map<wxString, key_binding>	key_binding_collection;

#endif
