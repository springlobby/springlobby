/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "battledataviewmodel.h"

#include "gui/iconscollection.h"
#include "ibattle.h"
#include "utils/slpaths.h"

BattleDataViewModel::BattleDataViewModel() {
}

BattleDataViewModel::~BattleDataViewModel() {
}

void BattleDataViewModel::GetValue(wxVariant& variant,
		const wxDataViewItem& item, unsigned int col) const {

	const IBattle* battle = static_cast<const IBattle*>(item.GetID());

	wxASSERT(battle != nullptr);

	IconsCollection* iconsCollection = IconsCollection::Instance();
	const BattleOptions& opts = battle->GetBattleOptions();

	switch (col) {
	case STATUS:
		variant = wxVariant(iconsCollection->GetBattleStatusBmp(*battle));
		break;

	case COUNTRY:
		variant = wxVariant(iconsCollection->GetFlagBmp(battle->GetFounder().GetCountry()));
		break;

	case RANK:
		variant = wxVariant(iconsCollection->GetRankBmp(battle->GetRankNeeded(), false));
		break;

	case DESCRIPTION:
		variant = wxVariant(wxString(opts.description));
		break;

	case MAP:
		variant = wxVariant(wxDataViewIconText(wxString(battle->GetHostMapName()), battle->MapExists(false) ? iconsCollection->ICON_EXISTS : iconsCollection->ICON_NEXISTS));
		break;

	case GAME:
		variant = wxVariant(wxDataViewIconText(wxString(battle->GetHostModName()), battle->ModExists(false) ? iconsCollection->ICON_EXISTS : iconsCollection->ICON_NEXISTS));
		break;

	case HOST:
		variant = wxVariant(wxString(opts.founder));
		break;

	case SPECTATORS:
		variant = wxVariant(wxString::Format(_T("%d"), battle->GetSpectators()));
		break;

	case PLAYERS:
		variant = wxVariant(wxString::Format(_T("%d"), (static_cast<int>(battle->GetNumUsers()) - battle->GetSpectators())));
		break;

	case MAXIMUM:
		variant = wxVariant(wxString::Format(_T("%d"), static_cast<int>(battle->GetMaxPlayers())));
		break;

	case RUNNING:
		variant = wxVariant((wxTimeSpan(0 /*h*/, 0 /*m*/,
				   battle->GetBattleRunningTime()).Format(_T("%H:%M"))));
		break;

	case ENGINE:
		variant = wxVariant(wxDataViewIconText(wxString(battle->GetEngineVersion()),
				SlPaths::GetCompatibleVersion(battle->GetEngineVersion()).empty() ? iconsCollection->ICON_NEXISTS : iconsCollection->ICON_EXISTS));
		break;

	default:
		wxASSERT(false);
	}
}

int BattleDataViewModel::Compare(const wxDataViewItem& itemA,
		const wxDataViewItem& itemB, unsigned int column,
		bool ascending) const {

	const IBattle* battleA = static_cast<const IBattle*>(itemA.GetID());
	const IBattle* battleB = static_cast<const IBattle*>(itemB.GetID());

	wxASSERT(battleA != nullptr);
	wxASSERT(battleB != nullptr);

	int sortingResult;

	switch (column) {
	case STATUS:
	{
		int b1 = 0, b2 = 0;

		if (battleA->GetNumActivePlayers() == 0)
			b1 += 2000;
		if (battleB->GetNumActivePlayers() == 0)
			b2 += 2000;
		if (battleA->GetInGame())
			b1 += 1000;
		if (battleB->GetInGame())
			b2 += 1000;
		if (battleA->IsLocked())
			b1 += 100;
		if (battleB->IsLocked())
			b2 += 100;
		if (battleA->IsPassworded())
			b1 += 50;
		if (battleB->IsPassworded())
			b2 += 50;
		if (battleA->IsFull())
			b1 += 25;
		if (battleB->IsFull())
			b2 += 25;

		// inverse the order
		if (b1 < b2) {
			sortingResult = -1;
		} else if (b1 > b2) {
			sortingResult = 1;
		} else {
			sortingResult = 0;
		}
	}
		break;

	case COUNTRY:
		if (battleA->GetFounder().GetCountry() < battleB->GetFounder().GetCountry()) {
			sortingResult = -1;
		} else if (battleA->GetFounder().GetCountry() > battleB->GetFounder().GetCountry()) {
			sortingResult = 1;
		} else {
			sortingResult = 0;
		}
		break;

	case RANK:
		sortingResult = (battleA->GetRankNeeded() - battleB->GetRankNeeded());
		break;

	case DESCRIPTION:
	case MAP:
	case GAME:
	case HOST:
	case ENGINE:
	case RUNNING:
		return BaseDataViewModel::Compare(itemA, itemB, column, ascending);
		break;

	case SPECTATORS:
		sortingResult = battleA->GetSpectators() - battleB->GetSpectators();
		break;

	case MAXIMUM:
		sortingResult = battleA->GetMaxPlayers() - battleB->GetMaxPlayers();
		break;

	case PLAYERS:
	{
		int playersA = battleA->GetNumPlayers() - battleA->GetSpectators();
		int playersB = battleB->GetNumPlayers() - battleB->GetSpectators();
		sortingResult = playersA - playersB;
	}
	break;

	default:
		wxASSERT(false);
		sortingResult = 0;
	}

	//Return direct sort order or reversed depending on ascending flag
	return ascending ? sortingResult : (sortingResult * (-1));
}
