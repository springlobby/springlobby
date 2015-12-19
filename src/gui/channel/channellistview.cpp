/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "channellistview.h"

#include "servermanager.h"
#include "channellistdataviewmodel.h"
#include "utils/conversion.h"


BEGIN_EVENT_TABLE(ChannelListView, BaseDataViewCtrl<ChannelInfo>)
	EVT_DATAVIEW_ITEM_ACTIVATED(CHANNELLIST_ID, ChannelListView::OnItemActivated)
END_EVENT_TABLE()


ChannelListView::ChannelListView(const wxString& dataViewName, wxWindow* parent) : BaseDataViewCtrl(dataViewName, parent, CHANNELLIST_ID){
	ChannelListDataViewModel* model = new ChannelListDataViewModel();
	AssociateModel(model);

	const int DEFAULT_SIZE = wxCOL_WIDTH_AUTOSIZE;
	AppendTextColumn(_("Channel"), CHANNEL_NAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("# users"), USERS_COUNT, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Topic"), TOPIC, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	LoadColumnProperties();
}

ChannelListView::~ChannelListView() {
}

void ChannelListView::AddChannel(const wxString& channel,
		unsigned int num_users, const wxString& topic) {

	ChannelInfo* data = new ChannelInfo(channel, num_users, topic);

	AddRealItem(data);
	AddItem(*data);
}

void ChannelListView::ClearChannels() {
	Clear();
}

wxString ChannelListView::GetInfo() {
	int itemsCount = GetItemsCount();
	int totalItems = m_realChannelCollection.size();
	return wxString::Format(_("Displaying %d out of %d channels"), itemsCount, totalItems);
}

void ChannelListView::FilterChannel(const wxString& partial) {

	for (auto const item : m_realChannelCollection) {
		if ( (partial.IsEmpty()) || (item.second->name.Contains(partial))) {
			if (ContainsItem(*item.second) == false) {
				AddItem(*item.second);
			}
		} else {
			if (ContainsItem(*item.second) == true) {
				RemoveItem(*item.second);
			}
		}
	}

	Resort();
	Refresh();
}

void ChannelListView::OnItemActivated(wxDataViewEvent& /*event*/) {
	ChannelInfo* channelInfo = static_cast<ChannelInfo*>(GetSelection().GetID());

	if (channelInfo == nullptr) {
		return;
	}

	wxString chan_name = channelInfo->name;
	ServerManager::Instance()->JoinChannel(STD_STRING(chan_name), "");
}

bool ChannelListView::AddRealItem(ChannelInfo* channel) {
	if (IsContainsRealItem(channel)) {
		return false;
	}

	m_realChannelCollection[channel->name] = channel;
	return true;
}

bool ChannelListView::IsContainsRealItem(ChannelInfo* channel) const {
	const auto it = m_realChannelCollection.find(channel->name);
	if (it == m_realChannelCollection.end()) {
		return false;
	}
	return true;
}
