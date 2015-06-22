/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define HAVE_WX	//needed for LSL::UnitsyncImage::wxbitmap!

#include "iconscollection.h"

#include "user.h"
#include "utils/conversion.h"
#include "log.h"
#include "lslunitsync/image.h"
#include "lslunitsync/unitsync.h"

#include <wx/icon.h>
#include <wx/image.h>
#include <wx/bitmap.h>

IconsCollection::IconsCollection() {
}

IconsCollection::~IconsCollection() {
}

IconsCollection* IconsCollection::Instance() {
	if (m_Instance == nullptr) {
		m_Instance = new IconsCollection();
	}
	return m_Instance;
}

void IconsCollection::Release() {
	if (m_Instance != nullptr) {
		delete m_Instance;
	}
}

IconsCollection* IconsCollection::m_Instance = nullptr;

wxBitmap& IconsCollection::GetHostBmp(bool isSpec) {
	if (isSpec) {
		return BMP_HOST_SPECTATOR;
	} else {
		return BMP_HOST;
	}
}

wxBitmap& IconsCollection::GetReadyBmp(bool isSpec, bool isReady, bool inSync,
		bool isBot) {

	if (isBot) {
		return BMP_BOT;
	}

	if (isSpec) {
		if (inSync) {
			return BMP_SPECTATOR;
		} else {
			return BMP_SPECTATOR_UNSYNC;
		}
	}

	if (isReady) {
		if (inSync) {
			return BMP_READY;
		} else {
			return BMP_READY_UNSYNC;
		}
	} else {
		if (inSync) {
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

wxBitmap& IconsCollection::GetFlagBmp(wxString& country) {
	//FIXME
	return BMP_ADMIN;
}

wxBitmap& IconsCollection::GetRankBmp(unsigned int rank, bool showLowest) {
	if ( (showLowest == false) && (rank == UserStatus::RANK_1) ) {
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

wxBitmap& IconsCollection::GetColourBmp(LSL::lslColor& colour) {
	//FIXME
	return BMP_ADMIN;
}

wxBitmap& IconsCollection::GetFractionBmp(const std::string& modName, int fractionId) {

	wxASSERT(-1 < fractionId);
	wxASSERT(modName.empty() == false);

	const auto sides = LSL::usync().GetSides(modName);

	//This can happen whenever in time, so must be caught in release build too
	ASSERT_LOGIC(sides.empty() == false, "LSL::usync().GetSides() failed!");
	ASSERT_LOGIC(fractionId < static_cast<int>(sides.size()), "LSL::usync().GetSides() < fractionID!");

	std::string sideName;

	sideName = sides[fractionId];

	const std::string cacheString = modName + "_" + sideName;

	//Check if image already in cache
	if (m_cachedFractionBmps.find(cacheString) != m_cachedFractionBmps.end()) {
		return m_cachedFractionBmps[cacheString];
	//Create one and add to cache
	} else {
		try {
			const LSL::UnitsyncImage img = LSL::usync().GetSidePicture(modName, sideName);
			m_cachedFractionBmps[cacheString] = img.wxbitmap();
		} catch (...) {
		//unitsync can fail!
			ASSERT_LOGIC(false, "LSL::usync().GetSidePicture() failed!");
		}
		return m_cachedFractionBmps[cacheString];
	}
}
