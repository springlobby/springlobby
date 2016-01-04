/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
#define SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H


#include <wx/imaglist.h>
#include <map>
#include <vector>

class IBattle;
namespace LSL
{
class lslColor;
}

struct UserStatus;

class IconImageList : public wxImageList
{
public:
	IconImageList();

	int GetUserListStateIcon(const UserStatus& us, bool chanop, bool inbroom) const;

	int GetRankIcon(const unsigned int& rank, const bool& showlowest = true) const;
	int GetFlagIcon(const std::string& flagname) const;

	int ICON_NONE;

	int ICON_ADMIN;
	int ICON_ADMIN_AWAY;
	int ICON_ADMIN_BROOM;
	int ICON_ADMIN_INGAME;

	int ICON_PLAYER;

	int ICON_BOT;
	int ICON_BOT_BROOM;
	int ICON_BOT_INGAME;
	int ICON_BOT_AWAY;

	int ICON_NOSTATE;
	int ICON_AWAY;
	int ICON_BROOM;
	int ICON_INGAME;

	int ICON_OP;
	int ICON_OP_AWAY;
	int ICON_OP_BROOM;
	int ICON_OP_INGAME;

	int ICON_UP;
	int ICON_DOWN;

	int ICON_RANK_NONE;
	int ICON_RANK_UNKNOWN;
	int ICON_RANK1;
	int ICON_RANK2;
	int ICON_RANK3;
	int ICON_RANK4;
	int ICON_RANK5;
	int ICON_RANK6;
	int ICON_RANK7;
	int ICON_RANK8;

	int ICON_GAME_UNKNOWN;
	int ICON_OPEN_GAME;
	int ICON_OPEN_PW_GAME;
	int ICON_OPEN_FULL_PW_GAME;
	int ICON_OPEN_FULL_GAME;
	int ICON_CLOSED_GAME;
	int ICON_CLOSED_PW_GAME;
	int ICON_CLOSED_FULL_PW_GAME;
	int ICON_CLOSED_FULL_GAME;
	int ICON_STARTED_GAME;
	int ICON_STARTED_GAME_LOCKED;

	int ICON_READY_UNSYNC;
	int ICON_NREADY_UNSYNC;
	int ICON_READY_QSYNC;
	int ICON_NREADY_QSYNC;
	int ICON_NREADY;
	int ICON_READY;

	int ICON_NEXISTS;
	int ICON_EXISTS;

	int ICON_SPECTATOR;
	int ICON_HOST;
	int ICON_HOST_SPECTATOR;

	int ICON_SIDEPIC_0;
	int ICON_SIDEPIC_1;

	//int ICON_FIXCOLOURS_PALETTE;

	int ICON_UNK_FLAG;
	int ICON_FLAGS_BASE;

	int ICON_WARNING_OVERLAY;

	int ICON_CHANNEL_OPTIONS;

	int ICON_EMPTY;

	int ICON_SPRINGLOBBY;

private:
	std::map<std::string, int> m_cached_side_icons;
	// why map? because i already included and didn't want to include more stuff, it's not time-critical code anyway
	typedef std::map<wxString, unsigned int>
	    PlayerColourMap;
	PlayerColourMap m_player_colour_icons;

	std::vector<int> m_rank_requirements;
	int m_minimum_rank_requirement_border;

	typedef std::map<int, int> IndexMap;
	IndexMap m_state_index_map;
};

IconImageList& icons();

#endif // SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
