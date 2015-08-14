/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_BATTLELIST_BATTLEDATAVIEWMODEL_H_
#define SRC_GUI_BATTLELIST_BATTLEDATAVIEWMODEL_H_

#include "gui/basedataviewmodel.h"

class IBattle;

class BattleDataViewModel: public BaseDataViewModel<IBattle> {
public:
	BattleDataViewModel();
	virtual ~BattleDataViewModel();

    virtual void GetValue( wxVariant &variant, const wxDataViewItem &item, unsigned int col ) const override;
    virtual int Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB, unsigned int column, bool ascending) const override;
    virtual bool GetAttr(const wxDataViewItem&, unsigned int, wxDataViewItemAttr&) const override;
    virtual wxString GetColumnType(unsigned int column) const override;

private:
	enum ColumnIndexes {
		STATUS = 0,
		COUNTRY,
		RANK,
		DESCRIPTION,
		MAP,
		GAME,
		HOST,
		SPECTATORS,
		PLAYERS,
		MAXIMUM,
		RUNNING,
		ENGINE,
		COLUMN_COUNT
	};
};

#endif /* SRC_GUI_BATTLELIST_BATTLEDATAVIEWMODEL_H_ */
