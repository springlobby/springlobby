/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "playbackdatamodel.h"

#include <wx/filename.h>
#include <wx/log.h>

#include "gui/iconscollection.h"
#include "storedgame.h"
#include "utils/conversion.h"
#include "utils/sortutil.h"

PlaybackDataModel::PlaybackDataModel()
    : BaseDataViewModel<StoredGame>::BaseDataViewModel(COLUMN_COUNT)
{
}

PlaybackDataModel::~PlaybackDataModel()
{
	// TODO Auto-generated destructor stub
}

int PlaybackDataModel::Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB,
			       unsigned int column, bool ascending) const
{
	int sortingResult = 0;
	const StoredGame* storedGameA = static_cast<const StoredGame*>(itemA.GetID());
	wxASSERT(storedGameA != nullptr);
	const StoredGame* storedGameB = static_cast<const StoredGame*>(itemB.GetID());
	wxASSERT(storedGameB != nullptr);

	switch (column) {
		case DEFAULT_COLUMN:
			ascending = false;
		case DATE:
			sortingResult = storedGameA->date_string.compare(storedGameB->date_string);
			break;
		case GAME:
			sortingResult = storedGameA->battle.GetHostGameNameAndVersion().compare(storedGameB->battle.GetHostGameNameAndVersion());
			break;
		case MAP:
			sortingResult = storedGameA->battle.GetHostMapName().compare(storedGameB->battle.GetHostMapName());
			break;
		case PLAYERS: {
			unsigned numPlayersA = storedGameA->battle.GetNumUsers() - storedGameA->battle.GetSpectators();
			unsigned numPlayersB = storedGameB->battle.GetNumUsers() - storedGameB->battle.GetSpectators();
			sortingResult = GenericCompare(numPlayersA, numPlayersB);
		} break;
		case DURATION:
			sortingResult = GenericCompare(storedGameA->duration, storedGameB->duration);
			break;
		case ENGINE:
			sortingResult = storedGameA->battle.GetEngineName().compare(storedGameB->battle.GetEngineName());
			if (0 == sortingResult)
				sortingResult = storedGameA->battle.GetEngineVersion().compare(storedGameB->battle.GetEngineVersion());
			break;
		case FILESIZE:
			sortingResult = GenericCompare(storedGameA->size, storedGameB->size);
			break;
		case FILENAME: {
			const std::string& fileNameA = storedGameA->battle.GetPlayBackFileName();
			const std::string& fileNameB = storedGameB->battle.GetPlayBackFileName();
			sortingResult = fileNameA.compare(fileNameB);
		} break;
		default:
			wxASSERT(false);
	}
	//Return direct sort order or reversed depending on ascending flag
	return ascending ? sortingResult : (sortingResult * (-1));
}

void PlaybackDataModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{

	const StoredGame* storedGame = static_cast<const StoredGame*>(item.GetID());

	if (storedGame == nullptr) {
		wxASSERT(false);
	}

	/* In case if wxGTK will try to render invalid item */
	if (storedGame == nullptr || !ContainsItem(*storedGame)) {
		switch (col) {
			case DATE:
				variant = wxVariant(wxDataViewIconText(wxEmptyString));
				break;

			case GAME:
			case MAP:
			case PLAYERS:
			case DURATION:
			case ENGINE:
			case FILESIZE:
			case FILENAME:
			case DEFAULT_COLUMN:
			default:
				variant = wxVariant(wxEmptyString);
		}
		return;
	}


	switch (col) {
		case DATE: {
			wxDataViewIconText iconText(TowxString(storedGame->date_string));
			IconsCollection* icons = IconsCollection::Instance();

			if (storedGame->duration == 0) {
				iconText.SetIcon(icons->ICON_NEXISTS);
			} else {
				iconText.SetIcon(icons->ICON_EXISTS);
			}
			variant = wxVariant(iconText);
		} break;

		case GAME:
			variant = TowxString(storedGame->battle.GetHostGameNameAndVersion());
			break;

		case MAP:
			variant = TowxString(storedGame->battle.GetHostMapName());
			break;

		case PLAYERS:
			variant = wxString::Format(_T("%d"), (int)storedGame->battle.GetNumUsers() - storedGame->battle.GetSpectators());
			break;

		case DURATION: {
			int seconds = storedGame->duration;
			int minutes = seconds / 60;
			int hours = minutes / 60;
			minutes = minutes % 60;
			seconds = seconds % 60;
			variant = wxString::Format(_T("%02d:%02d:%02d"), hours, minutes, seconds);
		} break;

		case ENGINE: {
			wxString engine(storedGame->battle.GetEngineName());
			engine += ' ';
			engine += storedGame->battle.GetEngineVersion();
			variant = engine;
		} break;

		case FILESIZE:
			variant = wxString::Format(_T("%d KB"), storedGame->size / 1024);
			break;

		case FILENAME:
			variant = TowxString(storedGame->battle.GetPlayBackFileName());
			break;

		case DEFAULT_COLUMN:
			//Do nothing
			break;

		default:
			wxASSERT(false);
	}
}

wxString PlaybackDataModel::GetColumnType(unsigned int columnt) const
{

	wxString colTypeString;

	switch (columnt) {
		case DATE:
			colTypeString = COL_TYPE_ICONTEXT;
			break;

		case GAME:
		case MAP:
		case PLAYERS:
		case DURATION:
		case ENGINE:
		case FILESIZE:
		case FILENAME:
		case DEFAULT_COLUMN:
			colTypeString = COL_TYPE_TEXT;
			break;

		default:
			wxASSERT(false);
			colTypeString = COL_TYPE_TEXT;
	}

	return colTypeString;
}
