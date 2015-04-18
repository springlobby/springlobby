/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/wupdlock.h>
#include <wx/colour.h>
#include <wx/log.h>

#include "customlistctrl.h"
#include "gui/sltipwin.h"
#include "gui/controls.h"
#include "settings.h"
#include "iconimagelist.h"
#include "gui/customdialogs.h"
#include "uiutils.h"


BEGIN_EVENT_TABLE(CustomListCtrl, wxListCtrl)
#if wxUSE_TIPWINDOW
EVT_MOTION(CustomListCtrl::OnMouseMotion)
EVT_TIMER(IDD_TIP_TIMER, CustomListCtrl::OnTimer)
#endif
EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, CustomListCtrl::OnStartResizeCol)
EVT_LIST_COL_END_DRAG(wxID_ANY, CustomListCtrl::OnEndResizeCol)
EVT_LEAVE_WINDOW(CustomListCtrl::noOp)
EVT_LIST_ITEM_SELECTED(wxID_ANY, CustomListCtrl::OnSelected)
EVT_LIST_ITEM_DESELECTED(wxID_ANY, CustomListCtrl::OnDeselected)
EVT_LIST_DELETE_ITEM(wxID_ANY, CustomListCtrl::OnDeselected)
END_EVENT_TABLE()


//wxTipWindow* CustomListCtrl::m_tipwindow = 0;
CustomListCtrl::CustomListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz, long style, wxString name,
			       unsigned int column_count, bool highlight, UserActions::ActionType hlaction)
    : wxListCtrl(parent, id, pt, sz, style)
    , m_tiptimer(this, IDD_TIP_TIMER)
    , m_tiptext(wxEmptyString)
    ,
#if wxUSE_TIPWINDOW
    m_tipwindow(0)
    , m_controlPointer(0)
    ,
#endif
    m_columnCount(column_count)
    , m_selected(-1)
    , m_selected_index(-1)
    , m_prev_selected(-1)
    , m_prev_selected_index(-1)
    , m_last_mouse_pos(wxPoint(-1, -1))
    , m_name(name)
    , m_highlight(highlight)
    , m_highlightAction(hlaction)
    , m_bg_color(GetBackgroundColour())
    , m_dirty_sort(false)
{
	//dummy init , will later be replaced with loading from settings
	for (unsigned int i = 0; i < m_columnCount; ++i) {
		m_column_map[i] = i;
	}

	SetImageList(&icons(), wxIMAGE_LIST_NORMAL);
	SetImageList(&icons(), wxIMAGE_LIST_SMALL);
	SetImageList(&icons(), wxIMAGE_LIST_STATE);
}

void CustomListCtrl::InsertColumn(long i, wxListItem item, wxString tip, bool modifiable)
{
	wxListCtrl::InsertColumn(i, item);
	colInfo temp(tip, modifiable);
	m_colinfovec.push_back(temp);
}

void CustomListCtrl::AddColumn(long i, int width, const wxString& label, const wxString& tip, bool modifiable)
{
	wxListCtrl::InsertColumn(i, label, wxLIST_FORMAT_LEFT, width);
	colInfo temp(tip, modifiable);
	m_colinfovec.push_back(temp);
}

void CustomListCtrl::SetSelectionRestorePoint()
{
	m_prev_selected = m_selected;
	m_prev_selected_index = m_selected_index;
}

