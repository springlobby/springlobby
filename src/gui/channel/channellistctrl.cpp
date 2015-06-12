/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "channellistctrl.h"
#include "iconimagelist.h"
#include "utils/conversion.h"
#include <algorithm>
#include "utils/sortutil.h"
#include "servermanager.h"

template <>
SortOrder CustomVirtListCtrl<ChannelInfo, ChannelListctrl>::m_sortorder = SortOrder();

BEGIN_EVENT_TABLE(ChannelListctrl, ChannelListctrl::BaseType)
EVT_LIST_ITEM_ACTIVATED(CHANNELLIST, ChannelListctrl::OnActivateItem)
END_EVENT_TABLE()


ChannelListctrl::ChannelListctrl(wxWindow* parent, wxWindowID /*unused*/, const wxString& /*unused*/,
				 long /*unused*/, const wxPoint& /*unused*/, const wxSize& /*unused*/)
    : CustomVirtListCtrl<ChannelInfo, ChannelListctrl>(parent, CHANNELLIST, wxDefaultPosition, wxDefaultSize,
						       wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("ChannelListCtrl"), 3, &ChannelListctrl::CompareOneCrit)
{
	const int widths[3] = {140, 70, 800};

	AddColumn(0, widths[0], _("Channel"), _T("Channelname"));
	AddColumn(1, widths[1], _("# users"), _T("Users"));
	AddColumn(2, widths[2], _T("Topic"), _T("Topic"));

	if (m_sortorder.size() == 0) {
		m_sortorder[2].col = 2;
		m_sortorder[2].direction = -1;
		m_sortorder[0].col = 0;
		m_sortorder[0].direction = -1;
		m_sortorder[1].col = 1;
		m_sortorder[1].direction = -1;
	}
}

ChannelListctrl::~ChannelListctrl()
{
	//dtor
}

void ChannelListctrl::HighlightItem(long)
{
}

/** @brief AddChannel
  *
  * @todo: document this function
  */
void ChannelListctrl::AddChannel(const wxString& channel, unsigned int num_users, const wxString& topic)
{
	ChannelInfo data(channel, num_users, topic);
	m_data.push_back(data);
	m_visible_idxs[m_data.size() - 1] = (m_data.size() - 1);
	SetItemCount(m_visible_idxs.size());

	RefreshItem(m_visible_idxs.size() - 1);
	MarkDirtySort();
}

int ChannelListctrl::CompareOneCrit(DataType u1, DataType u2, int col, int dir) const
{
	assert(dir != 0);
	switch (col) {
		case 0:
			return dir * u1.name.CmpNoCase(u2.name);
		case 1:
			return dir * compareSimple(u1.usercount, u2.usercount);
		case 2:
			return dir * u1.topic.CmpNoCase(u2.topic);
		default:
			assert(false);
			return 0;
	}
}

int ChannelListctrl::GetIndexFromData(const DataType& data) const
{
	DataCIter it = m_data.begin();
	for (int i = 0; it != m_data.end(); ++it, ++i) {
		if (it->name == data.name)
			return i;
	}
	return -1;
}

void ChannelListctrl::Sort()
{
	SaveSelection();
	FilterChannel(m_last_filter_value);
	SLInsertionSort(m_data, m_comparator);
	RestoreSelection();
}

/** @brief OnActivateItem
  *
  * @todo: document this function
  */
void ChannelListctrl::OnActivateItem(wxListEvent& event)
{
	int index = event.GetIndex();
	if (index == -1)
		return;
	wxString chan_name = m_data[m_visible_idxs[index]].name;
	ServerManager::Instance()->JoinChannel(chan_name, wxEmptyString);
}

void ChannelListctrl::ClearChannels()
{
	m_visible_idxs.clear();
	Clear();
}

wxString ChannelListctrl::GetInfo()
{
	int displayed = GetItemCount();
	int total = m_data.size();
	return wxString::Format(_("Displaying %d out of %d channels"), displayed, total);
}

void ChannelListctrl::FilterChannel(const wxString& partial)
{
	m_visible_idxs.clear();
	unsigned int idx = 0;
	for (unsigned int i = 0; i < m_data.size(); ++i) {
		const ChannelInfo& data = m_data[i];
		if (data.name.Find(partial) != wxNOT_FOUND) {
			m_visible_idxs[idx] = i;
			idx++;
		}
	}
	SelectNone();
	m_last_filter_value = partial;
	SetItemCount(m_visible_idxs.size());
	RefreshVisibleItems();
}


int ChannelListctrl::GetItemColumnImage(long /*unused*/, long) const
{
	return -1;
}

int ChannelListctrl::GetItemImage(long /*unused*/) const
{
	return -1;
}

wxString ChannelListctrl::GetItemText(long item, long column) const
{
	int idx = m_visible_idxs.find(item)->second;
	const DataType& chan = m_data[idx];

	switch (column) {
		case 0:
			return chan.name;
		case 1:
			return TowxString(chan.usercount);
		case 2:
			return chan.topic;
		default:
			return wxEmptyString;
	}
}

void ChannelListctrl::SetTipWindowText(const long item_hit, const wxPoint& position)
{
	int column = getColumnFromPosition(position);
	if (column > (int)m_colinfovec.size() || column < 0 || item_hit < 0 || item_hit > (long)m_data.size()) {
		m_tiptext = wxEmptyString;
		return;
	}
	const DataType& channel = m_data[item_hit];
	switch (column) {
		case 2: // status
			m_tiptext = channel.topic;
			m_tiptext.Replace(_T("\\n"), _T("\n"));
			break;

		default:
			m_tiptext = wxEmptyString;
			break;
	}
}
