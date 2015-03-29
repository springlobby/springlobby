/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "commandlist.h"

CommandList::CommandMap CommandList::m_commands;
CommandList::CommandMap CommandList::m_customCommands;
unsigned CommandList::m_nextCmdId = 1;

void CommandList::InitializeCommandList()
{
	if (!CommandList::m_commands.empty()) {
		return;
	}

	CommandList::addCommand(wxT("General"), wxT("fakemeta"), wxT("Nice command"));

	CommandList::addCommand(wxT("General"), wxT("quitmenu"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("quitforce"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("chat"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("chatally"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("chatswitchally"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("chatall"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("chatswitchall"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("chatspec"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("chatswitchspec"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("edit_return"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_escape"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_complete"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_backspace"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_delete"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_home"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_home"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_end"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_end"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_prev_char"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_next_char"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_prev_word"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_next_word"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_prev_line"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("edit_next_line"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("pastetext"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("pause"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("mouse2"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("mousestate"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("togglecammode"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("togglecammode"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("gameinfo"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("togglelos"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("toggleradarandjammer"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("toggleoverview"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("prevmenu"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("nextmenu"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("decguiopacity"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("incguiopacity"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("hotbind"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("hotunbind"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("speedup"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("speedup"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("slowdown"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("speedup"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("slowdown"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("speedup"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("slowdown"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("debug"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("singlestep"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("sharedialog"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("controlunit"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("trackmode"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("track"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("drawinmap"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("drawinmap"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("drawinmap"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("increaseViewRadius"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("decreaseViewRadius"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("moveforward"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("moveback"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("moveright"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("moveleft"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("moveup"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("movedown"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("movefast"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("moveslow"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("specteam 0"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 1"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 2"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 3"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 4"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 5"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 6"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 7"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 8"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 9"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 10"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 11"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 12"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 13"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 14"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 15"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 16"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 17"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 18"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("specteam 19"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("group0"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group1"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group2"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group3"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group4"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group5"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group6"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group7"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group8"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("group9"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("buildspacing inc"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("buildspacing dec"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("buildfacing inc"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("buildfacing inc"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("buildfacing dec"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("buildfacing dec"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("attack"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("areaattack"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("dgun"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("selfd"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("selfd queued"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("reclaim"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("fight"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("forcestart"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("guard"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("cloak"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("loadunits"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("move"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("patrol"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("repair"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("stop"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("unloadunits"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("unloadunits"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("wait"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("wait queued"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("onoff"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("onoff"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("groupselect"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("groupadd"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("aiselect"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("groupclear"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("viewfps"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("viewta"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("viewtw"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("viewrot"), wxT("Nice command"));
	//
	CommandList::addCommand(wxT("General"), wxT("showElevation"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("ShowPathMap"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("LastMsgPos"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("ShowMetalMap"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("hideinterface"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("NoSound"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("dynamicSky"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("savegame"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("showhealthbars"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("createvideo"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("screenshot"), wxT("Nice command"));
	CommandList::addCommand(wxT("General"), wxT("screenshot"), wxT("Nice command"));
}
