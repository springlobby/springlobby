/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

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
