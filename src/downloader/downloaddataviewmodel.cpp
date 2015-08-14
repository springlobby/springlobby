/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewmodel.h"
#include "downloadinfo.h"

DownloadDataViewModel::DownloadDataViewModel()
	:BaseDataViewModel<DownloadInfo>::BaseDataViewModel(COLUMN_COUNT)
{
}

DownloadDataViewModel::~DownloadDataViewModel() {
}

void DownloadDataViewModel::GetValue(wxVariant& variant,
		const wxDataViewItem& item, unsigned int column) const {

	DownloadInfo * downloadInfo = static_cast<DownloadInfo*>(item.GetID());

	wxASSERT(downloadInfo != nullptr);

	if (ContainsItem(*downloadInfo) == false) {
		return;
	}

	const int MB = 1024 * 1024;

	switch(column)
	{
	case NAME:
		variant = wxVariant(downloadInfo->GetName());
		break;

	case STATUS:
		if (downloadInfo->IsFinished()) {
			variant = wxVariant(wxString(_("complete")));
		} else {
			variant = wxVariant(wxString(_("downloading")));
		}
		break;

	case P_COMPLETE:
		if (downloadInfo->GetSize() < 1) { /* Prevent from division by zero */
			variant = wxVariant(wxString(_T("Indeterminate")));
		} else {
			variant = wxVariant(wxString::Format(wxT("%i%%"), downloadInfo->GetProgressPercent()));
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
		variant = wxVariant(downloadInfo->GetSize() > 0 ? wxString::Format(wxT("%i"), downloadInfo->GetSize() / MB) : wxString(_T("0")));
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

wxString DownloadDataViewModel::GetColumnType(unsigned int) const {

	wxString colTypeString = COL_TYPE_TEXT;

	return colTypeString;
}
