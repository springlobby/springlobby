/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "playbackdatamodel.h"

#include "storedgame.h"
#include "gui/iconscollection.h"

#include <wx/filename.h>
#include <wx/log.h>
#include "utils/conversion.h"

PlaybackDataModel::PlaybackDataModel()
    : BaseDataViewModel<StoredGame>::BaseDataViewModel(COLUMN_COUNT)
{
}

PlaybackDataModel::~PlaybackDataModel()
{
	// TODO Auto-generated destructor stub
}

void PlaybackDataModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{

	const StoredGame* storedGame = static_cast<const StoredGame*>(item.GetID());

	if (storedGame == nullptr) {
		wxASSERT(false);
	}

	/* In case if wxGTK will try to render invalid item */
	if (storedGame == nullptr || ContainsItem(*storedGame) == false) {
		switch (col) {
			case DATE:
				variant = wxVariant(wxDataViewIconText(wxEmptyString));
				break;

			case GAME:
			case MAP:
			case PLAYERS:
			case DURATION:
			case VERSION:
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
			variant = TowxString(storedGame->battle.GetHostGameName());
			break;

		case MAP:
			variant = TowxString(storedGame->battle.GetHostMapName());
			break;

		case PLAYERS:
			variant = wxString::Format(_T("%d"), (int)storedGame->battle.GetNumUsers() - storedGame->battle.GetSpectators());
			break;

		case DURATION:
			//FIXME: this seems to give incorrect result
			variant = wxString::Format(_T("%02ld:%02ld:%02ld"), (long)storedGame->duration / 3600, (long)(storedGame->duration % 3600) / 60, (long)(storedGame->duration % 60) / 60);
			break;

		case VERSION:
			variant = TowxString(storedGame->SpringVersion);
			break;

		case FILESIZE:
			variant = wxString::Format(_T("%d KB"), storedGame->size / 1024);
			break;

		case FILENAME:
			variant = TowxString(storedGame->Filename).AfterLast(wxFileName::GetPathSeparator());
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
		case VERSION:
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
