#ifndef __TAB_HOTKEY_TYPES_h__
#define __TAB_HOTKEY_TYPES_h__

#include <map>
#include <set>

#include <wx/string.h>
#include "KeynameConverter.h"

struct spring_command
{
	spring_command(wxString cmd) : command(cmd), orderIdx(1) {};
	spring_command(wxString cmd, unsigned order ) : command(cmd), orderIdx(order) {};

	bool operator<( const spring_command& other ) const
	{
		return ( this->orderIdx < other.orderIdx ) || (  this->orderIdx == other.orderIdx && this->command < other.command );
	}

	bool operator==( const spring_command& other ) const
	{
		return ( this->command == other.command ) && ( this->orderIdx == other.orderIdx );
	}

	wxString command;
	unsigned orderIdx;
};

class command_set 
{
public:
	command_set() : m_highestOrderIdx(0) {}

	void clear()
	{
		m_cmds.clear();
		m_rawList.clear();
		m_highestOrderIdx = 0;
	}

	void insert ( const wxString& command )
	{
		insert( command, ++m_highestOrderIdx );
	}

	void insert ( const wxString& command, size_t orderIdx )
	{
		if ( m_rawList.find( command ) != m_rawList.end() )
		{
			//already exists
			return;
		}

		m_cmds.insert( spring_command( command, orderIdx ) );
		m_rawList.insert( command );

		m_highestOrderIdx = std::max( m_highestOrderIdx, orderIdx );
	}

	void erase( const wxString& command )
	{
		if ( m_rawList.find( command ) == m_rawList.end() )
		{
			//does not even exists
			return;
		}

		//copy the content
		const command_list oldList = m_cmds;
		this->clear();

		//reinsert all but one to keep order index consistency
		for( command_list::const_iterator iter = oldList.begin(); iter != oldList.end(); ++iter )
		{
			if ( iter->command == command )
			{
				//skip this
				continue;
			}

			insert( iter->command );
		}
	}

	size_t getHighestOrderIndex()
	{
		return m_highestOrderIdx;
	}

	bool operator==( const command_set other ) const
	{
		return this->getCommands() == other.getCommands();
	}

	typedef std::set<spring_command>		command_list;

	const command_list& getCommands() const
	{
		return this->m_cmds;
	}

private:
	command_list							m_cmds;
	std::set<wxString>						m_rawList;	//aux. list to speed up lookups. does only contain command string without order

	size_t									m_highestOrderIdx;
	//typedef std::map<wxString, size_t>		CmdOrderMap;
	//CmdOrderMap								m_orderMap; //store highest order number here for fast access
};

typedef std::set<wxString>					key_set;

class key_binding
{
public:
	typedef std::map<wxString, key_set>		key_binding_c2k;
	typedef std::map<wxString, command_set>	key_binding_k2c;

	void bind( const wxString& cmd, const wxString& keyString, unsigned keyOrder );
	void bind( const wxString& cmd, const wxString& keyString );
	void bind( const command_set& cmds, const wxString& keyString );

	void unbind( const wxString& cmd, const wxString& keyString );

	bool exists( const wxString& command, const wxString& key );

	void clear();

	const key_binding operator-(const key_binding &other) const;

	const key_binding_c2k& getC2K() const;
	const key_binding_k2c& getK2C() const;

	bool operator==( const key_binding& other ) const;

private:
	key_binding_k2c		m_k2c;
	key_binding_c2k		m_c2k;
};
//typedef std::set<spring_key>				key_set;
//typedef std::map<wxString, key_set>			key_binding_c2k;
//typedef std::map<spring_key, command_set>	key_binding_k2c;

typedef std::map<wxString, key_binding>			key_binding_collection;

#endif
