#ifndef __COMMANDLIST_h__
#define __COMMANDLIST_h__

#include <map>
#include <utility>
#include <vector>

#include <wx/string.h>


class CommandList
{
private:
	static void InitializeCommandList();

public:
	struct Command
	{
		Command() : m_id(0) {}

		wxString		m_command;
		wxString		m_category;
		wxString		m_description;
		unsigned		m_id;
	};

	typedef std::map<wxString, CommandList::Command>		CommandMap;

	static const CommandList::CommandMap& getCommands()
	{
		CommandList::InitializeCommandList();

		return m_commands;
	}

	static const CommandList::Command& getCommandByName(const wxString& command)
	{
		CommandList::InitializeCommandList();

		if ( m_commands.find( command ) == m_commands.end() )
		{
			//add the command
			CommandList::addCommand( wxT("Custom"), command, wxT("Custom command") );
			//throw std::runtime_error( std::string("Unknown command: ") + command.ToAscii());
		}

		return m_commands[command];
	}

private:
	CommandList();

	static void addCommand(const wxString& category, const wxString& command, const wxString& description )
	{
		//put new command to command list
		CommandList::m_commands[command] = Command();
		
		static unsigned nextId = 1;
		//fill it
		Command& cmd = CommandList::m_commands[command];
		cmd.m_category = category;
		cmd.m_command = command;
		cmd.m_description = description;
		cmd.m_id = nextId++;
	}

	static CommandMap											m_commands;
};



#endif