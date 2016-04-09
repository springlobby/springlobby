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

	int ICON_FLAGS_BASE;

private:
	std::vector<int> m_rank_requirements;
	int m_minimum_rank_requirement_border;
};

IconImageList& icons();

#endif // SPRINGLOBBY_HEADERGUARD_ICONIMAGELIST_H
