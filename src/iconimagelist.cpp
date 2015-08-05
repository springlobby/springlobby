/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: IconImageList
//

#include <stdexcept>

#include <wx/image.h>
#include <wx/settings.h>
#include <wx/dc.h>
#include <wx/icon.h>

#include "iconimagelist.h"
#include "user.h"
#include "battle.h"
#define HAVE_WX
#include <lslunitsync/image.h>
#include <lslunitsync/unitsync.h>

#include "images/bot.xpm"
#include "images/bot_broom.png.h"
#include "images/bot_ingame.png.h"
#include "images/bot_away.xpm"

#include "images/admin.png.h"
#include "images/admin_away.png.h"
#include "images/admin_broom.png.h"
#include "images/admin_ingame.png.h"

#include "images/chanop.xpm"
#include "images/chanop_away.xpm"
#include "images/chanop_broom.xpm"
#include "images/chanop_ingame.xpm"

#include "images/away.png.h"
#include "images/broom.png.h"
#include "images/ingame.png.h"

#include "images/up.xpm"
#include "images/down.xpm"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"
#include "images/rank7.xpm"
#include "images/rank_unknown.xpm"

#include "images/major.xpm"
#include "images/minor.xpm"

#include "images/open_game.png.h"
#include "images/open_pw_game.png.h"
#include "images/open_full_pw_game.png.h"
#include "images/open_full_game.png.h"
#include "images/closed_game.png.h"
#include "images/closed_pw_game.png.h"
#include "images/closed_full_pw_game.png.h"
#include "images/closed_full_game.png.h"

#include "images/player.xpm"

#include "images/nexists.xpm"
#include "images/exists.xpm"

#include "images/ready_unsync.xpm"
#include "images/ready_q.xpm"
#include "images/nready_unsync.xpm"
#include "images/nready_q.xpm"

#include "images/spectator.png.h"
#include "images/host.xpm"
#include "images/host_spectator.xpm"

#include "images/no1_icon.png.h"
#include "images/no2_icon.png.h"
#include "images/warning_small.png.h"

#include "images/unknown_flag.xpm"

#include "images/channel_options.xpm"
#include "images/springlobby.xpm"

#include "flagimages.h"

#include "images/empty.xpm"
#include "gui/uiutils.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"

