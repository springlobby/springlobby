/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "savegamelist.h"

#include <fstream>
#include <wx/filename.h>

#include "storedgame.h"
#include "utils/conversion.h"

SavegameList::SavegameList()
{
}


bool SavegameList::GetSavegameInfos(const std::string& SavegamePath, StoredGame& ret) const
{
	//wxLogMessage(_T("GetSavegameInfos %s"), SavegamePath.c_str());
	//wxLOG_Info  ( STD_STRING( SavegamePath ) );
	//TODO extract moar info
	ret.type = StoredGame::SAVEGAME;
	ret.Filename = SavegamePath;
	ret.battle.SetPlayBackFilePath(SavegamePath);
	if (SavegamePath.empty())
		return false;
	ret.battle.SetScript(GetScriptFromSavegame(SavegamePath));
	//wxLogMessage(_T("Script: %s"), script.c_str());

	if (ret.battle.GetScript().empty())
		return false;

	ret.battle.GetBattleFromScript(false);
	ret.GameName = ret.battle.GetHostGameName();
	ret.battle.SetBattleType(BT_Savegame);
	ret.size = wxFileName::GetSize(TowxString(SavegamePath)).ToULong();

	return true;
}

std::string SavegameList::GetScriptFromSavegame(const std::string& SavegamePath) const
{
	// blatantly copied from spring source
	std::ifstream file(SavegamePath.c_str(), std::ios::in | std::ios::binary);
	std::string script;
	char c;
	if (file.is_open()) {
		do {
			file.read(&c, sizeof(char));
			if (c)
				script += c;
		} while ((c != 0) && !file.fail());
	}
	return script;
}
