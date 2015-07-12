/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_BASEDATAVIEWCTRL_H_
#define SRC_GUI_BASEDATAVIEWCTRL_H_

#include <vector>

#include <wx/dataview.h>

#include "basedataviewmodel.h"

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

protected:
	BaseDataViewModel<DataType>* m_DataModel;
	wxString m_DataViewName;

	DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE_TEMPLATE1(BaseDataViewCtrl, wxDataViewCtrl, DataType)
END_EVENT_TABLE()

template<class DataType>
BaseDataViewCtrl<DataType>::BaseDataViewCtrl(const wxString& dataViewName, wxWindow* parent, wxWindowID id) : wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES) {
	m_DataModel = nullptr;
	m_DataViewName = dataViewName;
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
	}
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::SaveColumnProperties() {
	const int columnCount = GetColumnCount();

	for(int columnIndex = 0; columnIndex < columnCount; columnIndex++) {
		wxDataViewColumn* column = GetColumn(columnIndex);
		const int colWidth = column->GetWidth();
		sett().SetColumnWidth(m_DataViewName, columnIndex, colWidth);

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

#endif /* SRC_GUI_BASEDATAVIEWCTRL_H_ */
