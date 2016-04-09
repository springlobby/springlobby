/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewmodel.h"
#include "utils/conversion.h"

DownloadDataViewModel::DownloadDataViewModel()
    : BaseDataViewModel<PrDownloader::DownloadProgress>::BaseDataViewModel(COLUMN_COUNT)
{
}

DownloadDataViewModel::~DownloadDataViewModel()
{
}

void DownloadDataViewModel::GetValue(wxVariant& variant,
				     const wxDataViewItem& item, unsigned int column) const
{

	PrDownloader::DownloadProgress* downloadInfo = static_cast<PrDownloader::DownloadProgress*>(item.GetID());

	wxASSERT(downloadInfo != nullptr);

	/* In case if wxGTK will try to render invalid item */
	if (downloadInfo == nullptr || ContainsItem(*downloadInfo) == false) {
		variant = wxVariant(wxEmptyString);
		return;
	}

	const int MB = 1024 * 1024;

	switch (column) {
		case NAME:
			variant = wxVariant(TowxString(downloadInfo->name));
			break;

		case STATUS:
			if (downloadInfo->IsFinished()) {
				variant = wxVariant(wxString(_("complete")));
			} else {
				variant = wxVariant(wxString(_("downloading")));
			}
			break;

		case P_COMPLETE:
			variant = wxVariant(wxString::Format(wxT("%.2f%%"), downloadInfo->GetProgressPercent()));
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
			variant = wxVariant(wxString::Format(wxT("%i"), downloadInfo->filesize / MB));
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
				   bool ascending) const
{
	return BaseDataViewModel::Compare(itemA, itemB, column, ascending);
}

wxString DownloadDataViewModel::GetColumnType(unsigned int) const
{

	wxString colTypeString = COL_TYPE_TEXT;

	return colTypeString;
}
