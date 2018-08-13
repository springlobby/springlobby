/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_HOSTING_BATTLEROOMDATAVIEWMODEL_H_
#define SRC_GUI_HOSTING_BATTLEROOMDATAVIEWMODEL_H_

#include "gui/basedataviewmodel.h"
class User;
class IBattle;

class BattleroomDataViewModel : public BaseDataViewModel<User>
{
public:
	BattleroomDataViewModel();
	virtual ~BattleroomDataViewModel();

	virtual void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const override;
	virtual int Compare(const wxDataViewItem& itemA, const wxDataViewItem& itemB, unsigned int column, bool ascending) const override;
	virtual bool GetAttr(const wxDataViewItem&, unsigned int, wxDataViewItemAttr&) const override;
	virtual wxString GetColumnType(unsigned int column) const override;

	IBattle* GetBattle() const;
	void SetBattle(IBattle*);

private:
	IBattle* m_Battle;

private:
	enum ColumnIndexes {
		STATUS = 0,
		INGAME,
		FACTION,
		COLOUR,
		COUNTRY,
		RANK,
		NICKNAME,
		TRUESKILL,
		TEAM,
		ALLY,
		BONUS,
		COLUMN_COUNT
	};
};

#endif /* SRC_GUI_HOSTING_BATTLEROOMDATAVIEWMODEL_H_ */
