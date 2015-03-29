/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef __COMMANDLIST_h__
#define __COMMANDLIST_h__

#include <map>
#include <utility>
#include <vector>

#include <wx/string.h>
#include <wx/button.h>

#include "HotkeyException.h"


class CommandList
{
private:
	static void InitializeCommandList();

public:
	struct Command
	{
		Command()
		    : m_id(0)
		{
		}

		wxString m_command;
		wxString m_category;
		wxString m_description;
		unsigned m_id;
	};

	typedef std::map<wxString, CommandList::Command> CommandMap;

	static CommandList::CommandMap getCommands()
	{
		InitializeCommandList();

		CommandList::CommandMap merged = m_commands;

		for (CommandList::CommandMap::const_iterator iter = CommandList::m_customCommands.begin(); iter != CommandList::m_customCommands.end(); ++iter) {
			merged[iter->first] = iter->second;
		}

		return merged;
	}

	static const CommandList::Command& getCommandByName(const wxString& command)
	{
		InitializeCommandList();

		if (m_commands.find(command) == m_commands.end()) {
			if (m_customCommands.find(command) == m_customCommands.end()) {
				//add the command
				addCustomCommand(command);
			}

			return m_customCommands[command];
		}

		return m_commands[command];
	}

	static void addCustomCommand(const wxString& command)
	{
		wxString category = wxT("Custom");
		wxString descr = wxT("Custom command");

		if (command.StartsWith(wxT("select "))) {
			category = wxT("Selection");
			descr = wxT("User-defined selection command");
		}

		addCustomCommand(category, command, descr);
	}

	static void addCustomCommand(const wxString& category, const wxString& command, const wxString& description)
	{
		if (CommandList::m_customCommands.find(command) != CommandList::m_customCommands.end()) {
			throw HotkeyException(_("Command already exists! Not added!"));
		}

		//put new command to command list
		CommandList::m_customCommands[command] = Command();

		//fill it
		Command& cmd = CommandList::m_customCommands[command];
		cmd.m_category = category;
		cmd.m_command = command;
		cmd.m_description = description;
		cmd.m_id = CommandList::m_nextCmdId++;
	}

private:
	CommandList();

	static void addCommand(const wxString& category, const wxString& command, const wxString& description)
	{
		//put new command to command list
		CommandList::m_commands[command] = Command();

		//fill it
		Command& cmd = CommandList::m_commands[command];
		cmd.m_category = category;
		cmd.m_command = command;
		cmd.m_description = description;
		cmd.m_id = CommandList::m_nextCmdId++;
	}

	static unsigned m_nextCmdId;
	static CommandMap m_commands;
	static CommandMap m_customCommands;
};


#endif
