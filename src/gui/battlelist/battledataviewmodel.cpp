/*
 * battledataviewmodel.cpp
 *
 *  Created on: 28 июня 2015 г.
 *      Author: Руслан
 */

#include "battledataviewmodel.h"

#include "gui/iconscollection.h"
#include "ibattle.h"
#include "utils/slpaths.h"

BattleDataViewModel::BattleDataViewModel() {
	// TODO Auto-generated constructor stub

}

BattleDataViewModel::~BattleDataViewModel() {
	// TODO Auto-generated destructor stub
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
	//TODO: implement!
	return BaseDataViewModel::Compare(itemA, itemB, column, ascending);
}
