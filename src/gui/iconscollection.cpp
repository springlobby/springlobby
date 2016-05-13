/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define HAVE_WX //needed for LSL::UnitsyncImage::wxbitmap!

#include "iconscollection.h"

#include "ibattle.h"
#include "user.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "log.h"
#include "lslunitsync/image.h"
#include "lslunitsync/unitsync.h"

#include "flagimagedata.h"

#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>

#include <map>

IconsCollection::IconsCollection()
{
	loadCountryFlags();
}

IconsCollection::~IconsCollection()
{
}

IconsCollection* IconsCollection::Instance()
{
	if (m_Instance == nullptr) {
		m_Instance = new IconsCollection();
	}
	return m_Instance;
}

void IconsCollection::Release()
{
	if (m_Instance != nullptr) {
		delete m_Instance;
	}
}

IconsCollection* IconsCollection::m_Instance = nullptr;

//Create collection populated with flags images
void IconsCollection::loadCountryFlags()
{
	int flagIndex = 0;
	m_countryFlagBmps.clear();
	for (flagIndex = 0; flag_str[flagIndex] != NULL; flagIndex++) {
		//Just in case (these two arrays must have same size!)
		wxASSERT(flag_xpm[flagIndex] != NULL);
		//Load flag image and store it in collection
		m_countryFlagBmps[flag_str[flagIndex]] = IconsCollection::CreateBitmap(flag_xpm[flagIndex]);
	}

	//Just in case (these two arrays must have same size!)
	wxASSERT((flag_str[flagIndex] == NULL) && (flag_xpm[flagIndex] == NULL));
}

wxBitmap& IconsCollection::GetHostBmp(bool isSpec)
{
	if (isSpec) {
		return BMP_HOST_SPECTATOR;
	} else {
		return BMP_HOST;
	}
}

wxBitmap& IconsCollection::GetReadyBmp(bool isSpec, bool isReady, unsigned int inSync,
				       bool isBot)
{

	if (isBot) {
		return BMP_BOT;
	}

	if (isSpec) {
		if (inSync == SYNC_SYNCED) {
			return BMP_SPECTATOR;
		} else {
			return BMP_SPECTATOR_UNSYNC;
		}
	}

	if (isReady) {
		if (inSync == SYNC_SYNCED) {
			return BMP_READY;
		} else {
			return BMP_READY_UNSYNC;
		}
	} else {
		if (inSync == SYNC_SYNCED) {
			return BMP_NREADY;
		} else {
			return BMP_NREADY_UNSYNC;
		}
	}
}

wxBitmap& IconsCollection::GetUserListStateIcon(const UserStatus& us, bool chanop, bool inbroom)
{
	if (us.bot) {
		if (us.in_game) {
			return BMP_BOT_INGAME;
		}
		if (inbroom) {
			return BMP_BOT_BROOM;
		}
		if (us.away) {
			return BMP_BOT_AWAY;
		}
		return BMP_BOT;
	} else if (us.moderator) {
		if (us.in_game) {
			return BMP_ADMIN_INGAME;
		}
		if (us.away) {
			return BMP_ADMIN_AWAY;
		}
		if (inbroom) {
			return BMP_ADMIN_BROOM;
		}
		return BMP_ADMIN;
	} else if (chanop) {
		if (us.in_game) {
			return BMP_OP_INGAME;
		}
		if (us.away) {
			return BMP_OP_AWAY;
		}
		if (inbroom) {
			return BMP_OP_BROOM;
		}
		return BMP_OP;
	}

	if (us.in_game) {
		return BMP_INGAME;
	}
	if (us.away) {
		return BMP_AWAY;
	}
	if (inbroom) {
		return BMP_BROOM;
	}

	return BMP_NOSTATE;
}

//Get flag image from collection
wxBitmap& IconsCollection::GetFlagBmp(const wxString& country)
{
	//Check for some predefined values
	if ((country.empty()) ||
	    (country == "??") || // unknown
	    (country == "XX") || // not sure where this come from, very likely from incomplete bootstrap at login
	    (country == "A1") || // anonymous proxy
	    (country == "A2") || // satellite provider
	    (country == "O1"))   // other country
		return BMP_UNK_FLAG;

	//Search for flag by country's name
	std::map<wxString, wxBitmap>::iterator itor = m_countryFlagBmps.find(country);
	//Return flag image if found
	if (itor != m_countryFlagBmps.end()) {
		return itor->second;
		//Return empty image otherwise
	} else {
		//Just return nothing. I think there is no need for triggering assert
		return BMP_UNK_FLAG;
	}
}

