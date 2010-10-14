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

	const wxString normKey = KeynameConverter::normalizeSpringKey( keyString );	
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
		
/*
	if ( this->m_binds.find(keyOrder) != this->m_binds.end() )
	{
		const size_t size = this->m_binds.size();
		for( unsigned i = 1; i <= size; ++i )
		{
			if ( i >= keyOrder )
			{
				this->m_binds[i+1] = this->m_binds[i];
			}
		}
		++this->m_nextOrderIdx;
	}
	else
	{
		this->m_nextOrderIdx = std::max( this->m_nextOrderIdx, keyOrder );
	}

	this->m_binds[keyOrder] = key_command(normKey, cmd);
	this->m_c2k[cmd].insert( normKey );
	this->m_isNormalized = false;*/
}

/*
void key_binding::bind( const wxString& cmd, const wxString& keyString )
{
	this->bind( cmd, keyString, m_nextOrderIdx++ );
}*/
/*
void key_binding::bind( const command_set& cmds, const wxString& keyString )
{
	const command_set::command_list& cmdList = cmds.getCommands();
	for( command_set::command_list::const_iterator iter = cmdList.begin(); iter != cmdList.end(); ++iter )
	{
		this->bind( iter->command, keyString, iter->orderIdx );
	}
}
*/

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
	const wxString& normKey = KeynameConverter::normalizeSpringKey( key );

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
}

bool key_binding::operator==(const key_binding& other) const
{
	return ( this->m_groups == other.m_groups ) && ( this->m_groupsAny == other.m_groupsAny );
}

const key_binding key_binding::operator-(const key_binding& other) const
{
	key_binding resBind;

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
				 ( findIter->second.size() < (idx - 1) ) ||
				 ( findIter->second[idx] != iter->second[idx] ) )
			{
				resBind.bind( iter->second[ idx ], iter->first );
				diffs = true;
			}
		}
	}

	return resBind;
}
/*
const key_binding::key_binding_map& key_binding::getBinds() const
{
	return this->m_binds;
}*/
/*
void key_binding::normalize()
{
	if ( this->m_isNormalized )
		return;



	typedef std::vector<NormCommand>							NormCmdList;
	typedef std::map<wxString, NormCmdList>						KeyMap;
	typedef std::set<wxString>									KeyList;

	KeyList anyKeys;
	KeyMap normBinds;
	for( unsigned i = 1; i <= this->m_binds.size(); ++i )
	{
		bool any = false;
		wxString keyString = this->m_binds[i].first;
		if ( keyString.StartsWith( wxT("Any+") ) )
		{
			anyKeys.insert( keyString );
			keyString = keyString.AfterLast( wxT('+') );
			any = true;
		}
		
		normBinds[ keyString ].push_back( NormCommand( this->m_binds[i].second, any ) );
	}

	//backup binds and clear
	key_binding_map bindBak = this->m_binds;
	this->m_binds.clear();

	//get all keys BEFORE the ANY-keys
	for( KeyMap::const_iterator iter = normBinds.begin(); iter != normBinds.end(); ++iter )
	{
		const NormCmdList& normCmds = iter->second;

		for( NormCmdList::const_iterator iter2 = normCmds.begin(); iter2 != normCmds.end(); ++iter2 )
		{
			if ( iter2->any )
			{
				break;
			}

			this->bind( iter2->command, iter->first );
		}
	}

	//add the ANY-keys
	for( KeyMap::const_iterator iter = normBinds.begin(); iter != normBinds.end(); ++iter )
	{
		const NormCmdList& normCmds = iter->second;

		for( NormCmdList::const_iterator iter2 = normCmds.begin(); iter2 != normCmds.end(); ++iter2 )
		{
			if ( iter2->any )
			{
				this->bind( iter2->command, wxT("Any+") + iter->first );
				break;
			}
		}
	}

	//get all keys AFTER the ANY-keys
	for( KeyMap::const_iterator iter = normBinds.begin(); iter != normBinds.end(); ++iter )
	{
		const NormCmdList& normCmds = iter->second;

		bool anyFound = false;
		for( NormCmdList::const_iterator iter2 = normCmds.begin(); iter2 != normCmds.end(); ++iter2 )
		{
			if ( anyFound )
			{
				this->bind( iter2->command, iter->first );
			}
			else if ( iter2->any )
			{
				anyFound = true;
			}
		}
	}

	this->m_isNormalized = true;
}
*/
/*
const key_binding::key_binding_k2c& key_binding::getK2C() const
{
	return this->m_k2c;
}
*/
