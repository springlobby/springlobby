/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "battleroomdataviewmodel.h"

#include "gui/iconscollection.h"
#include "user.h"
#include "battle.h"
#include "useractions.h"

#include <wx/dataview.h>

BattleroomDataViewModel::BattleroomDataViewModel() {
	m_Battle = nullptr;
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
			if (GetBattle()->IsFounder(*user)) {
				variant = wxVariant(iconsCollection->GetHostBmp(isSpectator));
			} else {
				variant = wxVariant(iconsCollection->GetReadyBmp(isSpectator, user->BattleStatus().ready, user->BattleStatus().sync, isBot));
			}
		}
		break;

	case INGAME:
		variant = wxVariant(iconsCollection->GetUserListStateIcon(user->GetStatus(), false /*channel operator?*/, (user->GetBattle() != nullptr) /*in broom?*/));
		break;

	case FACTION:
		if (isSpectator) {
			variant = wxVariant(iconsCollection->BMP_EMPTY);
		} else {
			variant = wxVariant(iconsCollection->GetFractionBmp(GetBattle()->GetHostModName(), user->BattleStatus().side));
		}
		break;

	case COLOUR:
		if (isSpectator) {
			variant = wxVariant(iconsCollection->BMP_EMPTY);
		} else {
			//TODO: implement!
			variant = wxVariant(iconsCollection->GetColourBmp(user->GetColor()));
		}
		break;

	case COUNTRY:
		if (isBot) {
			variant = wxVariant(iconsCollection->BMP_EMPTY);
		} else {
			variant = wxVariant(iconsCollection->GetFlagBmp(wxString(user->GetCountry())));
		}
		break;

	case RANK:
		if (isBot) {
			variant = wxVariant(iconsCollection->BMP_EMPTY);
		} else {
			variant = wxVariant(iconsCollection->GetRankBmp(user->GetRank()));
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

	case DEFAULT_COLUMN:
		//Do nothing
		break;

	default:
		wxASSERT(false);
	}
}

IBattle* BattleroomDataViewModel::GetBattle() const {
	wxASSERT(m_Battle != nullptr);

	return m_Battle;
}

void BattleroomDataViewModel::SetBattle(IBattle* battle) {
	m_Battle = battle;
}

