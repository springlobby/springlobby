/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "channellistdataviewmodel.h"

ChannelListDataViewModel::ChannelListDataViewModel()
    : BaseDataViewModel(COLUMNS_COUNT)
{
}

ChannelListDataViewModel::~ChannelListDataViewModel()
{
}

void ChannelListDataViewModel::GetValue(wxVariant& variant,
					const wxDataViewItem& item, unsigned int column) const
{
	ChannelInfo* channelInfo = static_cast<ChannelInfo*>(item.GetID());

	wxASSERT(channelInfo != nullptr);

	/* In case if wxGTK will try to render invalid item */
	if (channelInfo == nullptr || !ContainsItem(*channelInfo)) {
		variant = wxVariant(wxEmptyString);
		return;
	}

	switch (column) {
		case TOPIC:
			variant = wxVariant(channelInfo->topic);
			break;

		case USERS_COUNT:
			variant = wxVariant(wxString::Format(_T("%d"), channelInfo->usercount));
			break;

		case CHANNEL_NAME:
			variant = wxVariant(channelInfo->name);
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

int ChannelListDataViewModel::Compare(const wxDataViewItem& itemA,
				      const wxDataViewItem& itemB, unsigned int column,
				      bool ascending) const
{

	const ChannelInfo* channelA = static_cast<const ChannelInfo*>(itemA.GetID());
	const ChannelInfo* channelB = static_cast<const ChannelInfo*>(itemB.GetID());

	if (channelA == nullptr || channelB == nullptr) {
		wxASSERT(false);
		return 0;
	}

	int sortingResult = 0;

	switch (column) {
		case TOPIC:
			if (channelA->topic > channelB->topic) {
				sortingResult = 1;
			} else if (channelA->topic < channelB->topic) {
				sortingResult = -1;
			} else {
				sortingResult = 0;
			}
			break;

		case CHANNEL_NAME:
			if (channelA->name > channelB->name) {
				sortingResult = 1;
			} else if (channelA->name < channelB->name) {
				sortingResult = -1;
			} else {
				sortingResult = 0;
			}
			break;

		case USERS_COUNT:
			if (channelA->usercount > channelB->usercount) {
				sortingResult = 1;
			} else if (channelA->usercount < channelB->usercount) {
				sortingResult = -1;
			} else {
				sortingResult = 0;
			}
			break;

		case DEFAULT_COLUMN:
			sortingResult = 0;
			break;

		default:
			wxASSERT(false);
			sortingResult = 0;
	}

	//Return direct sort order or reversed depending on ascending flag
	return ascending ? sortingResult : (sortingResult * (-1));
}

wxString ChannelListDataViewModel::GetColumnType(unsigned int /*column*/) const
{
	wxString colTypeString = COL_TYPE_TEXT;

	return colTypeString;
}
