#ifndef __COMMANDLIST_h__
#define __COMMANDLIST_h__

#include <map>
#include <utility>
#include <vector>


class CommandList
{
private:
	static void InitializeCommandList()
	{
		CommandList::m_commands.clear();

		CommandList::addCommand("General", "quitmenu", "Nice command");
		CommandList::addCommand("General", "quitforce", "Nice command");
		//
		CommandList::addCommand("General", "chat", "Nice command");
		CommandList::addCommand("General", "chatally", "Nice command");
		CommandList::addCommand("General", "chatswitchally", "Nice command");
		CommandList::addCommand("General", "chatall", "Nice command");
		CommandList::addCommand("General", "chatswitchall", "Nice command");
		CommandList::addCommand("General", "chatspec", "Nice command");
		CommandList::addCommand("General", "chatswitchspec", "Nice command");
		//
		CommandList::addCommand("General", "edit_return", "Nice command");
		CommandList::addCommand("General", "edit_escape", "Nice command");
		CommandList::addCommand("General", "edit_complete", "Nice command");
		CommandList::addCommand("General", "edit_backspace", "Nice command");
		CommandList::addCommand("General", "edit_delete", "Nice command");
		CommandList::addCommand("General", "edit_home", "Nice command");
		CommandList::addCommand("General", "edit_home", "Nice command");
		CommandList::addCommand("General", "edit_end", "Nice command");
		CommandList::addCommand("General", "edit_end", "Nice command");
		CommandList::addCommand("General", "edit_prev_char", "Nice command");
		CommandList::addCommand("General", "edit_next_char", "Nice command");
		CommandList::addCommand("General", "edit_prev_word", "Nice command");
		CommandList::addCommand("General", "edit_next_word", "Nice command");
		CommandList::addCommand("General", "edit_prev_line", "Nice command");
		CommandList::addCommand("General", "edit_next_line", "Nice command");
		//
		CommandList::addCommand("General", "pastetext", "Nice command");
		//
		CommandList::addCommand("General", "pause", "Nice command");
		//
		CommandList::addCommand("General", "mouse2", "Nice command");
		CommandList::addCommand("General", "mousestate", "Nice command");
		CommandList::addCommand("General", "togglecammode", "Nice command");
		CommandList::addCommand("General", "togglecammode", "Nice command");
		//
		CommandList::addCommand("General", "gameinfo", "Nice command");
		//
		CommandList::addCommand("General", "togglelos", "Nice command");
		CommandList::addCommand("General", "toggleradarandjammer", "Nice command");
		CommandList::addCommand("General", "toggleoverview", "Nice command");
		//
		CommandList::addCommand("General", "prevmenu", "Nice command");
		CommandList::addCommand("General", "nextmenu", "Nice command");
		CommandList::addCommand("General", "decguiopacity", "Nice command");
		CommandList::addCommand("General", "incguiopacity", "Nice command");
		//
		CommandList::addCommand("General", "hotbind", "Nice command");
		CommandList::addCommand("General", "hotunbind", "Nice command");
		//
		CommandList::addCommand("General", "speedup", "Nice command");
		CommandList::addCommand("General", "speedup", "Nice command");
		CommandList::addCommand("General", "slowdown", "Nice command");
		CommandList::addCommand("General", "speedup", "Nice command");
		CommandList::addCommand("General", "slowdown", "Nice command");
		CommandList::addCommand("General", "speedup", "Nice command");
		CommandList::addCommand("General", "slowdown", "Nice command");
		//
		CommandList::addCommand("General", "debug", "Nice command");
		CommandList::addCommand("General", "singlestep", "Nice command");
		//
		CommandList::addCommand("General", "sharedialog", "Nice command");
		//
		CommandList::addCommand("General", "controlunit", "Nice command");
		CommandList::addCommand("General", "trackmode", "Nice command");
		CommandList::addCommand("General", "track", "Nice command");
		//
		CommandList::addCommand("General", "drawinmap", "Nice command");
		CommandList::addCommand("General", "drawinmap", "Nice command");
		CommandList::addCommand("General", "drawinmap", "Nice command");
		//
		CommandList::addCommand("General", "increaseViewRadius", "Nice command");
		CommandList::addCommand("General", "decreaseViewRadius", "Nice command");
		//
		CommandList::addCommand("General", "moveforward", "Nice command");
		CommandList::addCommand("General", "moveback", "Nice command");
		CommandList::addCommand("General", "moveright", "Nice command");
		CommandList::addCommand("General", "moveleft", "Nice command");
		CommandList::addCommand("General", "moveup", "Nice command");
		CommandList::addCommand("General", "movedown", "Nice command");
		CommandList::addCommand("General", "movefast", "Nice command");
		CommandList::addCommand("General", "moveslow", "Nice command");
		//
		CommandList::addCommand("General", "specteam 0", "Nice command"); 
		CommandList::addCommand("General", "specteam 1", "Nice command");
		CommandList::addCommand("General", "specteam 2", "Nice command");
		CommandList::addCommand("General", "specteam 3", "Nice command");
		CommandList::addCommand("General", "specteam 4", "Nice command");
		CommandList::addCommand("General", "specteam 5", "Nice command");
		CommandList::addCommand("General", "specteam 6", "Nice command");
		CommandList::addCommand("General", "specteam 7", "Nice command");
		CommandList::addCommand("General", "specteam 8", "Nice command");
		CommandList::addCommand("General", "specteam 9", "Nice command");
		CommandList::addCommand("General", "specteam 10", "Nice command");
		CommandList::addCommand("General", "specteam 11", "Nice command");
		CommandList::addCommand("General", "specteam 12", "Nice command");
		CommandList::addCommand("General", "specteam 13", "Nice command");
		CommandList::addCommand("General", "specteam 14", "Nice command");
		CommandList::addCommand("General", "specteam 15", "Nice command");
		CommandList::addCommand("General", "specteam 16", "Nice command");
		CommandList::addCommand("General", "specteam 17", "Nice command");
		CommandList::addCommand("General", "specteam 18", "Nice command");
		CommandList::addCommand("General", "specteam 19", "Nice command");
		//
		CommandList::addCommand("General", "group0", "Nice command");
		CommandList::addCommand("General", "group1", "Nice command");
		CommandList::addCommand("General", "group2", "Nice command");
		CommandList::addCommand("General", "group3", "Nice command");
		CommandList::addCommand("General", "group4", "Nice command");
		CommandList::addCommand("General", "group5", "Nice command");
		CommandList::addCommand("General", "group6", "Nice command");
		CommandList::addCommand("General", "group7", "Nice command");
		CommandList::addCommand("General", "group8", "Nice command");
		CommandList::addCommand("General", "group9", "Nice command");
		//
		CommandList::addCommand("General", "buildspacing inc", "Nice command");
		CommandList::addCommand("General", "buildspacing dec", "Nice command");
		//
		CommandList::addCommand("General", "buildfacing inc", "Nice command");
		CommandList::addCommand("General", "buildfacing inc", "Nice command");
		CommandList::addCommand("General", "buildfacing dec", "Nice command");
		CommandList::addCommand("General", "buildfacing dec", "Nice command");
		//
		CommandList::addCommand("General", "attack", "Nice command");
		CommandList::addCommand("General", "areaattack", "Nice command");
		CommandList::addCommand("General", "dgun", "Nice command");
		CommandList::addCommand("General", "selfd", "Nice command");
		CommandList::addCommand("General", "selfd queued", "Nice command");
		CommandList::addCommand("General", "reclaim", "Nice command");
		CommandList::addCommand("General", "fight", "Nice command");
		CommandList::addCommand("General", "forcestart", "Nice command");
		CommandList::addCommand("General", "guard", "Nice command");
		CommandList::addCommand("General", "cloak", "Nice command");
		CommandList::addCommand("General", "loadunits", "Nice command");
		CommandList::addCommand("General", "move", "Nice command");
		CommandList::addCommand("General", "patrol", "Nice command");
		CommandList::addCommand("General", "repair", "Nice command");
		CommandList::addCommand("General", "stop", "Nice command");
		CommandList::addCommand("General", "unloadunits", "Nice command");
		CommandList::addCommand("General", "unloadunits", "Nice command");
		CommandList::addCommand("General", "wait", "Nice command");
		CommandList::addCommand("General", "wait queued", "Nice command");
		CommandList::addCommand("General", "onoff", "Nice command");
		CommandList::addCommand("General", "onoff", "Nice command");
		//
		CommandList::addCommand("General", "groupselect", "Nice command");
		CommandList::addCommand("General", "groupadd", "Nice command");
		CommandList::addCommand("General", "aiselect", "Nice command");
		CommandList::addCommand("General", "groupclear", "Nice command");
		//
		CommandList::addCommand("General", "viewfps", "Nice command");
		CommandList::addCommand("General", "viewta", "Nice command");
		CommandList::addCommand("General", "viewtw", "Nice command");
		CommandList::addCommand("General", "viewrot", "Nice command");
		//
		CommandList::addCommand("General", "showElevation", "Nice command");
		CommandList::addCommand("General", "ShowPathMap", "Nice command");
		CommandList::addCommand("General", "LastMsgPos", "Nice command");
		CommandList::addCommand("General", "ShowMetalMap", "Nice command");
		CommandList::addCommand("General", "hideinterface", "Nice command");
		CommandList::addCommand("General", "NoSound", "Nice command");
		CommandList::addCommand("General", "dynamicSky", "Nice command");
		CommandList::addCommand("General", "savegame", "Nice command");
		CommandList::addCommand("General", "showhealthbars", "Nice command");
		CommandList::addCommand("General", "createvideo", "Nice command");
		CommandList::addCommand("General", "screenshot", "Nice command");
		CommandList::addCommand("General", "screenshot", "Nice command");
	}

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
		checkInit();

		return m_commands;
	}

	static const CommandList::Command& getCommandByName(const wxString& command)
	{
		checkInit();

		return m_commands[command];
	}
	/*
	static const CategoryCmdList& getCommandCategoryList()
	{
		if ( CommandList::m_commands.size() == 0 )
		{
			CommandList::InitializeCommandList();
		}
		return CommandList::m_commands;
	}
	*/
/*
	static const SpringCommandList& getCommandListByCat( const wxString& category )
	{
		if ( CommandList::m_commands.size() == 0 )
		{
			CommandList::InitializeCommandList();
		}

		if ( CommandList::m_commands.find(category.ToAscii()) == CommandList::m_commands.end() )
		{
			throw std::runtime_error( std::string("getCommandListByCat: Unknown category: ") + category.ToAscii() );
		}
		return CommandList::m_commands[category.ToAscii()];
	}
*/
private:
	CommandList();

	static void checkInit()
	{
		if ( CommandList::m_commands.size() == 0 )
		{
			CommandList::InitializeCommandList();
		}
	}

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

CommandList::CommandMap CommandList::m_commands;


#endif