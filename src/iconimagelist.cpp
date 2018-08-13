/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: IconImageList
//

#define HAVE_WX
#include "iconimagelist.h"

#include <lslunitsync/image.h>
#include <lslunitsync/unitsync.h>
#include <wx/dc.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/settings.h>
#include <stdexcept>

#include "battle.h"
#include "flagimages.h"
#include "gui/uiutils.h"
#include "images/admin.png.h"
#include "images/admin_away.png.h"
#include "images/admin_broom.png.h"
#include "images/admin_ingame.png.h"
#include "images/away.png.h"
#include "images/bot.xpm"
#include "images/bot_away.xpm"
#include "images/bot_broom.png.h"
#include "images/bot_ingame.png.h"
#include "images/broom.png.h"
#include "images/chanop.xpm"
#include "images/chanop_away.xpm"
#include "images/chanop_broom.xpm"
#include "images/chanop_ingame.xpm"
#include "images/closed_full_game.png.h"
#include "images/closed_full_pw_game.png.h"
#include "images/closed_game.png.h"
#include "images/closed_pw_game.png.h"
#include "images/empty.xpm"
#include "images/ingame.png.h"
#include "images/major.xpm"
#include "images/minor.xpm"
#include "images/no1_icon.png.h"
#include "images/no2_icon.png.h"
#include "images/open_full_game.png.h"
#include "images/open_full_pw_game.png.h"
#include "images/open_game.png.h"
#include "images/open_pw_game.png.h"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"
#include "images/rank7.xpm"
#include "images/rank_unknown.xpm"
#include "images/warning_small.png.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"

IconImageList::IconImageList()
    : wxImageList(16, 16, true)
{
	ICON_ADMIN = Add(charArr2wxBitmap(admin_png, sizeof(admin_png)));
	ICON_ADMIN_AWAY = Add(charArr2wxBitmap(admin_away_png, sizeof(admin_away_png)));
	ICON_ADMIN_BROOM = Add(charArr2wxBitmap(admin_broom_png, sizeof(admin_broom_png)));
	ICON_ADMIN_INGAME = Add(charArr2wxBitmap(admin_ingame_png, sizeof(admin_ingame_png)));

	ICON_BOT = Add(wxBitmap(bot_xpm));
	ICON_BOT_BROOM = Add(charArr2wxBitmap(bot_broom_png, sizeof(bot_broom_png)));
	ICON_BOT_INGAME = Add(charArr2wxBitmap(bot_ingame_png, sizeof(bot_ingame_png)));
	ICON_BOT_AWAY = Add(wxBitmap(bot_away_xpm));

	ICON_AWAY = Add(charArr2wxBitmap(away_png, sizeof(away_png)));
	ICON_BROOM = Add(charArr2wxBitmap(broom_png, sizeof(broom_png)));
	ICON_INGAME = Add(charArr2wxBitmap(ingame_png, sizeof(ingame_png)));

	ICON_OP = Add(wxBitmap(chanop_xpm));
	ICON_OP_AWAY = Add(wxBitmap(chanop_away_xpm));
	ICON_OP_BROOM = Add(wxBitmap(chanop_broom_xpm));
	ICON_OP_INGAME = Add(wxBitmap(chanop_ingame_xpm));

	ICON_RANK_UNKNOWN = Add(wxBitmap(rank_unknown_xpm));
	ICON_RANK1 = Add(wxBitmap(rank0_xpm));
	ICON_RANK2 = Add(wxBitmap(rank1_xpm));
	ICON_RANK3 = Add(wxBitmap(rank2_xpm));
	ICON_RANK4 = Add(wxBitmap(rank3_xpm));
	ICON_RANK5 = Add(wxBitmap(rank4_xpm));
	ICON_RANK6 = Add(wxBitmap(rank5_xpm));
	ICON_RANK7 = Add(wxBitmap(rank6_xpm));
	ICON_RANK8 = Add(wxBitmap(rank7_xpm));

	m_rank_requirements.push_back(ICON_RANK1);
	m_rank_requirements.push_back(ICON_RANK2);
	m_rank_requirements.push_back(ICON_RANK3);
	m_rank_requirements.push_back(ICON_RANK4);
	m_rank_requirements.push_back(ICON_RANK5);
	m_rank_requirements.push_back(ICON_RANK6);
	m_rank_requirements.push_back(ICON_RANK7);
	m_rank_requirements.push_back(ICON_RANK8);

	wxBitmap major_bitmap(major_xpm);
	wxBitmap minor_bitmap(minor_xpm);
	m_minimum_rank_requirement_border = m_rank_requirements.size();
	for (int i = 0; i < m_minimum_rank_requirement_border; i++) {
		wxBitmap to_blend = GetBitmap(m_rank_requirements[i]);
		m_rank_requirements[i] = Add(BlendBitmaps(to_blend, major_bitmap));       // replace old with blended with major
		m_rank_requirements.push_back(Add(BlendBitmaps(to_blend, minor_bitmap))); // add new blended with minor
	}

	ICON_FLAGS_BASE = AddFlagImages(*this);

	ICON_NONE = ICON_NOSTATE = ICON_RANK_NONE = Add(wxBitmap(empty_xpm));
}


IconImageList& icons()
{
	static IconImageList m_icons;
	return m_icons;
}


int IconImageList::GetUserListStateIcon(const UserStatus& us, bool chanop, bool inbroom) const
{
	if (us.bot) {
		if (us.in_game)
			return ICON_BOT_INGAME;
		if (inbroom)
			return ICON_BOT_BROOM;
		if (us.away)
			return ICON_BOT_AWAY;
		return ICON_BOT;
	}
	if (us.moderator) {
		if (us.in_game)
			return ICON_ADMIN_INGAME;
		if (us.away)
			return ICON_ADMIN_AWAY;
		if (inbroom)
			return ICON_ADMIN_BROOM;
		return ICON_ADMIN;
	}
	if (chanop) {
		if (us.in_game)
			return ICON_OP_INGAME;
		if (us.away)
			return ICON_OP_AWAY;
		if (inbroom)
			return ICON_OP_BROOM;
		return ICON_OP;
	}

	if (us.in_game)
		return ICON_INGAME;
	if (us.away)
		return ICON_AWAY;
	if (inbroom)
		return ICON_BROOM;

	return ICON_NOSTATE;
}

int IconImageList::GetRankIcon(const unsigned int& rank, const bool& showlowest) const
{
	if (!showlowest && rank == UserStatus::RANK_1)
		return ICON_RANK_NONE;
	switch (rank) {
		case UserStatus::RANK_1:
			return ICON_RANK1;
		case UserStatus::RANK_2:
			return ICON_RANK2;
		case UserStatus::RANK_3:
			return ICON_RANK3;
		case UserStatus::RANK_4:
			return ICON_RANK4;
		case UserStatus::RANK_5:
			return ICON_RANK5;
		case UserStatus::RANK_6:
			return ICON_RANK6;
		case UserStatus::RANK_7:
			return ICON_RANK7;
		case UserStatus::RANK_8:
			return ICON_RANK8;
		default:
			return ICON_RANK_UNKNOWN;
	}
}

int IconImageList::GetFlagIcon(const std::string& flagname) const
{
	return ICON_FLAGS_BASE + GetFlagIndex(flagname);
}
