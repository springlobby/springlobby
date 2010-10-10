#ifndef __TAB_HOTKEY_TYPES_h__
#define __TAB_HOTKEY_TYPES_h__

#include <map>
#include <set>

#include <wx/string.h>
#include "KeynameConverter.h"


typedef std::pair<wxString, wxString>			key_command;
typedef std::set<wxString>						key_set;
typedef std::map<wxString, key_set>				key_binding_c2k;

class key_binding
{
public:
	key_binding();

	typedef std::map<unsigned, key_command>		key_binding_map;
	//typedef std::map<wxString, command_set>	key_binding_k2c;

	void bind( const wxString& cmd, const wxString& keyString, unsigned keyOrder );
	void bind( const wxString& cmd, const wxString& keyString );
	//void bind( const command_set& cmds, const wxString& keyString );

	void unbind( const wxString& cmd, const wxString& keyString );

	bool exists( const wxString& command, const wxString& key );

	void clear();

	const key_binding key_binding::operator-(const key_binding &other) const;

	const key_binding_map& getBinds() const;
	//const key_binding_k2c& getK2C() const;

	bool operator==( const key_binding& other ) const;

private:
	key_binding_map		m_binds;
	key_binding_c2k		m_c2k; //for faster lookups. keep this in sync with m_binds
	unsigned			m_nextOrderIdx;
};
//typedef std::set<spring_key>				key_set;
//typedef std::map<wxString, key_set>			key_binding_c2k;
//typedef std::map<spring_key, command_set>	key_binding_k2c;

typedef std::map<wxString, key_binding>			key_binding_collection;

#endif
