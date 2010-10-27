#include <algorithm>

#include "HotkeyTypes.h"
#include "HotkeyException.h"


key_binding::key_binding() //: m_nextOrderIdx(1)
{
}

void key_binding::setMetaKey( const wxString& key )
{
	this->m_meta = key;
}

const wxString& key_binding::getMetaKey() const
{
	return this->m_meta;
}

void key_binding::bind( const wxString& cmd, const wxString& keyString )
{
	if ( this->exists( cmd, keyString ) )
	{
		return;
	}

	wxString normKey;
	if ( keyString.StartsWith( wxT("&") ) )
	{
		//key set
		normKey = resolveKeySymSetName( KeynameConverter::normalizeSpringKey( keyString.SubString( 1, keyString.size() + 1 ) ) );
	}
	else
	{
		//normal key
		normKey = resolveKeySymName( KeynameConverter::normalizeSpringKey( keyString ) );
	}

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

void key_binding::addKeySymSet( const wxString& name, const wxString& keyString )
{
	const wxString normName = name.Lower();
	const wxString normKey = this->resolveKeySymName( KeynameConverter::normalizeSpringKey( keyString ) );
	this->m_keySymsSet[normName] = normKey;
	this->m_keySymsSetRev[normKey] = normName;
}

void key_binding::addKeySym( const wxString& name, const wxString& keyString )
{
/*	const wxString normName = name.Lower();
	this->m_keySyms[normName] = keyString;
	this->m_keySymsRev[KeynameConverter::convertHexValueToKey( keyString )] = normName;
*/
	const wxString normName = name.Lower();
	const wxString normKey = KeynameConverter::normalizeSpringKey( keyString );
	this->m_keySyms[normName] = normKey;
	this->m_keySymsRev[KeynameConverter::convertHexValueToKey( keyString )] = normName;
}

const wxString key_binding::resolveKeySymSetName( const wxString& symName ) const
{
	key_sym_set_map::const_iterator iter = this->m_keySymsSet.find( symName );
	if ( iter == this->m_keySymsSet.end() )
	{
		return symName;
	}

	return iter->second;
}

const wxString key_binding::resolveKeySymSetKey( const wxString& key ) const
{
	key_sym_set_map::const_iterator iter = this->m_keySymsSetRev.find( key );
	if ( iter == this->m_keySymsSetRev.end() )
	{
		return key;
	}

	return wxT("&") + iter->second;
}

const wxString key_binding::resolveKeySymName( const wxString& symName ) const
{
	key_sym_map::const_iterator iter = this->m_keySyms.find( KeynameConverter::discardModifier( symName ) );
	if ( iter == this->m_keySyms.end() )
	{
		return symName;
	}

	KeynameConverter::ModifierList modList = KeynameConverter::stringToKeyModifier( symName );
	return KeynameConverter::modifier2String( modList ) + KeynameConverter::convertHexValueToKey( iter->second );
}

wxString key_binding::resolveKeySymKeyAndSet( const wxString& key ) const
{
	wxString res = resolveKeySymSetKey( key );
	
	return resolveKeySymKey(res);
}

wxString key_binding::resolveKeySymKey( const wxString& key ) const
{
	key_sym_map::const_iterator iter = this->m_keySymsRev.find( KeynameConverter::discardModifier( key ) );
	if ( iter == this->m_keySymsRev.end() )
	{
		return key;
	}

	KeynameConverter::ModifierList modList = KeynameConverter::stringToKeyModifier( key );
	return KeynameConverter::modifier2String( modList ) + iter->second;
}

void key_binding::setKeySymsSet( const key_sym_set_map& keySymsSet )
{
	this->m_keySymsSet = keySymsSet;
	
	//update reverse map
	for( key_sym_set_map::const_iterator iter = m_keySymsSet.begin(); iter != m_keySymsSet.end(); ++iter )
	{
		this->m_keySymsSetRev[iter->second] = iter->first;
	}
}

void key_binding::setKeySyms( const key_sym_map& keySyms )
{
	this->m_keySyms = keySyms;
	
	//update reverse map
	for( key_sym_map::const_iterator iter = m_keySyms.begin(); iter != m_keySyms.end(); ++iter )
	{
		//hex convert is needed here (data could come from wxProfile which stores raw keys (0x62 etc))
		this->m_keySymsRev[KeynameConverter::convertHexValueToKey( iter->second )] = iter->first;
	}
}

const key_sym_map& key_binding::getKeySyms() const
{
	return this->m_keySyms;
}

const key_sym_map& key_binding::getKeySymsSet() const
{
	return this->m_keySymsSet;
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

		if ( iter->second.size() == 0 )
			m_groupsAny.erase( iter->first );
	}
	else
	{
		m_keyCmdSet.erase( std::make_pair( normKey, cmd ) );

		key_binding::KeyGroupMap::iterator iter = m_groups.find( normKey );
		assert( iter != m_groups.end() ); //we can assert this, since we checked for exists() at the beginning
		iter->second.erase( std::find(iter->second.begin(), iter->second.end(), cmd) );

		if ( iter->second.size() == 0 )
			m_groups.erase( iter->first );
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
	this->m_keySymsSet.clear();
	this->m_meta.clear();
}

bool key_binding::operator==(const key_binding& other) const
{
	return ( this->m_groups == other.m_groups ) && ( this->m_groupsAny == other.m_groupsAny ) &&
		( this->m_keySyms == other.m_keySyms ) && ( this->m_keySymsSet == other.m_keySymsSet ) &&
		( this->m_meta == other.m_meta );
}

const key_binding key_binding::operator-(const key_binding& other) const
{
	key_binding resBind = (*this);
	resBind.m_groups.clear();
	resBind.m_groupsAny.clear();
	resBind.m_keyCmdSet.clear();
	resBind.m_keyCmdSetAny.clear();

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

