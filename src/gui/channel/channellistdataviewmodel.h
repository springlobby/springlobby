/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_CHANNEL_CHANNELLISTDATAVIEWMODEL_H_
#define SRC_GUI_CHANNEL_CHANNELLISTDATAVIEWMODEL_H_

#include <wx/variant.h>
#include "../basedataviewmodel.h"
#include "channellistview.h"
class ChannelListDataViewModel : public BaseDataViewModel<ChannelInfo>
{
public:
	ChannelListDataViewModel();
	virtual ~ChannelListDataViewModel();

	virtual void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const override;
	virtual int Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB, unsigned int column, bool ascending) const override;
	virtual wxString GetColumnType(unsigned int column) const override;

public:
	enum ColumnIndexes {
		CHANNEL_NAME = 0,
		USERS_COUNT,
		TOPIC,
		COLUMNS_COUNT
	};
};

#endif /* SRC_GUI_CHANNEL_CHANNELLISTDATAVIEWMODEL_H_ */
