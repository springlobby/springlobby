

#include "settings.h"
#include "iconimagelist.h"
#include "gui/controls.h"
#include "gui/sltipwin.h"
#include "customvirtlistctrl.h"
#include <lslutils/misc.h>

wxBEGIN_EVENT_TABLE_TEMPLATE2(CustomVirtListCtrl, wxListCtrl, T, L)

#if wxUSE_TIPWINDOW
    EVT_MOTION(CustomVirtListCtrl::OnMouseMotion)
    EVT_TIMER(IDD_TIP_TIMER, CustomVirtListCtrl::OnTimer)
#endif
    EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, CustomVirtListCtrl::OnStartResizeCol)
    EVT_LIST_COL_END_DRAG(wxID_ANY, CustomVirtListCtrl::OnEndResizeCol)
    EVT_LEAVE_WINDOW(CustomVirtListCtrl::noOp)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, CustomVirtListCtrl::OnActivated)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, CustomVirtListCtrl::OnSelected)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY, CustomVirtListCtrl::OnDeselected)
    EVT_LIST_DELETE_ITEM(wxID_ANY, CustomVirtListCtrl::OnDeselected)
    EVT_LIST_COL_CLICK(wxID_ANY, CustomVirtListCtrl::OnColClick)
    EVT_SIZE(CustomVirtListCtrl::OnSizeEvent)
    END_EVENT_TABLE()


    template <class T, class L>
    CustomVirtListCtrl<T, L>::CustomVirtListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,
						 long style, const wxString& name, unsigned int sort_criteria_count,
						 CompareFunction func, bool highlight, UserActions::ActionType hlaction, bool periodic_sort, unsigned int periodic_sort_interval)
    : wxListCtrl(parent, id, pt, sz, style | wxLC_VIRTUAL)
    , m_tiptimer(this, IDD_TIP_TIMER)
    , m_sort_timer(this, IDD_SORT_TIMER)
    , m_tiptext(wxEmptyString)
    ,
#if wxUSE_TIPWINDOW
    m_tipwindow(0)
    , m_controlPointer(0)
    ,