int BattleroomDataViewModel::Compare(const wxDataViewItem& itemA,
		const wxDataViewItem& itemB, unsigned int column, bool ascending) const {

	const User* userA = static_cast<const User*>(itemA.GetID());
	const User* userB = static_cast<const User*>(itemB.GetID());

	wxASSERT(userA != nullptr);
	wxASSERT(userB != nullptr);

	int sortingResult;

	switch(column)
	{
	case STATUS:
	{
		int status1 = 0;
		if (userA->IsBot()) {
			status1 = 9;
		} else {
			if (userA->BattleStatus().ready)
				status1 += 5;
			if (userA->BattleStatus().sync)
				status1 += 7;
			if (userA->BattleStatus().spectator)
				status1 += 10;
		}

		int status2 = 0;
		if (userB->IsBot()) {
			status2 = 9;
		} else {
			if (userB->BattleStatus().ready)
				status2 += 5;
			if (userB->BattleStatus().sync)
				status2 += 7;
			if (userB->BattleStatus().spectator)
				status2 += 10;
		}

		if (status1 < status2) {
			sortingResult = -1;
		} else if (status1 > status2) {
			sortingResult = 1;
		} else {
			sortingResult = 0;
		}
	}
	break;

	case INGAME:
	{
		int u1 = 0, u2 = 0;

		if (userA->GetStatus().bot)
			u1 += 1000;
		if (userB->GetStatus().bot)
			u2 += 1000;
		if (userA->GetStatus().moderator)
			u1 += 100;
		if (userB->GetStatus().moderator)
			u2 += 100;
		if (userA->GetStatus().in_game)
			u1 += -10;
		if (userB->GetStatus().in_game)
			u2 += -10;
		if (userA->GetStatus().away)
			u1 += -5;
		if (userB->GetStatus().away)
			u2 += -5;

		// inverse the order
		if (u1 < u2) {
			sortingResult = -1;
		} else if (u1 > u2) {
			sortingResult = 1;
		} else {
			sortingResult = 0;
		}
	}
	break;

	case FACTION:
		if (userA->BattleStatus().spectator && (userB->BattleStatus().spectator == false)) {
			return 1;
		} else if(userB->BattleStatus().spectator && (userA->BattleStatus().spectator == false)) {
			return -1;
		} else if (userA->BattleStatus().spectator && userB->BattleStatus().spectator) {
			return 0;
		} else {
			sortingResult = (userA->BattleStatus().side - userB->BattleStatus().side);
		}
		break;

	case COLOUR:
	{
		int color1_r, color1_g, color1_b;

		if (userA->BattleStatus().spectator) {
			sortingResult = -1;
			break;
		}
		color1_r = userA->BattleStatus().colour.Red();
		color1_g = userA->BattleStatus().colour.Green();
		color1_b = userA->BattleStatus().colour.Blue();

		int color2_r, color2_g, color2_b;

		if (userB->BattleStatus().spectator) {
			sortingResult = 1;
			break;
		}
		color2_r = userB->BattleStatus().colour.Red();
		color2_g = userB->BattleStatus().colour.Green();
		color2_b = userB->BattleStatus().colour.Blue();

		if ((color1_r + color1_g + color1_b) / 3 < (color2_r + color2_g + color2_b) / 3) {
			sortingResult = -1;
		} else if ((color1_r + color1_g + color1_b) / 3 > (color2_r + color2_g + color2_b) / 3) {
			sortingResult = 1;
		} else {
			sortingResult = 0;
		}
	}
	break;

	case COUNTRY:
		if (userA->GetCountry() < userB->GetCountry()) {
			sortingResult = -1;
		} else if(userA->GetCountry() > userB->GetCountry()) {
			sortingResult = 1;
		} else {
			sortingResult = 0;
		}
		break;

	case RANK:
		sortingResult = (userA->GetRank() - userB->GetRank());
		break;

	case NICKNAME:
		sortingResult = BaseDataViewModel::Compare(itemA, itemB, column, true);
		break;

	case TRUESKILL:
		sortingResult =(userA->GetTrueSkill() - userB->GetTrueSkill());
		break;

	case TEAM:
		if (userA->BattleStatus().spectator && (userB->BattleStatus().spectator == false)) {
			return 1;
		} else if(userB->BattleStatus().spectator && (userA->BattleStatus().spectator == false)) {
			return -1;
		} else if (userA->BattleStatus().spectator && userB->BattleStatus().spectator) {
			return 0;
		} else {
			sortingResult = (userA->BattleStatus().team - userB->BattleStatus().team);
		}
		break;

	case ALLY:
		if (userA->BattleStatus().spectator && (userB->BattleStatus().spectator == false)) {
			return 1;
		} else if(userB->BattleStatus().spectator && (userA->BattleStatus().spectator == false)) {
			return -1;
		} else if (userA->BattleStatus().spectator && userB->BattleStatus().spectator) {
			return 0;
		} else {
			sortingResult = (userA->BattleStatus().ally - userB->BattleStatus().ally);
		}
		break;

	case BONUS:
		if (userA->BattleStatus().spectator && (userB->BattleStatus().spectator == false)) {
			return 1;
		} else if(userB->BattleStatus().spectator && (userA->BattleStatus().spectator == false)) {
			return -1;
		} else if (userA->BattleStatus().spectator && userB->BattleStatus().spectator) {
			return 0;
		} else {
			sortingResult = (userA->BattleStatus().handicap - userB->BattleStatus().handicap);
		}
		break;

	case DEFAULT_COLUMN:
		sortingResult = 0;
		break;

	default:
		wxASSERT(false);
		sortingResult = 0;
	}

	//Return direct sort order or reversed depending on ascending flag
	return ascending ? sortingResult : (sortingResult * (-1));
}

bool BattleroomDataViewModel::GetAttr(const wxDataViewItem& item,
		unsigned int, wxDataViewItemAttr& attr) const {
	const User* user = static_cast<const User*>(item.GetID());

	wxASSERT(user != nullptr);

	wxString groupName = useractions().GetGroupOfUser(user->GetNick());
	if (groupName == wxEmptyString) {
		return false;
	} else {
		wxColour color = useractions().GetGroupHLColor(groupName);
		attr.SetBackgroundColour(color);
		return true;
	}
}
