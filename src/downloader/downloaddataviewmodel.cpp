/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloaddataviewmodel.h"

DownloadDataViewModel::DownloadDataViewModel() {
}

DownloadDataViewModel::~DownloadDataViewModel() {
}

void DownloadDataViewModel::GetValue(wxVariant& /*variant*/,
		const wxDataViewItem& /*item*/, unsigned int /*col*/) const {
	//TODO: implement!
}

int DownloadDataViewModel::Compare(const wxDataViewItem& itemA,
		const wxDataViewItem& itemB, unsigned int column,
		bool ascending) const {
	//TODO: implement!
	return BaseDataViewModel::Compare(itemA, itemB, column, ascending);
}
