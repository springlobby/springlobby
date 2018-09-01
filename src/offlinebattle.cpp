/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "offlinebattle.h"

#include <lslunitsync/unitsync.h>

#include "spring.h"
#include "utils/conversion.h"

OfflineBattle::OfflineBattle(const int id)
    : m_id(id)
    , m_me("Spectator")
{
	m_opts.founder = m_me.GetNick();
	OnUserAdded(m_me);
	UserBattleStatus& newstatus = m_me.BattleStatus();
	newstatus.spectator = true;
	newstatus.sync = true;
}

void OfflineBattle::StartSpring()
{
	spring().Run(*this);
}

OfflineBattle::OfflineBattle(OfflineBattle&& moved)
    : IBattle()
    , m_id(0)
{
	*this = (OfflineBattle && )moved;
}

OfflineBattle& OfflineBattle::operator=(OfflineBattle&& moved)
{
	// CAUTION: the userlist might contain pointers to the moved.m_me, which is about to go away.
	// So if it exists, remove this pointer and substitute in our own pointer to m_me.
	UserList::operator=((const UserList&&)moved);
	m_id = moved.m_id;
	m_me = moved.m_me;
	auto spec_it = m_users.find("Spectator");
	if (spec_it != m_users.end()) {
		RemoveUser("Spectator");
		OnUserAdded(m_me);
	}
	//	m_map_loaded = moved.m_map_loaded;
	//	m_game_loaded = moved.m_game_loaded;
	//	m_local_map = moved.m_local_map;
	//	m_local_mod = moved.m_local_mod;
	//	m_host_map = moved.m_host_map;
	//	m_host_mod = moved.m_host_mod;
	//	m_previous_local_mod_name = moved.m_previous_local_mod_name;
	//	m_restricted_units = moved.m_restricted_units;
	//	m_opt_wrap = moved.m_opt_wrap;
	m_opts = moved.m_opts;
	//	m_ingame = moved.m_ingame;
	//	m_auto_unspec = moved.m_auto_unspec;
	m_generating_script = moved.m_generating_script;
	//	m_rects = moved.m_rects;
	m_ready_up_map = moved.m_ready_up_map; // player name -> time counting from join/unspect
					       //	m_players_ready = moved.m_players_ready;
					       //	m_players_sync = moved.m_players_sync;
					       //	m_players_ok = moved.m_players_ok;
	m_teams_sizes = moved.m_teams_sizes;   // controlteam -> number of people in
					       //	m_ally_sizes = moved.m_ally_sizes; // allyteam -> number of people in
					       //	m_preset = moved.m_preset;
	m_is_self_in = moved.m_is_self_in;
	//	m_internal_bot_list = moved.m_internal_bot_list;
	//	m_script = moved.m_script;
	//	m_playback_file_path = moved.m_playback_file_path;
	//	m_parsed_teams = moved.m_parsed_teams;
	//	m_parsed_allies = moved.m_parsed_allies;
	//	m_internal_user_list = moved.m_internal_user_list;
	return *this;
}