wxBitmap& IconsCollection::GetRankBmp(unsigned int rank, bool showLowest)
{
	if ((!showLowest) && (rank == UserStatus::RANK_1)) {
		return BMP_RANK_NONE;
	}

	switch (rank) {
		case UserStatus::RANK_1:
			return BMP_RANK1;
		case UserStatus::RANK_2:
			return BMP_RANK2;
		case UserStatus::RANK_3:
			return BMP_RANK3;
		case UserStatus::RANK_4:
			return BMP_RANK4;
		case UserStatus::RANK_5:
			return BMP_RANK5;
		case UserStatus::RANK_6:
			return BMP_RANK6;
		case UserStatus::RANK_7:
			return BMP_RANK7;
		case UserStatus::RANK_8:
			return BMP_RANK8;
		default:
			return BMP_RANK_UNKNOWN;
	}
}

wxBitmap& IconsCollection::GetBattleStatusBmp(const IBattle& battle) const
{
	unsigned idx = battle.GetInGame() << 3 | battle.IsLocked() << 2 | battle.IsFull() << 1 | battle.IsPassworded() << 0;

	return *battleStatuses[idx];
	// return ICON_GAME_UNKNOWN;
}

wxBitmap& IconsCollection::GetColourBmp(const LSL::lslColor& colour)
{

	const wxString key = lslTowxColour(colour).GetAsString(wxC2S_HTML_SYNTAX).AfterFirst('#');

	//Search needed colour in collection (cache) and return it if found
	std::map<wxString, wxBitmap>::iterator itor = m_playerColorBmps.find(key);
	if (itor != m_playerColorBmps.end()) {
		return itor->second;
		//Or add new colour to collection
	} else {
		m_playerColorBmps[key] = getColourIcon(lslTowxColour(colour));
		return m_playerColorBmps[key];
	}
}

wxBitmap& IconsCollection::GetFractionBmp(const std::string& gameName, size_t fractionId)
{

	if (gameName.empty() || !LSL::usync().GameExists(gameName)) {
		wxLogWarning("SideIcon %d for game %s not found!", fractionId, gameName.c_str());
		// game doesn't exist, dl needed?!
		return BMP_EMPTY;
	}

	const auto sides = LSL::usync().GetSides(gameName);

	//This can happen whenever in time, so must be caught in release build too
	if (sides.empty()) {
		wxLogWarning("IconsCollection::GetFractionBmp(): sides.empty()");
		return BMP_EMPTY;
	}

	if (fractionId >= sides.size()) {
		wxLogWarning("Invalid side requested: %s:%d", gameName.c_str(), fractionId);
		return BMP_EMPTY;
	}

	std::string sideName;

	sideName = sides[fractionId];

	const std::string cacheString = gameName + "_" + sideName;

	//Check if image already in cache
	if (m_cachedFractionBmps.find(cacheString) != m_cachedFractionBmps.end()) {
		return m_cachedFractionBmps[cacheString];
		//Create one and add to cache
	} else {
		try {
			const LSL::UnitsyncImage img = LSL::usync().GetSidePicture(gameName, sideName);
			m_cachedFractionBmps[cacheString] = img.wxbitmap();
		} catch (...) {
			//unitsync can fail!
			ASSERT_LOGIC(false, "LSL::usync().GetSidePicture() failed!");
		}
		return m_cachedFractionBmps[cacheString];
	}
}

/* Used to create transparent bitmaps under Windows 7 and Windows XP */
wxBitmap IconsCollection::CreateBitmap(const char* const* data)
{
	wxImage img = wxImage(data);
	if (!img.HasAlpha()) {
		img.InitAlpha();
	}
	return wxBitmap(img);
}

wxBitmap& IconsCollection::GetUserBattleStateBmp(const UserStatus& us)
{
	if (us.bot) {
		return BMP_BOT;
	}
	if (us.moderator) {
		if (us.in_game) {
			return BMP_ADMIN_INGAME;
		}
		if (us.away) {
			return BMP_ADMIN_AWAY;
		}
		return BMP_ADMIN;
	}

	if (us.in_game) {
		return BMP_INGAME;
	}
	if (us.away) {
		return BMP_AWAY;
	}

	return BMP_CHANNEL_OPTIONS;
}

wxBitmap& IconsCollection::GetUserListStateBmp(const UserStatus& us,
					       bool chanop, bool inbroom)
{
	if (us.bot) {
		if (us.in_game)
			return BMP_BOT_INGAME;
		if (inbroom)
			return BMP_BOT_BROOM;
		if (us.away)
			return BMP_BOT_AWAY;
		return BMP_BOT;
	}
	if (us.moderator) {
		if (us.in_game)
			return BMP_ADMIN_INGAME;
		if (us.away)
			return BMP_ADMIN_AWAY;
		if (inbroom)
			return BMP_ADMIN_BROOM;
		return BMP_ADMIN;
	}
	if (chanop) {
		if (us.in_game)
			return BMP_OP_INGAME;
		if (us.away)
			return BMP_OP_AWAY;
		if (inbroom)
			return BMP_OP_BROOM;
		return BMP_OP;
	}

	if (us.in_game)
		return BMP_INGAME;
	if (us.away)
		return BMP_AWAY;
	if (inbroom)
		return BMP_BROOM;

	return BMP_NOSTATE;
}
