/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef REPLAY_H_INCLUDED
#define REPLAY_H_INCLUDED

#include "offlinebattle.h"


struct StoredGame
{

	int id;
	int playernum;
	bool can_watch;
	int duration; //in seconds
	int size;     //in bytes
	std::string MapName;
	std::string ModName;
	std::string SpringVersion;
	std::string Filename;
	time_t date;
	std::string date_string;
	OfflineBattle battle;

	enum Type {
		REPLAY,
		SAVEGAME
	} type;

	StoredGame(const size_t idx = 0)
	    : id(idx)
	    , playernum(0)
	    , can_watch(false)
	    , duration(0)
	    , size(0)
	    , date(0)
	    , type(REPLAY)
	{
	}

	StoredGame(const StoredGame& copy) = delete;

	StoredGame(StoredGame&& moved)
	{
		this->operator=((StoredGame && )moved);
	}

	StoredGame& operator=(const StoredGame& copy) = delete;
	StoredGame& operator=(StoredGame&& moved)
	{
		id = moved.id;
		playernum = moved.playernum;
		can_watch = moved.can_watch;
		duration = moved.duration;
		size = moved.size;
		MapName = moved.MapName;
		ModName = moved.ModName;
		SpringVersion = moved.SpringVersion;
		Filename = moved.Filename;
		date = moved.date;
		date_string = moved.date_string;
		battle.operator=((OfflineBattle && )moved.battle);
		return *this;
	}

	bool Equals(const StoredGame& other) const
	{
		return Filename == other.Filename;
	}
};

#endif
