#ifndef __TAB_HOTKEY_TYPES_h__
#define __TAB_HOTKEY_TYPES_h__

#include <map>
#include <set>
#include <vector>

#include <wx/string.h>
#include "KeynameConverter.h"


typedef std::pair<wxString, wxString>			key_command;
typedef std::set<key_command>					key_command_set;
typedef std::vector<key_command>				key_commands_sorted;
typedef std::set<wxString>						key_set;
typedef std::map<wxString, key_set>				key_binding_c2k;
typedef std::vector<wxString>					command_list_sorted;

class key_binding
{
public:
	key_binding();

	typedef std::map<unsigned, key_command>		key_binding_map;

	void bind( const wxString& cmd, const wxString& keyString );
	void unbind( const wxString& cmd, const wxString& keyString );
	void clear();

	bool exists( const wxString& command, const wxString& key );
	bool isEmpty() const;

	const key_binding key_binding::operator-(const key_binding &other) const;

	key_commands_sorted getBinds() const;
	//const key_binding_k2c& getK2C() const;

	bool operator==( const key_binding& other ) const;

private:
	//define different "views" onto the bindings
	typedef std::map<wxString, command_list_sorted >			KeyGroupMap;
	KeyGroupMap													m_groups;
	KeyGroupMap													m_groupsAny;

	key_command_set												m_keyCmdSet;
	key_command_set												m_keyCmdSetAny;

	//key_binding_map		m_binds;
	//key_binding_c2k		m_c2k; //for faster lookups. keep this in sync with m_binds
	//unsigned			m_nextOrderIdx;
	//bool				m_isNormalized;
};
//typedef std::set<spring_key>				key_set;
//typedef std::map<wxString, key_set>			key_binding_c2k;
//typedef std::map<spring_key, command_set>	key_binding_k2c;

typedef std::map<wxString, key_binding>			key_binding_collection;

#endif
