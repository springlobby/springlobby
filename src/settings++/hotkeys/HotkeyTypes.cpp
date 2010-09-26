#include "HotkeyTypes.h"

void key_binding::bind( const wxString& cmd, const wxString& keyString, unsigned keyOrder )
{
	const wxString normKey = KeynameConverter::normalizeSpringKey( keyString );
	
	this->m_c2k[ cmd ].insert( normKey );
	this->m_k2c[ normKey ].insert( cmd, keyOrder );
}

void key_binding::bind( const wxString& cmd, const wxString& keyString )
{
	const wxString normKey = KeynameConverter::normalizeSpringKey( keyString );
	const size_t orderIdx = 1 + this->m_k2c[ normKey ].getHighestOrderIndex();
	this->bind( cmd, keyString, orderIdx );
}

void key_binding::bind( const command_set& cmds, const wxString& keyString )
{
	const command_set::command_list& cmdList = cmds.getCommands();
	for( command_set::command_list::const_iterator iter = cmdList.begin(); iter != cmdList.end(); ++iter )
	{
		this->bind( iter->command, keyString, iter->orderIdx );
	}
}

void key_binding::unbind( const wxString& cmd, const wxString& keyString )
{
	if ( this->m_c2k.find( cmd ) == this->m_c2k.end() )
	{
		//nothing to unbind
		return;
	}

	if ( this->m_c2k[cmd].find( keyString ) == this->m_c2k[cmd].end() )
	{
		//nothing to unbind
		return;
	}

	this->m_k2c[ keyString ].erase( cmd );
	this->m_c2k[cmd].erase( keyString );
}

bool key_binding::exists( const wxString& command, const wxString& key )
{
	key_binding_c2k::const_iterator citer = this->m_c2k.find( command );
	if ( citer == this->m_c2k.end() )
	{
		return false;
	}

	key_set::const_iterator kiter = citer->second.find( key );
	if ( kiter == citer->second.end() )
	{
		return false;
	}

	return true;
}

void key_binding::clear()
{
	this->m_c2k.clear();
	this->m_k2c.clear();
}

const key_binding key_binding::operator-(const key_binding& other) const 
{
	key_binding resBind;

	const key_binding::key_binding_k2c& otherKeys = other.getK2C();
	for( key_binding_k2c::const_iterator iter = m_k2c.begin(); iter != m_k2c.end(); ++iter )
	{
		if ( otherKeys.find( iter->first ) == otherKeys.end() )
		{
			//key does not exist in other, so we can keep all
			resBind.bind( iter->second, iter->first );
			continue;

		}

		const command_set::command_list& otherCmdList = otherKeys.find( iter->first )->second.getCommands();

		const command_set::command_list& cmds = iter->second.getCommands();
		for( command_set::command_list::const_iterator iiter = cmds.begin(); iiter != cmds.end(); ++iiter )
		{
			if ( otherCmdList.find( (*iiter) ) != otherCmdList.end() )
			{
				//this command exists in other -> dont keep it
				continue;
			}

			resBind.bind( iiter->command, iter->first, iiter->orderIdx ); 
		}
	}

	return resBind;
}

const key_binding::key_binding_c2k& key_binding::getC2K() const
{
	return this->m_c2k;
}

const key_binding::key_binding_k2c& key_binding::getK2C() const
{
	return this->m_k2c;
}

bool key_binding::operator==( const key_binding& other ) const
{
	//just compare one, should be in sync with c2k
	return this->m_k2c == other.getK2C();
}