#endif
    m_columnCount(0)
    , m_selected_index(-1)
    , m_last_mouse_pos(wxPoint(-1, -1))
    , m_name(name)
    , m_highlight(highlight)
    , m_highlightAction(hlaction)
    , m_bg_color(GetBackgroundColour())
    , m_dirty_sort(false)
    , m_sort_criteria_count(sort_criteria_count)
    , m_comparator(this, m_sortorder, func)
    , m_periodic_sort_timer_id(wxNewId())
    , m_periodic_sort_timer(this, m_periodic_sort_timer_id)
    , m_periodic_sort(periodic_sort)
    , m_periodic_sort_interval(periodic_sort_interval)
{
	SetAutoResizableColumn(-1);

	SetImageList(&icons(), wxIMAGE_LIST_NORMAL);
	SetImageList(&icons(), wxIMAGE_LIST_SMALL);
	m_sortorder = sett().GetSortOrder(name);

	StartTimer();
	Connect(ListctrlDoSortEventType, wxCommandEventHandler(ThisType::OnSortEvent), NULL, this);

	unsigned char r = m_bg_color.Red();
	unsigned char g = m_bg_color.Green();
	unsigned char b = m_bg_color.Blue();

	r = r < 128 ? r + 20 : r - 20;
	g = g < 128 ? g + 20 : g - 20;
	b = b < 128 ? b + 20 : b - 20;

	m_list_attr_one.SetBackgroundColour(wxColour(r, g, b));
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnQuit(wxCommandEvent& /*data*/)
{
	m_periodic_sort_timer.Stop();
	m_dirty_sort = false;
	Clear();
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::StartTimer()
{
	if (m_periodic_sort) {
		Connect(m_periodic_sort_timer_id, wxTimerEvent().GetEventType(), wxTimerEventHandler(ThisType::OnPeriodicSort));
#ifndef NDEBUG
		bool started =
#endif
		    m_periodic_sort_timer.Start(m_periodic_sort_interval);
		assert(started);
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::StopTimer()
{
	m_periodic_sort_timer.Stop();
	Disconnect(m_periodic_sort_timer_id, wxTimerEvent().GetEventType(), wxTimerEventHandler(ThisType::OnPeriodicSort));
}

template <class T, class L>
CustomVirtListCtrl<T, L>::~CustomVirtListCtrl()
{
	StopTimer();
	sett().SetSortOrder(m_name, m_sortorder);
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::AddColumn(long i, int width, const wxString& label, const wxString& tip, bool modifiable)
{
	SetAutoResizableColumn(m_columnCount);

	m_columnCount++;

	//Lookup for user preferable width stored in settings
	int realWidth = sett().GetColumnWidth(m_name, i);
	if (realWidth == Settings::columnWidthUnset) {
		realWidth = width;
	}

	wxListCtrl::InsertColumn(i, label, wxLIST_FORMAT_LEFT, realWidth);
	colInfo temp(i, label, tip, modifiable, realWidth);
	m_colinfovec.push_back(temp);
}


template <class T, class L>
void CustomVirtListCtrl<T, L>::SaveSelection()
{
	ResetSelection();

	long item = -1;
	while (true) {
		item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		if (item == -1)
			break;
		m_selected_data.push_back(m_data[item]);
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::RestoreSelection()
{
	while (!m_selected_data.empty()) {
		SelectedDataType data = m_selected_data.back();
		m_selected_data.pop_back();
		int idx = GetIndexFromData(data);
		if ((idx < 0) || (idx >= GetItemCount())) { //check if idx is valid (could be deleted)
			continue;
		}
		SetItemState(idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::ResetSelection()
{
	m_selected_data.clear();
	m_selected_index = -1;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnActivated(wxListEvent& event)
{
	m_selected_index = event.GetIndex();
	event.Skip();
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnSelected(wxListEvent& event)
{
	m_selected_index = event.GetIndex();
	event.Skip();
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnDeselected(wxListEvent& event)
{
	if (m_selected_index == event.GetIndex())
		m_selected_index = -1;
}

template <class T, class L>
long CustomVirtListCtrl<T, L>::GetSelectedIndex() const
{
	return m_selected_index;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::SelectAll()
{
	for (long i = 0; i < GetItemCount(); i++) {
		SetItemState(i, wxLIST_STATE_SELECTED, -1);
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::SelectNone()
{
	for (long i = 0; i < GetItemCount(); i++) {
		SetItemState(i, wxLIST_STATE_DONTCARE, -1);
	}
}
template <class T, class L>
void CustomVirtListCtrl<T, L>::SelectInverse()
{
	for (long i = 0; i < GetItemCount(); i++) {
		int state = GetItemState(i, -1);
		state = (state == wxLIST_STATE_DONTCARE ? wxLIST_STATE_SELECTED : wxLIST_STATE_DONTCARE);
		SetItemState(i, state, -1);
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::SetSelectedIndex(const long newindex)
{
	m_selected_index = newindex;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::RefreshVisibleItems()
{
	if (m_data.empty())
		return;
	const long topItemIndex = GetTopItem();
	const long range = topItemIndex + GetCountPerPage();
	RefreshItems(topItemIndex, LSL::Util::Clamp(range, topItemIndex, (long)m_data.size() - 1));
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnTimer(wxTimerEvent& /*unused*/)
{
#if wxUSE_TIPWINDOW

	if (!m_tiptext.empty()) {
		m_tipwindow = new SLTipWindow(this, m_tiptext);
		m_controlPointer = &m_tipwindow;
		m_tipwindow->SetTipWindowPtr((wxTipWindow**)m_controlPointer);
		m_tiptext = wxEmptyString;
		m_tiptimer.Start(m_tooltip_duration, wxTIMER_ONE_SHOT);
	} else {
		m_tiptext = wxEmptyString;
		m_tiptimer.Stop();
		if (m_controlPointer != 0 && *m_controlPointer != 0) {
			m_tipwindow->Close();
			delete m_tipwindow;
			m_tipwindow = 0;
		}
	}

#endif
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnMouseMotion(wxMouseEvent& event)
{
	m_sort_timer.Stop();
	m_sort_timer.Start(m_sort_block_time, wxTIMER_ONE_SHOT);
#if wxUSE_TIPWINDOW
	//we don't want to display the tooltip again until mouse has moved
	if (m_last_mouse_pos == event.GetPosition())
		return;

	m_last_mouse_pos = event.GetPosition();

	if (event.Leaving()) {
		m_tiptext = wxEmptyString;
		if (m_tipwindow) {
			m_tipwindow->Close();
			delete m_tipwindow;
			m_tipwindow = 0;
		}
		m_tiptimer.Stop();
	} else {
		if (m_tiptimer.IsRunning()) {
			m_tiptimer.Stop();
		}

		wxPoint position = event.GetPosition();

		int flag = wxLIST_HITTEST_ONITEM;

		long item_hit = HitTest(position, flag);
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

template <class T, class L>
void CustomVirtListCtrl<T, L>::SetTipWindowText(const long /*unused*/, const wxPoint& position)
{
	int column = getColumnFromPosition(position);
	if (column >= int(m_colinfovec.size()) || column < 0) {
		m_tiptext = wxEmptyString;
	} else {
		m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
		m_tiptext = _(m_colinfovec[column].tip);
	}
}

template <class T, class L>
int CustomVirtListCtrl<T, L>::getColumnFromPosition(wxPoint pos)
{
	int x_pos = 0;
	for (int i = 0; i < int(m_colinfovec.size()); ++i) {
		x_pos += GetColumnWidth(i);
		if (pos.x < x_pos)
			return i;
	}
	return -1;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnStartResizeCol(wxListEvent& event)
{
	if (!m_colinfovec[event.GetColumn()].can_resize)
		event.Veto();
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnEndResizeCol(wxListEvent& event)
{
	//Recalculate width of one column
	AdjustColumnsWidth();

	//let the event go further
	event.Skip();
}

template <class T, class L>
bool CustomVirtListCtrl<T, L>::SetColumnWidth(int col, int width)
{
	int realWidth = width;

	assert(col < (long)m_columnCount);
	assert(col >= 0);

	//If width is set to wxLIST_AUTOSIZE or wxLIST_AUTOSIZE_USEHEADER
	//check settings for concrete value stored from last time
	if (width < 0) {
		realWidth = sett().GetColumnWidth(m_name, col);
		if (realWidth == Settings::columnWidthUnset) {
			realWidth = width; //No value found in settings, use original
		}
	}

	sett().SetColumnWidth(m_name, col, realWidth);
	return wxListCtrl::SetColumnWidth(col, realWidth);
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::noOp(wxMouseEvent& event)
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

template <class T, class L>
wxListItemAttr* CustomVirtListCtrl<T, L>::HighlightItemUser(const wxString& name) const
{
	static wxListItemAttr att;
	if (m_highlight && useractions().DoActionOnUser(m_highlightAction, name)) {
		att.SetBackgroundColour(useractions().GetGroupHLColor(useractions().GetGroupOfUser(name)));
		return &att;
	} else
		return NULL;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::SetHighLightAction(UserActions::ActionType action)
{
	m_highlightAction = action;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::MarkDirtySort()
{
	m_dirty_sort = true;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::CancelTooltipTimer()
{
	m_tiptimer.Stop();
}

template <class T, class L>
bool CustomVirtListCtrl<T, L>::PopupMenu(wxMenu* menu, const wxPoint& pos)
{
	CancelTooltipTimer();
	return wxListCtrl::PopupMenu(menu, pos);
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::SortList(bool force)
{
	if ((m_sort_timer.IsRunning() || !m_dirty_sort) && !force) {
		return;
	}

	{
		wxWindowUpdateLocker upd(this);
		SaveSelection();
		Sort();
		RestoreSelection();
		m_dirty_sort = false;
	}
	RefreshVisibleItems(); //needs to be out of locker scope
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::Clear()
{
	m_data.clear();
	m_selected_data.clear();
	SetItemCount(0);
	ResetSelection();
	RefreshVisibleItems();
}

template <class T, class L>
typename CustomVirtListCtrl<T, L>::DataType CustomVirtListCtrl<T, L>::GetDataFromIndex(const long index)
{
	return m_data[index];
}

template <class T, class L>
const typename CustomVirtListCtrl<T, L>::DataType CustomVirtListCtrl<T, L>::GetDataFromIndex(const long index) const
{
	return m_data[index];
}

template <class T, class L>
typename CustomVirtListCtrl<T, L>::DataType CustomVirtListCtrl<T, L>::GetSelectedData()
{
	return GetDataFromIndex(m_selected_index);
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::ResetColumnSizes()
{
	int columnsCount = GetColumnCount();

	for (int columnIndex = 0; columnIndex < columnsCount; ++columnIndex) {
		int columnWidth = sett().GetColumnWidth(m_name, columnIndex);
		wxListCtrl::SetColumnWidth(columnIndex, columnWidth);
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnColClick(wxListEvent& event)
{
	if (event.GetColumn() == -1)
		return;

	const int evt_col = event.GetColumn();


	m_sort_timer.Stop(); //otherwise sorting will be way delayed

	int old_sort_col = m_sortorder[0].col;

	wxListItem col;
	GetColumn(m_sortorder[0].col, col);
	col.SetImage(icons().ICON_NONE);
	SetColumn(m_sortorder[0].col, col);

	unsigned int i = 0;
	SortOrder::const_iterator it = m_sortorder.begin();
	for (; it != m_sortorder.begin(); ++i, ++it) {
		if (m_sortorder[i].col == evt_col)
			break;
	}

	//    for ( ; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}

	i = LSL::Util::Clamp(i, (unsigned int)0, m_sort_criteria_count);

	for (; i > 0; i--) {
		m_sortorder[i] = m_sortorder[i - 1];
	}

	m_sortorder[0].col = evt_col;
	m_sortorder[0].direction *= -1;


	GetColumn(m_sortorder[0].col, col);
	//col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
	col.SetImage((m_sortorder[0].direction > 0) ? icons().ICON_UP : icons().ICON_DOWN);
	SetColumn(m_sortorder[0].col, col);

	if ((old_sort_col != m_sortorder[0].col) || m_dirty_sort) {
		SortList(true);
	} else { // O(n) instead of guaranteed worst case O(n*n)
		ReverseOrder();
	}
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnSortEvent(wxCommandEvent& evt)
{
	bool force = evt.GetInt() != 0;
	SortList(force);
}

template <class T, class L>
bool CustomVirtListCtrl<T, L>::GetColumn(int col, wxListItem& item) const
{
	return wxListCtrl::GetColumn(col, item);
}

template <class T, class L>
bool CustomVirtListCtrl<T, L>::SetColumn(int col, const wxListItem& item)
{
	return wxListCtrl::SetColumn(col, item);
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::ReverseOrder()
{
	SaveSelection();
	std::reverse(m_data.begin(), m_data.end());
	RefreshVisibleItems();
	RestoreSelection();
}

template <class T, class L>
bool CustomVirtListCtrl<T, L>::AddItem(const T& item)
{
	if (GetIndexFromData(item) != -1)
		return false;

	m_data.push_back(item);
	SetItemCount(m_data.size());
	RefreshItem(m_data.size() - 1);
	MarkDirtySort();
	return true;
}

template <class T, class L>
bool CustomVirtListCtrl<T, L>::RemoveItem(const T& item)
{
	int index = GetIndexFromData(item);
	if ((index < 0) || (index > (long)m_data.size())) {
		return false;
	}
	SaveSelection();
	m_data.erase(m_data.begin() + index);
	SetItemCount(m_data.size());
	if (index > (long)m_data.size() - 1)
		index--;
	if (m_data.size() > 0) {
		RestoreSelection();
		RefreshVisibleItems();
	} else {
		Clear();
	}
	return true;
}

template <class T, class L>
wxString CustomVirtListCtrl<T, L>::OnGetItemText(long item, long column) const
{
	//assert( item < (long)m_data.size() );
	//assert( column < m_columnCount );
	column = LSL::Util::Clamp(column, long(0), long(m_columnCount));
	return GetItemText(item, column);
}

template <class T, class L>
int CustomVirtListCtrl<T, L>::OnGetItemColumnImage(long item, long column) const
{
	return GetItemColumnImage(item, column);
}

template <class T, class L>
wxListItemAttr* CustomVirtListCtrl<T, L>::OnGetItemAttr(long item) const
{
	auto* _att = GetItemAttr(item);

	if (_att != NULL)
		return _att;

	if ((item % 2) == 0)
		return const_cast<wxListItemAttr*>(&m_list_attr_one);
	else
		return const_cast<wxListItemAttr*>(&m_list_attr_two);
}

template <class T, class L>
wxString CustomVirtListCtrl<T, L>::GetItemText(long /*item*/, long /*column*/) const
{
	return wxEmptyString;
}

template <class T, class L>
int CustomVirtListCtrl<T, L>::GetItemColumnImage(long /*item*/, long /*column*/) const
{
	return -1;
}

template <class T, class L>
wxListItemAttr* CustomVirtListCtrl<T, L>::GetItemAttr(long /*item*/) const
{
	return nullptr;
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::OnPeriodicSort(wxTimerEvent& /*unused*/)
{

	SortList();
}

template <class T, class L>
void CustomVirtListCtrl<T, L>::SetAutoResizableColumn(int columnIndex)
{
	autoResizableColumnIndex = columnIndex;
}

//Catch OnSizeEvent only for adjusting width
//of one of columns
template <class T, class L>
void CustomVirtListCtrl<T, L>::OnSizeEvent(wxSizeEvent& sizeEvent)
{
	AdjustColumnsWidth();
	sizeEvent.Skip();
}

//Recalculate width of some column (autoResizableColumnIndex)
//so wxListCtrl will fit all available space
//This is just for make it look better
template <class T, class L>
void CustomVirtListCtrl<T, L>::AdjustColumnsWidth()
{
	int columnsCount = GetColumnCount();
	int totalColumnsWidth = 0;

	if (autoResizableColumnIndex < 0) {
		return;
	}

	if (autoResizableColumnIndex >= columnsCount) {
		return;
	}

	for (int colIndex = 0; colIndex < columnsCount; ++colIndex) {
		if (colIndex == autoResizableColumnIndex) {
			continue;
		}
		totalColumnsWidth += GetColumnWidth(colIndex);
	}

	wxSize clientSize = GetClientSize();
	SetColumnWidth(autoResizableColumnIndex, clientSize.GetWidth() - totalColumnsWidth);

	//Save all column sizes
	for (int columnIndex = 0; columnIndex < GetColumnCount(); ++columnIndex) {
		sett().SetColumnWidth(m_name, columnIndex, GetColumnWidth(columnIndex));
	}
}