IconImageList::IconImageList()
    : wxImageList(16, 16, true)
{
	ICON_ADMIN = Add(charArr2wxBitmap(admin_png, sizeof(admin_png)));
	ICON_ADMIN_AWAY = Add(charArr2wxBitmap(admin_away_png, sizeof(admin_away_png)));
	ICON_ADMIN_BROOM = Add(charArr2wxBitmap(admin_broom_png, sizeof(admin_broom_png)));
	ICON_ADMIN_INGAME = Add(charArr2wxBitmap(admin_ingame_png, sizeof(admin_ingame_png)));

	ICON_PLAYER = Add(wxBitmap(player_xpm));

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

	ICON_UP = Add(wxBitmap(up_xpm));
	ICON_DOWN = Add(wxBitmap(down_xpm));

	ICON_RANK_UNKNOWN = Add(wxBitmap(rank_unknown_xpm));
	ICON_RANK1 = Add(wxBitmap(rank0_xpm));
	ICON_RANK2 = Add(wxBitmap(rank1_xpm));
	ICON_RANK3 = Add(wxBitmap(rank2_xpm));
	ICON_RANK4 = Add(wxBitmap(rank3_xpm));
	ICON_RANK5 = Add(wxBitmap(rank4_xpm));
	ICON_RANK6 = Add(wxBitmap(rank5_xpm));
	ICON_RANK7 = Add(wxBitmap(rank6_xpm));
	ICON_RANK8 = Add(wxBitmap(rank7_xpm));
	ICON_SPRINGLOBBY = Add(wxBitmap(springlobby_xpm));

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

	ICON_READY = ICON_OPEN_GAME = Add(charArr2wxBitmap(open_game_png, sizeof(open_game_png)));
	ICON_OPEN_PW_GAME = Add(charArr2wxBitmap(open_pw_game_png, sizeof(open_pw_game_png)));
	ICON_OPEN_FULL_PW_GAME = Add(charArr2wxBitmap(open_full_pw_game_png, sizeof(open_full_pw_game_png)));
	ICON_OPEN_FULL_GAME = Add(charArr2wxBitmap(open_full_game_png, sizeof(open_full_game_png)));
	ICON_NREADY = ICON_CLOSED_GAME = Add(charArr2wxBitmap(closed_game_png, sizeof(closed_game_png)));
	ICON_CLOSED_PW_GAME = Add(charArr2wxBitmap(closed_pw_game_png, sizeof(closed_pw_game_png)));
	ICON_CLOSED_FULL_PW_GAME = Add(charArr2wxBitmap(closed_full_pw_game_png, sizeof(closed_full_pw_game_png)));
	ICON_CLOSED_FULL_GAME = Add(charArr2wxBitmap(closed_full_game_png, sizeof(closed_full_game_png)));
	ICON_STARTED_GAME = Add(charArr2wxBitmap(ingame_png, sizeof(ingame_png)));

	ICON_STARTED_GAME_LOCKED = Add(charArr2wxBitmapWithBlending(closed_game_png, sizeof(closed_game_png), ingame_png, sizeof(ingame_png)));


	ICON_READY_UNSYNC = Add(wxBitmap(ready_unsync_xpm));
	ICON_NREADY_UNSYNC = Add(wxBitmap(nready_unsync_xpm));
	ICON_READY_QSYNC = Add(wxBitmap(ready_q_xpm));
	ICON_NREADY_QSYNC = Add(wxBitmap(nready_q_xpm));

	ICON_NEXISTS = Add(wxBitmap(nexists_xpm));
	ICON_EXISTS = Add(wxBitmap(exists_xpm));

	ICON_SPECTATOR = Add(charArr2wxBitmap(spectator_png, sizeof(spectator_png)));
	ICON_HOST = Add(wxBitmap(host_xpm));
	ICON_HOST_SPECTATOR = Add(wxBitmap(host_spectator_xpm));

	ICON_SIDEPIC_0 = Add(charArr2wxBitmap(no1_icon_png, sizeof(no1_icon_png)));
	ICON_SIDEPIC_1 = Add(charArr2wxBitmap(no2_icon_png, sizeof(no2_icon_png)));

	ICON_CHANNEL_OPTIONS = Add(wxBitmap(channel_options_xpm));

	//	SetColourIcon( wxColour( 255,   0,   0 ) );
	//	SetColourIcon( wxColour(   0, 255,   0 ) );
	//	SetColourIcon( wxColour(   0,   0, 255 ) );


	//ICON_FIXCOLOURS_PALETTE = Add( wxBitmap(fixcolours_palette_xpm) );

	ICON_UNK_FLAG = Add(wxBitmap(unknown_flag_xpm));

	ICON_FLAGS_BASE = AddFlagImages(*this);

	ICON_EMPTY = Add(wxBitmap(empty_xpm));

	ICON_NONE = ICON_NOSTATE = ICON_RANK_NONE = ICON_GAME_UNKNOWN = ICON_EMPTY;

	ICON_WARNING_OVERLAY = Add(charArr2wxBitmap(warning_small_png, sizeof(warning_small_png)));
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


int IconImageList::GetUserBattleStateIcon(const UserStatus& us) const
{
	if (us.bot)
		return ICON_BOT;
	if (us.moderator) {
		if (us.in_game)
			return ICON_ADMIN_INGAME;
		if (us.away)
			return ICON_ADMIN_AWAY;
		return ICON_ADMIN;
	}

	if (us.in_game)
		return ICON_INGAME;
	if (us.away)
		return ICON_AWAY;

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

int IconImageList::GetRankLimitIcon(int rank, bool showlowest) const
{
	if (!showlowest && rank == UserStatus::RANK_1)
		return ICON_RANK_NONE;

	if (rank < UserStatus::RANK_1) {
		rank = -rank - 1 + m_minimum_rank_requirement_border;
	}
	if (rank > int(m_rank_requirements.size()))
		return ICON_RANK_UNKNOWN;

	return m_rank_requirements[rank];
}


int IconImageList::GetFlagIcon(const std::string& flagname) const
{
	return ICON_FLAGS_BASE + GetFlagIndex(flagname);
}


int IconImageList::GetBattleStatusIcon(const IBattle& battle) const
{
	unsigned idx = battle.GetInGame() << 3 | battle.IsLocked() << 2 | battle.IsFull() << 1 | battle.IsPassworded() << 0;
	static const int icons[16] = {
	    /* -                                 */ ICON_OPEN_GAME,
	    /* passworded                        */ ICON_OPEN_PW_GAME,
	    /* full                              */ ICON_OPEN_FULL_GAME,
	    /* full, passworded                  */ ICON_OPEN_FULL_PW_GAME,
	    /* locked                            */ ICON_CLOSED_GAME,
	    /* locked, passworded                */ ICON_CLOSED_PW_GAME,
	    /* locked, full                      */ ICON_CLOSED_FULL_GAME,
	    /* locked, full, passworded          */ ICON_CLOSED_FULL_PW_GAME,
	    /* in game                           */ ICON_STARTED_GAME,
	    /* in game, passworded               */ ICON_STARTED_GAME,
	    /* in game, full                     */ ICON_STARTED_GAME,
	    /* in game, full, passworded         */ ICON_STARTED_GAME,
	    /* in game, locked                   */ ICON_STARTED_GAME_LOCKED,
	    /* in game, locked, passworded       */ ICON_STARTED_GAME_LOCKED,
	    /* in game, locked, full             */ ICON_STARTED_GAME_LOCKED,
	    /* in game, locked, full, passworded */ ICON_STARTED_GAME_LOCKED,
	};
	return icons[idx];
	// return ICON_GAME_UNKNOWN;
}

wxString IconImageList::GetBattleStatus(const IBattle& battle) const
{
	unsigned idx = battle.GetInGame() << 3 | battle.IsLocked() << 2 | battle.IsFull() << 1 | battle.IsPassworded() << 0;
	static const wxString states[16] = {
	    /* -                                 */ _("Game is open."),
	    /* passworded                        */ _("Game is password-protected."),
	    /* full                              */ _("Game is full."),
	    /* full, passworded                  */ _("Game is full and password-protected."),
	    /* locked                            */ _("Game is closed."),
	    /* locked, passworded                */ _("Game is closed and password-protected."),
	    /* locked, full                      */ _("Game is closed and full."),
	    /* locked, full, passworded          */ _("Game is closed, full and password-protected."),
	    /* in game                           */ _("Game is in progress."),
	    /* in game, passworded               */ _("Game is in progress and password-protected."),
	    /* in game, full                     */ _("Game is in progress and full."),
	    /* in game, full, passworded         */ _("Game is in progress, full and password-protected."),
	    /* in game, locked                   */ _("Game is in progress and closed."),
	    /* in game, locked, passworded       */ _("Game is in progress, closed and password-protected."),
	    /* in game, locked, full             */ _("Game is in progress, closed and full."),
	    /* in game, locked, full, passworded */ _("Game is in progress, closed, full and password-protected."),
	};
	return states[idx];
	// return _T("Game has unknown status");
}

wxString getColourIconKey(const wxColour& colour)
{
	return colour.GetAsString(wxC2S_HTML_SYNTAX).AfterFirst('#');
}

int IconImageList::GetColourIcon(const LSL::lslColor& colour) const
{
	const wxString key = getColourIconKey(lslTowxColour(colour));
	PlayerColourMap::const_iterator it = m_player_colour_icons.find(key);
	if (it != m_player_colour_icons.end() && it->second != 0)
		return it->second;
	else
		return -1;
}


int IconImageList::GetHostIcon(const bool& spectator) const
{
	return spectator ? ICON_HOST_SPECTATOR : ICON_HOST;
}


void IconImageList::SetColourIcon(const LSL::lslColor& colour)
{
	const wxString key = getColourIconKey(lslTowxColour(colour));
	if (m_player_colour_icons.find(key) == m_player_colour_icons.end())
		m_player_colour_icons[key] = Add(getColourIcon(lslTowxColour(colour)));
}


int IconImageList::GetSideIcon(const std::string& gamename, int side)
{
	const auto sides = LSL::usync().GetSides(gamename);
	std::string sidename;
	if (side < (int)sides.size()) {
		sidename = sides[side];
	}
	const std::string cachestring = gamename + "_" + sidename;
	if (m_cached_side_icons.find(cachestring) != m_cached_side_icons.end()) { //already cached
		return m_cached_side_icons[cachestring];
	}

	try {
		const LSL::UnitsyncImage img = LSL::usync().GetSidePicture(gamename, sidename);
		int IconPosition = Add(wxBitmap(img.wxbitmap()), wxNullBitmap);
		m_cached_side_icons[cachestring] = IconPosition;
		return IconPosition;
	} catch (...) {
	}
	if (side == 0) { //failed to load, store dummies in cache
		m_cached_side_icons[cachestring] = ICON_SIDEPIC_0;
	} else {
		m_cached_side_icons[cachestring] = ICON_SIDEPIC_1;
	}
	return m_cached_side_icons[cachestring];
}

int IconImageList::GetReadyIcon(const bool& spectator, const bool& ready, const unsigned int& sync, const bool& bot)
{
	int index;
	if (bot)
		index = ICON_BOT;
	else if (spectator)
		index = ICON_SPECTATOR;
	else if (ready)
		index = ICON_READY;
	else
		index = ICON_NREADY;

	if (sync == SYNC_SYNCED) //could this cause #674 ??
		return index;

	else {
		if (m_state_index_map.find(index) == m_state_index_map.end()) {
			wxBitmap bg;
			if (index == ICON_NREADY)
				bg = charArr2wxBitmap(closed_game_png, sizeof(closed_game_png)); // ICON_NREADY
			else if (index == ICON_READY)
				bg = charArr2wxBitmap(open_game_png, sizeof(open_game_png)); // ICON_READY
			else if (index == ICON_SPECTATOR)
				bg = charArr2wxBitmap(spectator_png, sizeof(spectator_png)); // ICON_SPECTATOR
			else
				bg = wxBitmap(bot_xpm); // ICON_BOT

			m_state_index_map[index] = Add(BlendBitmaps(bg, charArr2wxBitmap(warning_small_png, sizeof(warning_small_png))));
		}
		return m_state_index_map[index];
	}
}
