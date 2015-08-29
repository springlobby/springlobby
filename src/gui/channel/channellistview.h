/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_CHANNEL_CHANNELLISTVIEW_H_
#define SRC_GUI_CHANNEL_CHANNELLISTVIEW_H_

#include <wx/string.h>
#include <map>

#include "gui/basedataviewctrl.h"

using namespace std;

class wxWindow;

struct ChannelInfo
{
	ChannelInfo()
	    : name(wxEmptyString)
	    , usercount(0)
	    , topic(wxEmptyString)
	{
	}
	ChannelInfo(const wxString& name_, int usercount_, const wxString& topic_ = wxEmptyString)
	    : name(name_)
	    , usercount(usercount_)
	    , topic(topic_)
	{
	}

	wxString name;
	int usercount;
	wxString topic;
};

class ChannelListView: public BaseDataViewCtrl<ChannelInfo> {
public:
	ChannelListView(const wxString& dataViewName, wxWindow* parent);
	virtual ~ChannelListView();

	void AddChannel(const wxString& channel, unsigned int num_users, const wxString& topic);
	void ClearChannels();
	wxString GetInfo();
	void FilterChannel(const wxString& partial);

private:
	void OnItemActivated(wxDataViewEvent& event);

	bool AddRealItem(ChannelInfo* channel);
	bool IsContainsRealItem(ChannelInfo* channel) const;

public:
	enum {
		CHANNELLIST_ID = wxID_HIGHEST
	};
	enum ColumnIndexes
	{
		CHANNEL_NAME = 0,
		USERS_COUNT,
		TOPIC,
		COLUMNS_COUNT
	};

private:
	map<wxString, ChannelInfo*> m_realChannelCollection;

	DECLARE_EVENT_TABLE()
};

#endif /* SRC_GUI_CHANNEL_CHANNELLISTVIEW_H_ */
