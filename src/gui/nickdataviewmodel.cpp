/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "nickdataviewmodel.h"

#include <wx/dataview.h>

#include "ibattle.h"
#include "iconscollection.h"
#include "user.h"
#include "useractions.h"

NickDataViewModel::NickDataViewModel()
    : BaseDataViewModel<User>::BaseDataViewModel(COLUMN_COUNT)
{
}

NickDataViewModel::~NickDataViewModel()
{
	// TODO Auto-generated destructor stub
}

void NickDataViewModel::GetValue(wxVariant& variant, const wxDataViewItem& item,
				 unsigned int col) const
{
	const User* user = static_cast<const User*>(item.GetID());
	IconsCollection* iconsCollection = IconsCollection::Instance();

	wxASSERT(user != nullptr);

	/* In case if wxGTK will try to render invalid item */
	if (user == nullptr || !ContainsItem(*user)) {
		switch (col) {
			case STATUS:
			case COUNTRY:
			case RANK:
				variant = wxVariant(iconsCollection->BMP_EMPTY);
				break;

			case NICKNAME:
			case LOBBYAGENT:
			case DEFAULT_COLUMN:
			default:
				variant = wxVariant(wxEmptyString);
		}
		return;
	}


	bool isBot = user->BattleStatus().IsBot();
	bool isBridged = user->IsBridged();

	switch (col) {
		case STATUS:
			variant = wxVariant(iconsCollection->GetUserListStateIcon(user->GetStatus(), false /*channel operator?*/, (user->GetBattle() != nullptr) /*in broom?*/));
			break;

		case COUNTRY:
			if (isBot || isBridged) {
				variant = wxVariant(iconsCollection->BMP_EMPTY);
			} else {
				variant = wxVariant(
				    iconsCollection->GetFlagBmp(wxString(user->GetCountry())));
			}
			break;

		case RANK:
			if (isBot || isBridged) {
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

				if (!user->BattleStatus().aiversion.empty()) {
					botName += _T(" ") + user->BattleStatus().aiversion;
				}
				variant = wxString::Format(_T("%s - %s (%s)"), playerName, botName,
							   ownerName);
			} else {
				variant = wxString(user->GetNick());
			}
			break;

		case LOBBYAGENT:
			variant = wxString(user->GetClientAgent());
			break;

		case DEFAULT_COLUMN:
			//Do nothing
			break;

		default:
			wxASSERT(false);
	}
}

int NickDataViewModel::Compare(const wxDataViewItem& itemA,
			       const wxDataViewItem& itemB, unsigned int column,
			       bool ascending) const
{
	const User* userA = static_cast<const User*>(itemA.GetID());
	const User* userB = static_cast<const User*>(itemB.GetID());

	wxASSERT(userA != nullptr);
	wxASSERT(userB != nullptr);

	int sortingResult;

	switch (column) {
		case STATUS: {
			int u1 = 0, u2 = 0;

			if (userA->IsBridged())
				u1 += 10000;
			if (userB->IsBridged())
				u2 -= 10000;
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
		} break;

		case COUNTRY:
			if (userA->IsBridged() && !userB->IsBridged()) {
				sortingResult = 1;			
			} else if (!userA->IsBridged() && userB->IsBridged()) {
				sortingResult = -1;
			} else if (userA->GetCountry() < userB->GetCountry()) {
				sortingResult = -1;
			} else if (userA->GetCountry() > userB->GetCountry()) {
				sortingResult = 1;
			} else {
				sortingResult = 0;
			}
			break;

		case RANK:
			if (userA->IsBridged() && !userB->IsBridged()) {
				sortingResult = 1;			
			} else if (!userA->IsBridged() && userB->IsBridged()) {
				sortingResult = -1;
			} else {
				sortingResult = (userA->GetRank() - userB->GetRank());
			}
			break;

		case NICKNAME:
			if (userA->IsBridged() && !userB->IsBridged()) {
				sortingResult = 1;			
			} else if (!userA->IsBridged() && userB->IsBridged()) {
				sortingResult = -1;
			} else {
				sortingResult = BaseDataViewModel::Compare(itemA, itemB, column, true);
			}
			break;

		case DEFAULT_COLUMN:
			sortingResult = 0;
			break;

		case LOBBYAGENT:
			sortingResult = BaseDataViewModel::Compare(itemA, itemB, column, true);
			break;

		default:
			wxASSERT(false);
			sortingResult = 0;
	}

	//Return direct sort order or reversed depending on ascending flag
	return ascending ? sortingResult : (sortingResult * (-1));
}

bool NickDataViewModel::GetAttr(const wxDataViewItem& item, unsigned int,
				wxDataViewItemAttr& attr) const
{
	const User* user = static_cast<const User*>(item.GetID());

	wxASSERT(user != nullptr);

	if (user == nullptr || !ContainsItem(*user)) {
		return false;
	}

	wxString groupName = useractions().GetGroupOfUser(user->GetNick());
	if (groupName == wxEmptyString) {
		return false;
	} else {
		wxColour color = useractions().GetGroupHLColor(groupName);
		attr.SetBackgroundColour(color);
		return true;
	}
}

wxString NickDataViewModel::GetColumnType(unsigned int column) const
{

	wxString colTypeString;

	switch (column) {
		case STATUS:
		case COUNTRY:
		case RANK:
			colTypeString = COL_TYPE_BITMAP;
			break;

		case NICKNAME:
		case LOBBYAGENT:
		case DEFAULT_COLUMN:
			colTypeString = COL_TYPE_TEXT;
			break;

		default:
			wxASSERT(false);
			colTypeString = COL_TYPE_TEXT;
	}


	return colTypeString;
}
