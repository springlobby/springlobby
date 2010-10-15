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
typedef std::map<wxString, wxString>			key_sym_map;
typedef std::map<wxString, wxString>			key_sym_set_map;

class key_binding
{
public:
	key_binding();

	typedef std::map<unsigned, key_command>		key_binding_map;

	void bind( const wxString& cmd, const wxString& keyString );
	void unbind( const wxString& cmd, const wxString& keyString );
	void clear();

	void addKeySym( const wxString& name, const wxString& keyString );
	void addKeySymSet( const wxString& name, const wxString& keyString );

	bool exists( const wxString& command, const wxString& key );
	bool isEmpty() const;

	const key_binding key_binding::operator-(const key_binding &other) const;


	void setKeySymsSet( const key_sym_set_map& keySyms );
	void setKeySyms( const key_sym_map& keySyms );
	const key_sym_map& getKeySyms() const;
	const key_sym_map& getKeySymsSet() const;

	key_commands_sorted getBinds() const;
	//const key_binding_k2c& getK2C() const;

	wxString resolveKeySymKeyAndSet( const wxString& key ) const;
	wxString resolveKeySymKey( const wxString& key ) const;
	bool operator==( const key_binding& other ) const;

private:
	const wxString resolveKeySymSetName( const wxString& symName ) const;
	const wxString resolveKeySymSetKey( const wxString& key ) const;
	const wxString resolveKeySymName( const wxString& symName ) const;

	//define different "views" onto the bindings
	typedef std::map<wxString, command_list_sorted >			KeyGroupMap;
	KeyGroupMap													m_groups;
	KeyGroupMap													m_groupsAny;

	key_command_set												m_keyCmdSet;
	key_command_set												m_keyCmdSetAny;

	key_sym_map													m_keySyms;
	key_sym_map													m_keySymsRev;

	key_sym_set_map												m_keySymsSet;
	key_sym_set_map												m_keySymsSetRev;
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
