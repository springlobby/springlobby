/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SAVEGAME_H_INCLUDED
#define SAVEGAME_H_INCLUDED

#include "offlinebattle.h"

struct Savegame
{
    int id;
    int playernum;
    bool can_watch;
    int duration; //in seconds
	unsigned long size; //in bytes
    std::string MapName;
    std::string ModName;
    std::string SpringVersion;
    std::string Filename;
    time_t date;
    std::string date_string;
    OfflineBattle battle;

	Savegame(const size_t idx = 0):
		id(idx),
		playernum(0),
		can_watch(false),
		duration(0),
		size(0),
		date(0)
	{
	}

	Savegame(const Savegame& copy) = delete;
	Savegame(Savegame&& moved) {
		this->operator=((Savegame&&) moved);
	}
	Savegame& operator=(const Savegame& copy) = delete;
	Savegame& operator=(Savegame&& moved) {
		id				= moved.id;
		playernum		= moved.playernum;
		can_watch		= moved.can_watch;
		duration		= moved.duration;
		size			= moved.size;
		MapName			= moved.MapName;
		ModName			= moved.ModName;
		SpringVersion	= moved.SpringVersion;
		Filename		= moved.Filename;
		date			= moved.date;
		date_string		= moved.date_string;
		battle.operator=((OfflineBattle&&) moved.battle);
		return *this;
	}

    bool Equals( const Savegame& other ) const { return Filename == other.Filename; }
};



#endif // PLAYBACKSTRUCTS_H_INCLUDED
