/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentsearchresultdatamodel.h"

ContentSearchResultDataModel::ContentSearchResultDataModel() : BaseDataViewModel(COLUMNS_COUNT){
}

ContentSearchResultDataModel::~ContentSearchResultDataModel() {
}

void ContentSearchResultDataModel::GetValue(wxVariant& variant,
		const wxDataViewItem& item, unsigned int column) const {

	ContentSearchResult * searchResult = static_cast<ContentSearchResult*>(item.GetID());

	wxASSERT(searchResult != nullptr);

    /* In case if wxGTK will try to render invalid item */
    if (searchResult == nullptr || ContainsItem(*searchResult) == false) {
        variant = wxVariant(wxEmptyString);
        return;
    }

	switch(column)
	{
	case NAME:
		variant = wxVariant(searchResult->name);
		break;

	case TYPE:
		variant = wxVariant(searchResult->type);
		break;

	case DOWNLOADED:
		variant = wxVariant(searchResult->is_downloaded ? _("Yes") : _("No"));
		break;

	case PREVIEW:
		variant = wxVariant(wxEmptyString);
		break;

	case SIZE:
		variant = wxVariant(wxString::Format(_T("%u"), searchResult->filesize));
		break;

	case DEFAULT_COLUMN:
		variant = wxVariant(wxEmptyString);
		break;

	default:
		wxASSERT(false);
		variant = wxVariant(wxEmptyString);
		break;
	}
}

int ContentSearchResultDataModel::Compare(const wxDataViewItem& itemA,
		const wxDataViewItem& itemB, unsigned int column,
		bool ascending) const {

	return (BaseDataViewModel::Compare(itemA, itemB, column, ascending));
}

wxString ContentSearchResultDataModel::GetColumnType(
		unsigned int /*column*/) const {
	return COL_TYPE_TEXT;
}
