#ifndef SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
#define SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H


#include <wx/imaglist.h>
#include <map>
#include <vector>

class IBattle;
class wxColour;
struct UserStatus;

class IconImageList : public wxImageList
{
  public:
    IconImageList();

	int GetUserListStateIcon( const UserStatus& us, bool chanop, bool inbroom ) const;
	int GetUserBattleStateIcon( const UserStatus& us ) const;

	int GetRankLimitIcon(  int rank, bool showlowest = true ) const;
	int GetRankIcon( const unsigned int& rank, const bool& showlowest = true ) const;
	int GetFlagIcon( const wxString& flagname ) const;
    int GetBattleStatusIcon( const IBattle& battle ) const;
    wxString GetBattleStatus(const IBattle& battle) const;
	int GetHostIcon( const bool& spectator = false ) const;
	int GetColourIcon( const int& num ) const;
    void SetColourIcon( const int& num, const wxColour& colour );
    int GetSideIcon( const wxString& modname, int side );
	int GetReadyIcon( const bool& spectator, const bool& ready, const unsigned int& sync, const bool& bot );

    int ICON_NONE;

    int ICON_ADMIN;
    int ICON_ADMIN_AWAY;
    int ICON_ADMIN_BROOM;
    int ICON_ADMIN_INGAME;

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

  protected:
    std::map<wxString, int> m_cached_side_icons;
    // why map? because i already included and didn't want to include more stuff, it's not time-critical code anyway
	typedef std::map<unsigned int, unsigned int>
		PlayerColourMap;
	PlayerColourMap m_player_colour_icons;

	std::vector<int> m_rank_requirements;
	int m_minimum_rank_requirement_border;

    typedef std::map<int,int> IndexMap;
    IndexMap m_state_index_map;
};

IconImageList& icons();

#endif // SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

