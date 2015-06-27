/*
 * nickdataviewmodel.h
 *
 *  Created on: 27 июня 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_GUI_NICKDATAVIEWMODEL_H_
#define SRC_GUI_NICKDATAVIEWMODEL_H_

#include "basedataviewmodel.h"

class User;

class NickDataViewModel: public BaseDataViewModel<User> {
public:
	NickDataViewModel();
	virtual ~NickDataViewModel();

    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;
    virtual int Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB, unsigned int column, bool ascending) const override;

private:
	enum ColumnIndexes
	{
		STATUS = 0,
		COUNTRY,
		RANK,
		NICKNAME
	};
};

#endif /* SRC_GUI_NICKDATAVIEWMODEL_H_ */
