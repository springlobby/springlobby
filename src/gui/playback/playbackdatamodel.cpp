/*
 * PlaybackDataModel.cpp
 *
 *  Created on: 19 июня 2015 г.
 *      Author: Руслан
 */

#include "playbackdatamodel.h"

#include "storedgame.h"
#include "gui/iconscollection.h"

#include <wx/filename.h>

PlaybackDataModel::PlaybackDataModel() {
	// TODO Auto-generated constructor stub
}

PlaybackDataModel::~PlaybackDataModel() {
	// TODO Auto-generated destructor stub
}

void PlaybackDataModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const {

	const StoredGame* storedGame = static_cast<const StoredGame*>(item.GetID());

	if (storedGame == nullptr) {
		wxASSERT(false);
	}

	switch (col) {
	case DATE:
		{
			wxDataViewIconText iconText(wxString(storedGame->date_string));
			IconsCollection* icons = IconsCollection::Instance();

			if (storedGame->duration == 0) {
				iconText.SetIcon(icons->IconNExists());
			} else {
				iconText.SetIcon(icons->IconExists());
			}
			variant = wxVariant(iconText);
		}
		break;

	case GAME:
		variant = wxString(storedGame->battle.GetHostModName());
		break;

	case MAP:
		variant = wxString(storedGame->battle.GetHostMapName());
		break;

	case PLAYERS:
		variant = wxString::Format(_T("%d"), (int)storedGame->battle.GetNumUsers() - storedGame->battle.GetSpectators());
		break;

	case DURATION:
		//FIXME: this seems to give incorrect result
		variant = wxString::Format(_T("%02ld:%02ld:%02ld"), (long)storedGame->duration / 3600, (long)(storedGame->duration % 3600) / 60, (long)(storedGame->duration % 60) / 60);
		break;

	case VERSION:
		variant = wxString(storedGame->SpringVersion);
		break;

	case FILESIZE:
		variant = wxString::Format(_T("%d KB"), storedGame->size / 1024);
		break;

	case FILENAME:
		variant = wxString(storedGame->Filename).AfterLast(wxFileName::GetPathSeparator());
		break;

	default:
		wxASSERT(false);
	}
}