void CustomListCtrl::RestoreSelection()
{
	if (m_prev_selected_index > -1) {
		SetItemState(GetIndexFromData(m_prev_selected), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}
}

void CustomListCtrl::ResetSelection()
{
	m_selected = m_prev_selected = m_prev_selected_index = m_selected_index = -1;
}

void CustomListCtrl::OnSelected(wxListEvent& event)
{
	m_selected = GetItemData(event.GetIndex());
	m_selected_index = event.GetIndex();
	event.Skip();
}


void CustomListCtrl::OnDeselected(wxListEvent& event)
{
	if (m_selected == (int)GetItemData(event.GetIndex()))
		m_selected = m_selected_index = -1;
}

long CustomListCtrl::GetIndexFromData(const unsigned long data)
{
	for (int i = 0; i < GetItemCount(); i++) {
		if (data == GetItemData(i))
			return i;
	}
	return -1;
}

long CustomListCtrl::GetSelectedIndex()
{
	return m_selected_index;
}

void CustomListCtrl::SelectAll()
{
	for (long i = 0; i < GetItemCount(); i++) {
		SetItemState(i, wxLIST_STATE_SELECTED, -1);
	}
}

void CustomListCtrl::SelectNone()
{
	for (long i = 0; i < GetItemCount(); i++) {
		SetItemState(i, wxLIST_STATE_DONTCARE, -1);
	}
}

void CustomListCtrl::SelectInverse()
{
	for (long i = 0; i < GetItemCount(); i++) {
		int state = GetItemState(i, -1);
		state = (state == wxLIST_STATE_DONTCARE ? wxLIST_STATE_SELECTED : wxLIST_STATE_DONTCARE);
		SetItemState(i, state, -1);
	}
}

void CustomListCtrl::SetSelectedIndex(const long newindex)
{
	m_selected_index = newindex;
}

long CustomListCtrl::GetSelectedData()
{
	return m_selected;
}

void CustomListCtrl::OnTimer(wxTimerEvent& /*unused*/)
{
#if wxUSE_TIPWINDOW

	if (!m_tiptext.empty()) {
		m_tipwindow = new SLTipWindow(this, m_tiptext);
		m_controlPointer = &m_tipwindow;
		m_tipwindow->SetTipWindowPtr((wxTipWindow**)m_controlPointer);
		m_tipwindow->SetBoundingRect(wxRect(1, 1, 50, 50));
		m_tiptext = wxEmptyString;
		m_tiptimer.Start(m_tooltip_duration, wxTIMER_ONE_SHOT);
	} else {
		m_tiptext = wxEmptyString;
		m_tiptimer.Stop();
		if (m_controlPointer != 0 && *m_controlPointer != 0) {
			m_tipwindow->Close();
			m_tipwindow = 0;
		}
	}

#endif
}

void CustomListCtrl::OnMouseMotion(wxMouseEvent& event)
{
#if wxUSE_TIPWINDOW
	//we don't want to display the tooltip again until mouse has moved
	if (m_last_mouse_pos == event.GetPosition())
		return;

	m_last_mouse_pos = event.GetPosition();

	if (event.Leaving()) {
		m_tiptext = wxEmptyString;
		if (m_tipwindow) {
			m_tipwindow->Close();
			m_tipwindow = 0;
		}
		m_tiptimer.Stop();
	} else {
		if (m_tiptimer.IsRunning()) {
			m_tiptimer.Stop();
		}

		wxPoint position = event.GetPosition();

		int flag = wxLIST_HITTEST_ONITEM;

		long subItem;
		long item_hit = HitTest(position, flag, &subItem);
		if (item_hit != wxNOT_FOUND && item_hit >= 0 && item_hit < GetItemCount()) {
			// we don't really need to recover from this if it fails
			try {
				SetTipWindowText(item_hit, m_last_mouse_pos);
				m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
			} catch (...) {
				wxLogWarning(_T("Exception setting tooltip"));
			}
		}
	}
#endif
}

void CustomListCtrl::SetTipWindowText(const long /*unused*/, const wxPoint& position)
{
	int column = getColumnFromPosition(position);
	if (column >= int(m_colinfovec.size()) || column < 0) {
		m_tiptext = wxEmptyString;
	} else {
		m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
		m_tiptext = m_colinfovec[column].first;
	}
}

int CustomListCtrl::getColumnFromPosition(wxPoint pos)
{
	int x_pos = 0;
	for (int i = 0; i < int(m_colinfovec.size()); ++i) {
		x_pos += GetColumnWidth(i);
		if (pos.x < x_pos)
			return i;
	}
	return -1;
}

void CustomListCtrl::OnStartResizeCol(wxListEvent& event)
{
	if (!m_colinfovec[event.GetColumn()].second)
		event.Veto();
}

void CustomListCtrl::OnEndResizeCol(wxListEvent& event)
{
	int column = event.GetColumn();
	int new_size = GetColumnWidth(column);
	sett().SetColumnWidth(m_name, column, new_size);

	//let the event go further
	event.Skip();
}

bool CustomListCtrl::SetColumnWidth(int col, int width)
{
	if (sett().GetColumnWidth(m_name, col) != Settings::columnWidthUnset) {
		return wxListCtrl::SetColumnWidth(col, sett().GetColumnWidth(m_name, col));
	} else {
		sett().SetColumnWidth(m_name, col, width);
		return wxListCtrl::SetColumnWidth(col, width);
	}
}

void CustomListCtrl::noOp(wxMouseEvent& event)
{
	m_tiptext = wxEmptyString;
	//            m_tiptimer.Stop();
	//            if (m_controlPointer!= 0 && *m_controlPointer!= 0)
	//            {
	//                m_tipwindow->Close();
	//                m_tipwindow = 0;
	//            }
	event.Skip();
}

void CustomListCtrl::UpdateHighlights()
{
	wxWindowUpdateLocker noUpdates(this);
	try {
		long item = -1;
		while (true) {
			item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
			if (item == -1)
				break;
			HighlightItem(item);
		}
	} catch (...) {
	}
}

void CustomListCtrl::HighlightItemUser(long item, const wxString& name)
{
	if (m_highlight && useractions().DoActionOnUser(m_highlightAction, name)) {
		wxColour c = useractions().GetGroupHLColor(useractions().GetGroupOfUser(name));
		SetItemBackgroundColour(item, c);
	} else
		SetItemBackgroundColour(item, m_bg_color);
}

void CustomListCtrl::SetHighLightAction(UserActions::ActionType action)
{
	m_highlightAction = action;
}

void CustomListCtrl::MarkDirtySort()
{
	m_dirty_sort = true;
}

void CustomListCtrl::CancelTooltipTimer()
{
	m_tiptimer.Stop();
}

bool CustomListCtrl::PopupMenu(wxMenu* menu, const wxPoint& pos)
{
	CancelTooltipTimer();
	return wxListCtrl::PopupMenu(menu, pos);
}
