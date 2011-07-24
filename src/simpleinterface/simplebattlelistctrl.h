#ifndef SIMPLEBATTLELISTCTRL_H
#define SIMPLEBATTLELISTCTRL_H

#include "../customvirtlistctrl.h"
#include "../utils/battleevents.h"

class wxMenu;
class IBattle;
class wxListEvent;
class wxCommandEvent;
class Ui;

class SimpleBattlelistCtrl : public CustomVirtListCtrl< IBattle *, SimpleBattlelistCtrl>
{
public:
	SimpleBattlelistCtrl(wxWindow* parent );
	~SimpleBattlelistCtrl(){}

	void AddBattle( BattleEvents::BattleEventData data );
	void RemoveBattle( BattleEvents::BattleEventData data );
	void UpdateBattle( BattleEvents::BattleEventData data );

	//these are overloaded to use list in virtual style
	wxString GetItemText(long item, long column) const;
	int GetItemImage(long item) const;
	int GetItemColumnImage(long item, long column) const;
	wxListItemAttr * GetItemAttr(long item) const;

protected:
	int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
	int GetIndexFromData( const DataType& data ) const;
	virtual void Sort();

	EventReceiverFunc<SimpleBattlelistCtrl, BattleEvents::BattleEventData, &SimpleBattlelistCtrl::AddBattle>
		m_addSink;
	EventReceiverFunc<SimpleBattlelistCtrl, BattleEvents::BattleEventData, &SimpleBattlelistCtrl::RemoveBattle>
		m_removeSink;
	EventReceiverFunc<SimpleBattlelistCtrl, BattleEvents::BattleEventData, &SimpleBattlelistCtrl::UpdateBattle>
		m_updateSink;

	enum {
		SI_BLIST_LIST = wxID_HIGHEST
	};

	DECLARE_EVENT_TABLE()

};

#endif // SIMPLEBATTLELISTCTRL_H

/**
	This file is part of SpringLobby,
	Copyright (C) 2007-2011

	SpringLobby is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 2 as published by
	the Free Software Foundation.

	SpringLobby is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
