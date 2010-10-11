#include "HotkeyTypes.h"
#include "HotkeyException.h"

key_binding::key_binding() : m_nextOrderIdx(1)
{
}

void key_binding::bind( const wxString& cmd, const wxString& keyString, unsigned keyOrder )
{
	const wxString& normKey = KeynameConverter::normalizeSpringKey( keyString );
	
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
}

void key_binding::bind( const wxString& cmd, const wxString& keyString )
{
	this->bind( cmd, keyString, m_nextOrderIdx++ );
}
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
void key_binding::unbind( const wxString& cmd, const wxString& keyString )
{
	const key_command searchKey( keyString, cmd );
	bool found = false;

	for( unsigned i = 1; i <= this->m_binds.size(); ++i )
	{
		if ( !found )
		{
			if ( this->m_binds[i] == searchKey )
			{
				found = true;
			}
		}
		else
		{
			this->m_binds[i - 1] = this->m_binds[i];
		}

	}

	if ( found )
	{
		this->m_binds.erase( this->m_binds.size() );
		this->m_c2k[cmd].erase( keyString );

		if ( this->m_c2k[cmd].size() == 0 )
		{
			this->m_c2k.erase( cmd );
		}
		--m_nextOrderIdx;
	}
}

bool key_binding::exists( const wxString& command, const wxString& key )
{
	if ( this->m_c2k.find( command ) == this->m_c2k.end() )
	{
		return false;
	}

	if ( this->m_c2k[ command ].find(key) == this->m_c2k[ command ].end() )
	{
		return false;
	}

	return false;
}

void key_binding::clear()
{
	this->m_binds.clear();
	this->m_c2k.clear();
}

bool key_binding::operator==(const key_binding& other) const
{
	return this->m_binds == other.m_binds;
}

const key_binding key_binding::operator-(const key_binding& other) const
{
	key_binding resBind;

	for( key_binding_map::const_iterator iter = m_binds.begin(); iter != m_binds.end(); ++iter )
	{
		const key_binding_c2k::const_iterator cIter = other.m_c2k.find( iter->second.second );
		if ( cIter == other.m_c2k.end() || ( cIter->second.find( iter->second.first ) == cIter->second.end() ) )
		{
			resBind.bind( iter->second.second, iter->second.first, iter->first );
		}
	}

	return resBind;
}

const key_binding::key_binding_map& key_binding::getBinds() const
{
	return this->m_binds;
}
/*
const key_binding::key_binding_k2c& key_binding::getK2C() const
{
	return this->m_k2c;
}
*/
