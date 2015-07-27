/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_BASEDATAVIEWCTRL_H_
#define SRC_GUI_BASEDATAVIEWCTRL_H_

#include <vector>

#include <wx/dataview.h>

#include "basedataviewmodel.h"
#include "dataviewvtrlheadermenu.h"

#include "settings.h"
#include "utils/slconfig.h"

template<class DataType>
class BaseDataViewCtrl: public wxDataViewCtrl {
public:
	BaseDataViewCtrl(const wxString&, wxWindow*, wxWindowID);
	virtual ~BaseDataViewCtrl();

	bool AssociateModel(BaseDataViewModel<DataType>*);

	void Resort();
	void RefreshItem(const DataType&);
	bool ContainsItem(const DataType&);
	int GetItemsCount() const;
	void Clear();
	const DataType* GetSelectedItem();
	std::list<const DataType*>& GetItemsContainer();

protected:
	virtual void LoadColumnProperties();
	virtual void SaveColumnProperties();

private:
	void OnColumnHeaderContext(wxDataViewEvent&);
	void OnHideColumn(wxCommandEvent&);
	void OnShowColumns(wxCommandEvent&);

protected:
	BaseDataViewModel<DataType>* m_DataModel;
	wxString m_DataViewName;

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE_TEMPLATE1(BaseDataViewCtrl, wxDataViewCtrl, DataType)
//	EVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK(wxID_ANY, OnColumnHeaderContext)
END_EVENT_TABLE()

template<class DataType>
BaseDataViewCtrl<DataType>::BaseDataViewCtrl(const wxString& dataViewName, wxWindow* parent, wxWindowID id) : wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES) {
	m_DataModel = nullptr;
	m_DataViewName = dataViewName;

	/*Link events and handlers*/
	Connect(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, wxObjectEventFunction(&BaseDataViewCtrl::OnColumnHeaderContext));
	Connect(DataViewCtrlHeaderMenu::HIDE_COLUMN_EVT, wxObjectEventFunction(&BaseDataViewCtrl::OnHideColumn));
	Connect(DataViewCtrlHeaderMenu::SHOW_ALL_COLUMNS_EVT, wxObjectEventFunction(&BaseDataViewCtrl::OnShowColumns));
}

template<class DataType>
BaseDataViewCtrl<DataType>::~BaseDataViewCtrl() {
	SaveColumnProperties();
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::Resort() {
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->Resort();
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::RefreshItem(const DataType& item) {
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->UpdateItem(item);
}

template<class DataType>
inline bool BaseDataViewCtrl<DataType>::ContainsItem(const DataType& item) {
	wxASSERT(m_DataModel != nullptr);
	return m_DataModel->ContainsItem(item);
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::Clear() {
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->Clear();
}

template<class DataType>
inline bool BaseDataViewCtrl<DataType>::AssociateModel(BaseDataViewModel<DataType>* model) {
	m_DataModel = model;

	return wxDataViewCtrl::AssociateModel(model);
}

template<class DataType>
inline const DataType* BaseDataViewCtrl<DataType>::GetSelectedItem() {
	wxDataViewItem item = GetSelection();

	if (item.IsOk() == false) {
		return nullptr;
	} else {
		return (static_cast<DataType*>(item.GetID()));
	}
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::LoadColumnProperties() {
	const int columnCount = GetColumnCount();

	//Set up sorting column
	int sortingColumnIndex;
	cfg().Read(wxString(m_DataViewName + _T("/sorting_column")), &sortingColumnIndex, -1);
	//Set up sorting order
	bool sortOrderAscending;
	cfg().Read(wxString(m_DataViewName + _T("/sorting_order")), &sortOrderAscending, true);

	//Loop through existing columns and set their sizes and sort orders
	for(int columnIndex = 0; columnIndex < columnCount; columnIndex++)
	{
		//Set up column's width
		const int colWidth = sett().GetColumnWidth(m_DataViewName, columnIndex);
		const bool isHidden = sett().GetColumnVisibility(m_DataViewName, columnIndex);
		wxDataViewColumn* column = GetColumn(columnIndex);
		if (colWidth >= (wxDVC_DEFAULT_MINWIDTH - 5)) {
			column->SetWidth(colWidth);
		}
		//Set up column's sort order (if any)
		if (columnIndex == sortingColumnIndex) {
			column->SetSortOrder(sortOrderAscending);
		} else {
			/*column->UnsetAsSortKey(); <- this method breaks sorting of any column that was or will be set as sort key! (wxMSW3.0)*/
		}
		//Set column's visibility
		column->SetHidden(isHidden);
	}
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::SaveColumnProperties() {
	const int columnCount = GetColumnCount();

	for(int columnIndex = 0; columnIndex < columnCount; columnIndex++) {
		wxDataViewColumn* column = GetColumn(columnIndex);
		const int colWidth = column->GetWidth();
		sett().SetColumnWidth(m_DataViewName, columnIndex, colWidth);
		sett().SetColumnVisibility(m_DataViewName, columnIndex, column->IsHidden());
		if (column->IsSortKey()) {
			//Save sorting column
			cfg().Write(wxString(m_DataViewName + _T("/sorting_column")), columnIndex);
			//Save sorting order
			const bool sortOrderAscending = column->IsSortOrderAscending();
			cfg().Write(wxString(m_DataViewName + _T("/sorting_order")), sortOrderAscending);
		}
	}
}

template<class DataType>
inline int BaseDataViewCtrl<DataType>::GetItemsCount() const {
	wxASSERT(m_DataModel != nullptr);

	return m_DataModel->GetItemsCount();
}

template<class DataType>
inline std::list<const DataType*>& BaseDataViewCtrl<DataType>::GetItemsContainer() {
	return m_DataModel->GetItemsContainer();
}


template<class DataType>
inline void BaseDataViewCtrl<DataType>::OnColumnHeaderContext(
		wxDataViewEvent& event) {

	int columnIndex = event.GetColumn();

	wxMenu* menu = new DataViewCtrlHeaderMenu(this, columnIndex);

	PopupMenu(menu);

	delete menu;
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::OnHideColumn(wxCommandEvent& event) {
	unsigned int columnIndex = static_cast<unsigned int>(event.GetExtraLong());

	wxASSERT(columnIndex < GetColumnCount());

	wxDataViewColumn* column = GetColumn(columnIndex);

	wxASSERT(column != nullptr);
	wxASSERT(column->IsHidden() == false);

	column->SetHidden(true);
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::OnShowColumns(wxCommandEvent&) {
	int totalColumnsCount = GetColumnCount();

	for ( int i = 0;  i < totalColumnsCount; ++i) {
		wxDataViewColumn* column = GetColumn(i);
		if (column->IsHidden()) {
			column->SetHidden(false);
		}
	}

}

#endif /* SRC_GUI_BASEDATAVIEWCTRL_H_ */
