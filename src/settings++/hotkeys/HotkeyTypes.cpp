#include <algorithm>

#include "HotkeyTypes.h"
#include "HotkeyException.h"


key_binding::key_binding() //: m_nextOrderIdx(1)
{
}

void key_binding::bind( const wxString& cmd, const wxString& keyString )
{
	if ( this->exists( cmd, keyString ) )
	{
		return;
	}

	wxString normKey = resolveKeySymName( KeynameConverter::normalizeSpringKey( keyString ) );

	if ( normKey.StartsWith( wxT("Any+") ) )
	{
		m_groupsAny[ normKey ].push_back( cmd );
		m_keyCmdSetAny.insert( std::make_pair( normKey, cmd ) );
	}
	else
	{
		m_groups[ normKey ].push_back( cmd );
		m_keyCmdSet.insert( std::make_pair( normKey, cmd ) );
	}
}

void key_binding::addKeySym( const wxString& name, const wxString& keyString )
{
	this->m_keySyms[name] = keyString;
	this->m_keySymsRev[KeynameConverter::convertHexValueToKey( keyString )] = name;
}

const wxString key_binding::resolveKeySymName( const wxString& symName ) const
{
	key_sym_map::const_iterator iter = this->m_keySyms.find( symName );
	if ( iter == this->m_keySyms.end() )
	{
		return symName;
	}

	wxString key = KeynameConverter::convertHexValueToKey( iter->second );
	return key;
}

const wxString& key_binding::resolveKeySymKey( const wxString& key ) const
{
	key_sym_map::const_iterator iter = this->m_keySymsRev.find( key );
	if ( iter == this->m_keySymsRev.end() )
	{
		return key;
	}

	return iter->second;
}

void key_binding::setKeySyms( const key_sym_map& keySyms )
{
	this->m_keySyms = keySyms;
	
	//update reverse map
	for( key_sym_map::const_iterator iter = keySyms.begin(); iter != keySyms.end(); ++iter )
	{
		this->m_keySymsRev[KeynameConverter::convertHexValueToKey(iter->second)] = iter->first;
	}
}

const key_sym_map& key_binding::getKeySyms() const
{
	return this->m_keySyms;
}

key_commands_sorted key_binding::getBinds() const
{
	key_commands_sorted sortKeys;

	//any keys
	for( KeyGroupMap::const_iterator iter = m_groupsAny.begin(); iter != m_groupsAny.end(); ++iter )
	{
		for( size_t idx = 0; idx < iter->second.size(); ++idx )
		{
			sortKeys.push_back( key_command( iter->first, iter->second[ idx ] ) );
		}
	}

	//normal keys
	for( KeyGroupMap::const_iterator iter = m_groups.begin(); iter != m_groups.end(); ++iter )
	{
		for( size_t idx = 0; idx < iter->second.size(); ++idx )
		{
			sortKeys.push_back( key_command( iter->first, iter->second[ idx ] ) );
		}
	}

	return sortKeys;
}

bool key_binding::isEmpty() const
{
	if ( m_keyCmdSet.empty() && m_keyCmdSetAny.empty() )
	{
		return true;
	}
	return false;
}

void key_binding::unbind( const wxString& cmd, const wxString& keyString )
{
	if ( !this->exists( cmd, keyString ) )
	{
		return;
	}

	const wxString normKey = KeynameConverter::normalizeSpringKey( keyString );	
	if ( normKey.StartsWith( wxT("Any+") ) )
	{
		m_keyCmdSetAny.erase( std::make_pair( normKey, cmd ) );

		key_binding::KeyGroupMap::iterator iter = m_groupsAny.find( normKey );
		assert( iter != m_groupsAny.end() ); //we can assert this, since we checked for exists() at the beginning
		iter->second.erase( std::find(iter->second.begin(), iter->second.end(), cmd) );
	}
	else
	{
		m_keyCmdSet.erase( std::make_pair( normKey, cmd ) );

		key_binding::KeyGroupMap::iterator iter = m_groups.find( normKey );
		assert( iter != m_groups.end() ); //we can assert this, since we checked for exists() at the beginning
		iter->second.erase( std::find(iter->second.begin(), iter->second.end(), cmd) );
	}
}

bool key_binding::exists( const wxString& command, const wxString& key )
{
	const wxString& normKey = resolveKeySymName( KeynameConverter::normalizeSpringKey( key ) ); 

	bool found = false;
	if ( normKey.StartsWith( wxT("Any+") ) )
	{
		if ( m_keyCmdSetAny.find( std::make_pair( normKey, command ) ) != m_keyCmdSetAny.end() )
		{
			found = true;
		}
	}
	else
	{
		if ( m_keyCmdSet.find( std::make_pair( normKey, command ) ) != m_keyCmdSet.end() )
		{
			found = true;
		}
	}

	return found;
}

void key_binding::clear()
{
	this->m_groups.clear();
	this->m_groupsAny.clear();
	this->m_keyCmdSetAny.clear();
	this->m_keyCmdSet.clear();
	this->m_keySyms.clear();
}

bool key_binding::operator==(const key_binding& other) const
{
	return ( this->m_groups == other.m_groups ) && ( this->m_groupsAny == other.m_groupsAny ) &&
		( this->m_keySyms == other.m_keySyms );
}

const key_binding key_binding::operator-(const key_binding& other) const
{
	key_binding resBind;
	resBind.m_keySyms = this->m_keySyms;

	//normal keys
	for( KeyGroupMap::const_iterator iter = m_groups.begin(); iter != m_groups.end(); ++iter )
	{
		bool diffs = false;
		for( size_t idx = 0; idx < iter->second.size(); ++idx )
		{
			KeyGroupMap::const_iterator findIter = other.m_groups.find( iter->first );
			if ( ( diffs ) ||
				 ( findIter == other.m_groups.end() ) ||
				 ( iter->second.empty() ) ||
				 ( findIter->second.size() < (idx + 1) ) ||
				 ( findIter->second[idx] != iter->second[idx] ) )
			{
				resBind.bind( iter->second[ idx ], iter->first );
				diffs = true;
			}
		}
	}

	//any-keys
	for( KeyGroupMap::const_iterator iter = m_groupsAny.begin(); iter != m_groupsAny.end(); ++iter )
	{
		bool diffs = false;
		for( size_t idx = 0; idx < iter->second.size(); ++idx )
		{
			KeyGroupMap::const_iterator findIter = other.m_groupsAny.find( iter->first );
			if ( ( diffs ) ||
				 ( findIter == other.m_groupsAny.end() ) ||
				 ( findIter->second.size() < (idx + 1) ) ||
				 ( findIter->second[idx] != iter->second[idx] ) )
			{
				resBind.bind( iter->second[ idx ], iter->first );
				diffs = true;
			}
		}
	}

	return resBind;
}

