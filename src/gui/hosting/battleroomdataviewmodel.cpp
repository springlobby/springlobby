/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "battleroomdataviewmodel.h"

#include "gui/iconscollection.h"
#include "user.h"
#include "battle.h"

#include <wx/dataview.h>

BattleroomDataViewModel::BattleroomDataViewModel() {
	// TODO Auto-generated constructor stub

}

BattleroomDataViewModel::~BattleroomDataViewModel() {
	// TODO Auto-generated destructor stub
}

void BattleroomDataViewModel::GetValue(wxVariant& variant,
		const wxDataViewItem& item, unsigned int col) const {

	const User* user = static_cast<const User*>(item.GetID());
	IconsCollection* iconsCollection = IconsCollection::Instance();

	wxASSERT(user != nullptr);

	bool isSpectator = user->BattleStatus().spectator;
	bool isBot = user->BattleStatus().IsBot();

	switch(col)
	{
	case STATUS:
		if (isBot) {
			variant = wxVariant(iconsCollection->BMP_BOT);
		} else {
			variant = wxVariant(iconsCollection->GetReadyBmp(isSpectator, user->BattleStatus().ready, user->BattleStatus().sync, isBot));
		}
		break;

	case INGAME:
		variant = wxVariant(iconsCollection->GetUserListStateIcon(user->GetStatus(), false /*channel operator?*/, (user->GetBattle() != nullptr) /*in broom?*/));
		break;

	case FACTION:
		if (isSpectator) {
			return iconsCollection->BMP_EMPTY;
		}
		return iconsCollection->BMP_EMPTY;
		break;

	case COLOUR:
		if (isSpectator) {
			return iconsCollection->BMP_EMPTY;
		}
		//TODO: implement!
		return iconsCollection->BMP_EMPTY;
		break;

	case COUNTRY:
		if (isBot) {
			return iconsCollection->BMP_EMPTY;
		}
		//TODO: implement!
		return iconsCollection->BMP_EMPTY;
		break;

	case RANK:
		if (isBot) {
			return iconsCollection->BMP_EMPTY;
		} else {
			return iconsCollection->GetRankBmp(user->GetStatus().rank);
		}
		break;

	case NICKNAME:
		if (isBot) {
			wxString botName = wxString(user->BattleStatus().aishortname);
			wxString ownerName = wxString(user->BattleStatus().owner);
			wxString playerName = wxString(user->GetNick());

			if (user->BattleStatus().aiversion.empty() == false) {
				botName += _T(" ") + user->BattleStatus().aiversion;
			}
			variant = wxString::Format(_T("%s - %s (%s)"),playerName, botName, ownerName);
		} else {
			variant = wxString(user->GetNick());
		}
		break;

	case TRUESKILL:
		if (user->GetTrueSkill() == 0) {
			variant = wxString(_T("-"));
		} else {
			variant = wxString::Format(_T("%d"), static_cast<int>(user->GetTrueSkill()));
		}
		break;

	case TEAM:
		if (isSpectator) {
			variant = wxEmptyString;
		} else {
			variant = wxString::Format(_T("%d"), user->BattleStatus().team + 1);
		}
		break;

	case ALLY:
		if (isSpectator) {
			variant = wxEmptyString;
		} else {
			variant = wxString::Format(_T("%d"), user->BattleStatus().ally + 1);
		}
		break;

	case BONUS:
		if (isSpectator) {
			variant = wxEmptyString;
		} else {
			variant = wxString::Format(_T("%d%%"), user->BattleStatus().handicap);
		}
		break;

	default:
		wxASSERT(false);
	}
}
