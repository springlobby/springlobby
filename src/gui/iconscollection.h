/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_ICONSCOLLECTION_H_
#define SRC_GUI_ICONSCOLLECTION_H_

#include <wx/icon.h>
#include <wx/bitmap.h>
#include <wx/image.h>

#include <map>

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

/*
#include "images/major.xpm"
#include "images/minor.xpm"
*/

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

class UserStatus;
class wxString;

namespace LSL
{
	class lslColor;
}

class IconsCollection {
private:
	IconsCollection();
	virtual ~IconsCollection();

public:
	static IconsCollection* Instance();
	static void Release();

private:
	void loadCountryFlags();
	
public:
	wxBitmap& GetHostBmp(bool isSpec);
	wxBitmap& GetReadyBmp(bool isSpec, bool isReady, bool inSync, bool isBot);
	wxBitmap& GetUserListStateIcon(const UserStatus& us, bool chanop, bool inbroom);
	wxBitmap& GetFlagBmp(wxString& country);
	wxBitmap& GetRankBmp(unsigned int rank, bool showLowest = true);
	wxBitmap& GetColourBmp(LSL::lslColor& colour);
	wxBitmap& GetFractionBmp(const std::string& modName, int fractionId);

private:
	static IconsCollection* m_Instance;
	std::map<std::string, wxBitmap> m_cachedFractionBmps;
	std::map<wxString, wxBitmap> m_countryFlagBmps;
	
public:
	wxIcon ICON_EXISTS = wxIcon(exists_xpm);
	wxIcon ICON_NEXISTS = wxIcon(nexists_xpm);

public:
	wxBitmap BMP_ADMIN = charArr2wxBitmap(admin_png, sizeof(admin_png));
	wxBitmap BMP_ADMIN_AWAY = charArr2wxBitmap(admin_away_png, sizeof(admin_away_png));
	wxBitmap BMP_ADMIN_BROOM = charArr2wxBitmap(admin_broom_png, sizeof(admin_broom_png));
	wxBitmap BMP_ADMIN_INGAME = charArr2wxBitmap(admin_ingame_png, sizeof(admin_ingame_png));

	wxBitmap BMP_PLAYER = wxBitmap(player_xpm);

	wxBitmap BMP_BOT = wxBitmap(bot_xpm);
	wxBitmap BMP_BOT_BROOM = charArr2wxBitmap(bot_broom_png, sizeof(bot_broom_png));
	wxBitmap BMP_BOT_INGAME = charArr2wxBitmap(bot_ingame_png, sizeof(bot_ingame_png));
	wxBitmap BMP_BOT_AWAY = wxBitmap(bot_away_xpm);

	wxBitmap BMP_NOSTATE = wxBitmap(empty_xpm);
	wxBitmap BMP_AWAY = charArr2wxBitmap(away_png, sizeof(away_png));
	wxBitmap BMP_BROOM = charArr2wxBitmap(broom_png, sizeof(broom_png));
	wxBitmap BMP_INGAME = charArr2wxBitmap(ingame_png, sizeof(ingame_png));

	wxBitmap BMP_OP = wxBitmap(chanop_xpm);
	wxBitmap BMP_OP_AWAY = wxBitmap(chanop_away_xpm);
	wxBitmap BMP_OP_BROOM = wxBitmap(chanop_broom_xpm);
	wxBitmap BMP_OP_INGAME = wxBitmap(chanop_ingame_xpm);

	wxBitmap BMP_UP = wxBitmap(up_xpm);
	wxBitmap BMP_DOWN = wxBitmap(down_xpm);

