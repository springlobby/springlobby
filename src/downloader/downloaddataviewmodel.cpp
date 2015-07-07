/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewmodel.h"
#include "downloadsobserver.h"
#include <wx/msgdlg.h>
DownloadDataViewModel::DownloadDataViewModel() {
}

DownloadDataViewModel::~DownloadDataViewModel() {
}

void DownloadDataViewModel::GetValue(wxVariant& variant,
		const wxDataViewItem& item, unsigned int column) const {

	ObserverDownloadInfo * downloadInfo = static_cast<ObserverDownloadInfo*>(item.GetID());

	wxASSERT(downloadInfo != nullptr);

	const int MB = 1024 * 1024;

	switch(column)
	{
	case NAME:
		variant = wxVariant(downloadInfo->name);
		break;

	case STATUS:
		if (downloadInfo->finished) {
			variant = wxVariant(wxString(_("complete")));
		} else {
			variant = wxVariant(wxString(_("downloading")));
		}
		break;

	case P_COMPLETE:
		if (downloadInfo->size < 1) { /* Prevent from division by zero */
			variant = wxVariant(wxString(_T("downloadInfo->size < 1")));
		} else {
			variant = wxVariant(wxString::Format(wxT("%i%%"), (int)((double)100.0 * downloadInfo->progress / (double)downloadInfo->size)));
		}
		break;

	case SPEED:
		//TODO: implement
		variant = wxVariant(wxEmptyString);
		break;

	case ETA:
		//TODO: implement
		variant = wxVariant(wxEmptyString);
		break;

	case FILESIZE:
		variant = wxVariant(downloadInfo->size > 0 ? wxString::Format(wxT("%i"), downloadInfo->size / MB) : wxString(_T("0")));
		break;

	case DEFAULT_COLUMN:
		//Do nothing
		break;

	default:
		wxASSERT(false);
		break;
	}
}

int DownloadDataViewModel::Compare(const wxDataViewItem& itemA,
		const wxDataViewItem& itemB, unsigned int column,
		bool ascending) const {
	return BaseDataViewModel::Compare(itemA, itemB, column, ascending);
}
