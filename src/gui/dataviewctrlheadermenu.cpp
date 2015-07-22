/*
 * dataviewctrlheadermenu.cpp
 *
 *  Created on: 22 июля 2015 г.
 *      Author: Руслан
 */
#include <wx/dataview.h>

#include "dataviewvtrlheadermenu.h"

DataViewCtrlHeaderMenu::DataViewCtrlHeaderMenu(wxDataViewCtrl* parent, int column) : wxMenu(_("Header Managing")){
	parentDataView = parent;
	columnIndex = column;

	CreateMenuItems();

	ConnectEvents();
}

DataViewCtrlHeaderMenu::~DataViewCtrlHeaderMenu() {
}

void DataViewCtrlHeaderMenu::ConnectEvents() {
	Connect(HIDE_COLUMN_ID, wxEVT_MENU, wxObjectEventFunction(&DataViewCtrlHeaderMenu::OnHideColumn));
	Connect(SHOW_ALL_COLUMNS_ID, wxEVT_MENU, wxObjectEventFunction(&DataViewCtrlHeaderMenu::OnShowColumns));
}

void DataViewCtrlHeaderMenu::OnHideColumn(wxCommandEvent&) {
	wxCommandEvent newEvent;

	newEvent.SetEventType(HIDE_COLUMN_EVT);
	newEvent.SetExtraLong(columnIndex);

	parentDataView->GetEventHandler()->ProcessEvent(newEvent);
}

void DataViewCtrlHeaderMenu::CreateMenuItems() {
	int visibleColumnsCount = CountVisibleColumns();

	if (visibleColumnsCount > 1) {
		Append(HIDE_COLUMN_ID, _("Hide this column"));
	}

	int totalColumnsCount = parentDataView->GetColumnCount();

	if (visibleColumnsCount != totalColumnsCount) {
		Append(SHOW_ALL_COLUMNS_ID, _("Show all columns"));
	}
}

int DataViewCtrlHeaderMenu::CountVisibleColumns() {

	int totalVisibleColumnsCount = 0;
	int totalColumnsCount = parentDataView->GetColumnCount();

	for ( int i = 0;  i < totalColumnsCount; ++i) {
		wxDataViewColumn* column = parentDataView->GetColumn(i);
		if (column->IsHidden() == false) {
			totalVisibleColumnsCount++;
		}
	}

	return totalVisibleColumnsCount;
}

void DataViewCtrlHeaderMenu::OnShowColumns(wxCommandEvent&) {
	wxCommandEvent newEvent;

	newEvent.SetEventType(SHOW_ALL_COLUMNS_EVT);

	parentDataView->GetEventHandler()->ProcessEvent(newEvent);
}

wxEventType DataViewCtrlHeaderMenu::HIDE_COLUMN_EVT = wxNewEventType();
wxEventType DataViewCtrlHeaderMenu::SHOW_ALL_COLUMNS_EVT = wxNewEventType();