	wxBitmap BMP_RANK_NONE = wxBitmap(empty_xpm);
	wxBitmap BMP_RANK_UNKNOWN = wxBitmap(rank_unknown_xpm);
	wxBitmap BMP_RANK1 = wxBitmap(rank0_xpm);
	wxBitmap BMP_RANK2 = wxBitmap(rank1_xpm);
	wxBitmap BMP_RANK3 = wxBitmap(rank2_xpm);
	wxBitmap BMP_RANK4 = wxBitmap(rank3_xpm);
	wxBitmap BMP_RANK5 = wxBitmap(rank4_xpm);
	wxBitmap BMP_RANK6 = wxBitmap(rank5_xpm);
	wxBitmap BMP_RANK7 = wxBitmap(rank6_xpm);
	wxBitmap BMP_RANK8 = wxBitmap(rank7_xpm);

	wxBitmap BMP_GAME_UNKNOWN = wxBitmap();
	wxBitmap BMP_OPEN_GAME = charArr2wxBitmap(open_game_png, sizeof(open_game_png));
	wxBitmap BMP_OPEN_PW_GAME = charArr2wxBitmap(open_pw_game_png, sizeof(open_pw_game_png));
	wxBitmap BMP_OPEN_FULL_PW_GAME = charArr2wxBitmap(open_full_pw_game_png, sizeof(open_full_pw_game_png));
	wxBitmap BMP_OPEN_FULL_GAME = charArr2wxBitmap(open_full_game_png, sizeof(open_full_game_png));
	wxBitmap BMP_CLOSED_GAME = charArr2wxBitmap(closed_game_png, sizeof(closed_game_png));
	wxBitmap BMP_CLOSED_PW_GAME = charArr2wxBitmap(closed_pw_game_png, sizeof(closed_pw_game_png));
	wxBitmap BMP_CLOSED_FULL_PW_GAME = charArr2wxBitmap(closed_full_pw_game_png, sizeof(closed_full_pw_game_png));
	wxBitmap BMP_CLOSED_FULL_GAME = charArr2wxBitmap(closed_full_game_png, sizeof(closed_full_game_png));
	wxBitmap BMP_STARTED_GAME = wxBitmap();
	wxBitmap BMP_STARTED_GAME_LOCKED = wxBitmap();

	wxBitmap BMP_READY_UNSYNC = wxBitmap(ready_unsync_xpm);
	wxBitmap BMP_NREADY_UNSYNC = wxBitmap(nready_unsync_xpm);
	wxBitmap BMP_READY_QSYNC = wxBitmap(ready_q_xpm);
	wxBitmap BMP_NREADY_QSYNC = wxBitmap(nready_q_xpm);
	wxBitmap BMP_NREADY = charArr2wxBitmap(closed_game_png, sizeof(closed_game_png));
	wxBitmap BMP_READY = charArr2wxBitmap(open_game_png, sizeof(open_game_png));

	wxBitmap BMP_NEXISTS = wxBitmap(nexists_xpm);
	wxBitmap BMP_EXISTS = wxBitmap(exists_xpm);

	wxBitmap BMP_SPECTATOR = charArr2wxBitmap(spectator_png, sizeof(spectator_png));
	wxBitmap BMP_SPECTATOR_UNSYNC = charArr2wxBitmapWithBlending(spectator_png, sizeof(spectator_png), warning_small_png, sizeof(warning_small_png));
	wxBitmap BMP_HOST = wxBitmap(host_xpm);
	wxBitmap BMP_HOST_SPECTATOR = wxBitmap(host_spectator_xpm);

	wxBitmap BMP_SIDEPIC_0 = wxBitmap();
	wxBitmap BMP_SIDEPIC_1 = wxBitmap();

	wxBitmap BMP_UNK_FLAG = wxBitmap(unknown_flag_xpm);
	wxBitmap BMP_FLAGS_BASE = wxBitmap();

	wxBitmap BMP_WARNING_OVERLAY = wxBitmap();

	wxBitmap BMP_CHANNEL_OPTIONS = wxBitmap(channel_options_xpm);

	wxBitmap BMP_EMPTY = wxBitmap(empty_xpm);

	wxBitmap BMP_SPRINGLOBBY = wxBitmap(springlobby_xpm);
};

#endif /* SRC_GUI_ICONSCOLLECTION_H_ */
