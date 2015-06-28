/*
 * battledataviewmodel.h
 *
 *  Created on: 28 июня 2015 г.
 *      Author: Руслан
 */

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
		ENGINE
	};
};

#endif /* SRC_GUI_BATTLELIST_BATTLEDATAVIEWMODEL_H_ */
