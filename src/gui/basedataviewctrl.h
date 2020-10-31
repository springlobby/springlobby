/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_BASEDATAVIEWCTRL_H_
#define SRC_GUI_BASEDATAVIEWCTRL_H_

#include <wx/dataview.h>
#include <vector>
#include "basedataviewmodel.h"
#include "dataviewvtrlheadermenu.h"
#include "settings.h"
#include "utils/slconfig.h"
template <class DataType>
class BaseDataViewCtrl : public wxDataViewCtrl
{
public:
	BaseDataViewCtrl(const wxString&, wxWindow*, wxWindowID);
	virtual ~BaseDataViewCtrl();

	using wxDataViewCtrl::AssociateModel;
	virtual bool AssociateModel(BaseDataViewModel<DataType>*);

	virtual void Resort();

	virtual bool AddItem(const DataType&, bool resortIsNeeded = true);
	virtual bool RemoveItem(const DataType&);
	virtual bool RefreshItem(const DataType&);
	virtual bool ContainsItem(const DataType&);
	virtual int GetItemsCount() const;
	virtual void Clear();
	virtual DataType* GetSelectedItem();
	virtual const std::set<const DataType*>& GetItemsContainer() const;

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

template <class DataType>
BaseDataViewCtrl<DataType>::BaseDataViewCtrl(const wxString& dataViewName, wxWindow* parent, wxWindowID id)
    : wxDataViewCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES)
{
	m_DataModel = nullptr;
	m_DataViewName = dataViewName;

	/*Link events and handlers*/
	Connect(wxEVT_COMMAND_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, wxObjectEventFunction(&BaseDataViewCtrl::OnColumnHeaderContext));
	Connect(DataViewCtrlHeaderMenu::HIDE_COLUMN_EVT, wxObjectEventFunction(&BaseDataViewCtrl::OnHideColumn));
	Connect(DataViewCtrlHeaderMenu::SHOW_ALL_COLUMNS_EVT, wxObjectEventFunction(&BaseDataViewCtrl::OnShowColumns));
}

template <class DataType>
BaseDataViewCtrl<DataType>::~BaseDataViewCtrl()
{
	SaveColumnProperties();
	Clear();
	wxDataViewCtrl::AssociateModel(nullptr);
	delete m_DataModel;
	m_DataModel = nullptr;
}

template <class DataType>
inline void BaseDataViewCtrl<DataType>::Resort()
{
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->Resort();
}

template <class DataType>
inline bool BaseDataViewCtrl<DataType>::RefreshItem(const DataType& item)
{
	wxASSERT(m_DataModel != nullptr);

	wxDataViewItem selectedItem = GetSelection();

	bool result = m_DataModel->UpdateItem(item);

	if (result) {
		Resort();
	}

	/*Preserve selection*/
	Select(selectedItem);

	return result;
}

template <class DataType>
inline bool BaseDataViewCtrl<DataType>::ContainsItem(const DataType& item)
{
	wxASSERT(m_DataModel != nullptr);
	return m_DataModel->ContainsItem(item);
}

template <class DataType>
inline void BaseDataViewCtrl<DataType>::Clear()
{
	wxASSERT(m_DataModel != nullptr);

	UnselectAll();

	m_DataModel->Clear();
}

template <class DataType>
inline bool BaseDataViewCtrl<DataType>::AssociateModel(BaseDataViewModel<DataType>* model)
{
	m_DataModel = model;

	return wxDataViewCtrl::AssociateModel(model);
}

template <class DataType>
inline DataType* BaseDataViewCtrl<DataType>::GetSelectedItem()
{

	/*Prevent from returning undefined object if model is empty*/
	if (GetItemsCount() == 0) {
		return nullptr;
	}

	wxDataViewItem item = GetSelection();

	if (!item.IsOk()) {
		return nullptr;
	} else {
		DataType* dataItem = static_cast<DataType*>(item.GetID());

		if (ContainsItem(*dataItem)) {
			return dataItem;
		}

		return nullptr;
	}
}

