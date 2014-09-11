/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SAVEGAMELIST_H
#define SAVEGAMELIST_H

#include "iplaybacklist.h"

struct StoredGame;

class SavegameList : public IPlaybackList
{
  public:

   virtual  void LoadPlaybacks( const std::vector<std::string>& filenames );

private:
    SavegameList();

private:
	bool GetSavegameInfos ( const std::string& SavegamePath, StoredGame& ret ) const;
	std::string GetScriptFromSavegame ( const std::string& SavegamePath ) const;
};

#endif // SAVEGAMELIST_H
