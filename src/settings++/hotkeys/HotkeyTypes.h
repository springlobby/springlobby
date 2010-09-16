#ifndef __TAB_HOTKEY_TYPES_h__
#define __TAB_HOTKEY_TYPES_h__

#include <map>
#include <set>

#include <wx/string.h>

struct spring_key
{
	spring_key(wxString key) : keyString(key), orderIdx(1) {};
	spring_key(wxString key, unsigned order ) : keyString(key), orderIdx(order) {};

	bool operator<( const spring_key& other ) const
	{
		return this->keyString < other.keyString;
	}

	wxString keyString;
	unsigned orderIdx;
};

typedef std::set<wxString>					command_set;
typedef std::set<spring_key>				key_set;
typedef std::map<wxString, key_set>			key_binding_c2k;
typedef std::map<spring_key, command_set>	key_binding_k2c;

typedef std::map<wxString, key_binding_c2k>	key_binding_collection;

#endif
