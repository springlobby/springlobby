/*
 * BaseDataViewModel.h
 *
 *  Created on: 20 июня 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_GUI_BASEDATAVIEWMODEL_H_
#define SRC_GUI_BASEDATAVIEWMODEL_H_

#include <wx/dataview.h>
#include <list>

template<class DataType>
class BaseDataViewModel : public wxDataViewModel {
public:
	BaseDataViewModel();
	virtual ~BaseDataViewModel();

public:
	//Overriden methods from wxDataViewModel
    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const override;
    virtual unsigned int GetChildren( const wxDataViewItem &item, wxDataViewItemArray &children ) const override;
    virtual int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column, bool ascending ) const override;
    virtual bool IsListModel() const override;

public:
    //Custom methods
    bool AddItem(const DataType&);
    bool RemoveItem(const DataType&);
    bool ContainsItem(const DataType&);
    void Clear();
    void UpdateItem(const DataType&);

public:
    //These methods from wxDataViewModel does not require to be overriden in derived classes
    unsigned int GetColumnCount() const override;
    bool IsContainer( const wxDataViewItem &item ) const override;
    bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) override;
    wxString GetColumnType( unsigned int col ) const override;

private:
    std::list<const DataType*> m_ModelData;
};

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Implementation ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template<class DataType>
BaseDataViewModel<DataType>::BaseDataViewModel() {
	// TODO Auto-generated constructor stub

}

template<class DataType>
BaseDataViewModel<DataType>::~BaseDataViewModel() {
	// TODO Auto-generated destructor stub
}

template<class DataType>
void BaseDataViewModel<DataType>::GetValue(wxVariant& variant, const wxDataViewItem& item,
		unsigned int col) const {
	throw new std::exception();
}

template<class DataType>
int BaseDataViewModel<DataType>::Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                     unsigned int column, bool ascending ) const
{
	return wxDataViewModel::Compare(item1, item2, column, ascending);
}

template<class DataType>
wxDataViewItem BaseDataViewModel<DataType>::GetParent(const wxDataViewItem& item) const {
	return wxDataViewItem(nullptr); //Root is only one possible parent
}

template<class DataType>
unsigned int BaseDataViewModel<DataType>::GetChildren(const wxDataViewItem& item,
		wxDataViewItemArray& children) const {
	if (item.IsOk() == true) { //Return items only for root!
		return 0;
	} else {

		for(auto dataItem : m_ModelData) {
			children.Add(wxDataViewItem(const_cast<DataType*>(dataItem)));
		}
		return children.GetCount();
	}
}

template<class DataType>
bool BaseDataViewModel<DataType>::AddItem(const DataType& data) {
	m_ModelData.push_back(&data);

	//Inform model about new item
	ItemAdded(wxDataViewItem(nullptr), wxDataViewItem(const_cast<DataType*>(&data)));
	return true;
}

template<class DataType>
bool BaseDataViewModel<DataType>::RemoveItem(const DataType& data) {
	m_ModelData.remove(&data);

	//Inform model about deleted item
	ItemDeleted(wxDataViewItem(nullptr), wxDataViewItem(const_cast<DataType*>(&data)));
	return true;
}

template<class DataType>
unsigned int BaseDataViewModel<DataType>::GetColumnCount() const {
	return 0; //This value does not matters
}

template<class DataType>
bool BaseDataViewModel<DataType>::IsContainer(const wxDataViewItem& item) const {
	if (item.IsOk() == true) { //Only root can containt items
		return false;
	} else {
		return true;
	}
}

template<class DataType>
bool BaseDataViewModel<DataType>::SetValue(const wxVariant& variant,
		const wxDataViewItem& item, unsigned int col) {
	return true; //Dymmy method
}

//Needed to inform model that there will be no expanders (make view looks like list)
template<class DataType>
inline bool BaseDataViewModel<DataType>::IsListModel() const {
	return true;
}

template<class DataType>
inline bool BaseDataViewModel<DataType>::ContainsItem(const DataType& checkedItem) {
	const DataType* checkItemPointer = &checkedItem;
	for(const DataType* item : m_ModelData)
	{
		if (item == checkItemPointer) {
			return true;
		}
	}
	return false;
}

template<class DataType>
inline void BaseDataViewModel<DataType>::Clear() {
	m_ModelData.clear();
	Cleared();
}

template<class DataType>
inline void BaseDataViewModel<DataType>::UpdateItem(const DataType& item) {
	if (ContainsItem(item) == true) {
		//FIXME: Maybe update item in m_ModelData. At this moment it stores pointer and does not need to be updated
		ItemChanged(wxDataViewItem(const_cast<DataType*>(&item)));
	} else {
		return;
	}
}

template<class DataType>
wxString BaseDataViewModel<DataType>::GetColumnType(unsigned int col) const {
	return wxEmptyString; //Does nothing
}

#endif /* SRC_GUI_BASEDATAVIEWMODEL_H_ */