template <class DataType>
inline void BaseDataViewCtrl<DataType>::LoadColumnProperties()
{
	const int columnCount = GetColumnCount();

	//Set up sorting column
	int sortingColumnIndex;
	cfg().Read(wxString(m_DataViewName + _T("/sorting_column")), &sortingColumnIndex, -1);
	//Set up sorting order
	bool sortOrderAscending;
	cfg().Read(wxString(m_DataViewName + _T("/sorting_order")), &sortOrderAscending, true);

	//Loop through existing columns and set their sizes and sort orders
	for (int columnIndex = 0; columnIndex < columnCount; columnIndex++) {
		//Set up column's width
		wxDataViewColumn* column = GetColumn(columnIndex);
		const int colWidth = sett().GetColumnWidth(m_DataViewName, columnIndex);
		const bool isHidden = sett().IsColumnHidden(m_DataViewName, columnIndex, column->IsHidden());

		//WORKAROUND!
		//This weird code prevents columns in wxDataViewCtrl under Linux to loose ability to resize
		//https://groups.google.com/forum/#!searchin/wx-users/wxDataViewCtrl/wx-users/8khN3gsYWcg/_hOa30I6dqoJ
		//Hope this will be fixed someday...
		column->SetResizeable(true);

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

template <class DataType>
inline void BaseDataViewCtrl<DataType>::SaveColumnProperties()
{
	const int columnCount = GetColumnCount();

	for (int columnIndex = 0; columnIndex < columnCount; columnIndex++) {
		wxDataViewColumn* column = GetColumn(columnIndex);
		//Get column width
		const size_t colWidth = column->GetWidth();
#ifdef _WIN32
		//If column's width was set to wxCOL_WIDTH_AUTOSIZE then column->GetWidth()
		//will return calculated value (that will be taken from wxDataViewCtrl::GetBestColumnWidth()).
		//This is not acceptable here because at this moment control is empty (has no items) and
		//calculation will take in account only header's caption. This is not what user expected to see after restart.
		//Just do not save such column's width and let application to calculate it again after new launch.
		if (colWidth != this->GetBestColumnWidth(columnIndex)) {
			//Save column's width if it is not wxCOL_WIDTH_AUTOSIZE
			sett().SetColumnWidth(m_DataViewName, columnIndex, colWidth);
		}
#else
		sett().SetColumnWidth(m_DataViewName, columnIndex, colWidth);
#endif
		//Save other properties
		sett().SetColumnHidden(m_DataViewName, columnIndex, column->IsHidden());
		if (column->IsSortKey()) {
			//Save sorting column
			cfg().Write(wxString(m_DataViewName + _T("/sorting_column")), columnIndex);
			//Save sorting order
			const bool sortOrderAscending = column->IsSortOrderAscending();
			cfg().Write(wxString(m_DataViewName + _T("/sorting_order")), sortOrderAscending);
		}
	}
}

template <class DataType>
inline int BaseDataViewCtrl<DataType>::GetItemsCount() const
{
	wxASSERT(m_DataModel != nullptr);

	return m_DataModel->GetItemsCount();
}

template <class DataType>
inline const std::set<const DataType*>& BaseDataViewCtrl<DataType>::GetItemsContainer() const
{
	return m_DataModel->GetItemsContainer();
}


template <class DataType>
inline void BaseDataViewCtrl<DataType>::OnColumnHeaderContext(
    wxDataViewEvent& event)
{

	int columnIndex = event.GetColumn();
	if (columnIndex < 0) {
		wxDataViewColumn* column = event.GetDataViewColumn();
		if (column == nullptr) {
			/*Looks like this function is unsupported*/
			return;
		}

		columnIndex = GetColumnPosition(column);
		if (columnIndex < 0) {
			/*Looks like this function is unsupported*/
			return;
		}
	}

	if (static_cast<unsigned int>(columnIndex) >= m_DataModel->GetColumnCount()) {
		wxLogWarning(_T("BaseDataViewCtrl<DataType>::OnColumnHeaderContext() :  event.GetColumn() returned invalid index"));
		return;
	}

	wxMenu* menu = new DataViewCtrlHeaderMenu(this, columnIndex);

	PopupMenu(menu);

	delete menu;
}

template <class DataType>
inline void BaseDataViewCtrl<DataType>::OnHideColumn(wxCommandEvent& event)
{
	unsigned int columnIndex = static_cast<unsigned int>(event.GetExtraLong());

	if (columnIndex >= GetColumnCount()) {
		wxASSERT(false);
		wxLogWarning(_T("BaseDataViewCtrl<DataType>::OnHideColumn() : columnIndex >= GetColumnCount()"));
		return;
	}

	wxDataViewColumn* column = GetColumn(columnIndex);

	if (column == nullptr) {
		wxASSERT(false);
		wxLogWarning(_T("BaseDataViewCtrl<DataType>::OnHideColumn() : column == nullptr"));
		return;
	}

	if (column->IsHidden()) {
		wxASSERT(false);
		wxLogWarning(_T("BaseDataViewCtrl<DataType>::OnHideColumn() : column->IsHidden()"));
		return;
	}

	column->SetHidden(true);
}

template <class DataType>
inline void BaseDataViewCtrl<DataType>::OnShowColumns(wxCommandEvent&)
{
	int totalColumnsCount = GetColumnCount();

	for (int i = 0; i < totalColumnsCount; ++i) {
		wxDataViewColumn* column = GetColumn(i);
		if (column->IsHidden()) {
			column->SetHidden(false);
		}
	}
}

template <class DataType>
inline bool BaseDataViewCtrl<DataType>::AddItem(const DataType& item, bool resortIsNeeded)
{
	wxDataViewItem selectedItem = GetSelection();

	bool result = m_DataModel->AddItem(item);

	if (result && resortIsNeeded) {
		Resort();
	}

	/*Preserve selection*/
	Select(selectedItem);

	return result;
}

template <class DataType>
inline bool BaseDataViewCtrl<DataType>::RemoveItem(const DataType& item)
{
	wxDataViewItem selectedItem = GetSelection();

	bool result = m_DataModel->RemoveItem(item);

	if (result) {
		Resort();
	}

	//Do no try to select removed item
	if ((selectedItem.GetID()) != (&item)) {
		Select(selectedItem);
	}

	return result;
}

#endif /* SRC_GUI_BASEDATAVIEWCTRL_H_ */
