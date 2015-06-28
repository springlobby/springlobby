/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_BATTLELIST_BATTLEDATAVIEWCTTRL_H_
#define SRC_GUI_BATTLELIST_BATTLEDATAVIEWCTTRL_H_

#include "gui/basedataviewctrl.h"

class IBattle;
class wxWindow;
class wxString;
class wxMenu;
class wxCommandEvent;
class wxPoint;
class wxDataViewEvent;

class BattleDataViewCtrl: public BaseDataViewCtrl<IBattle> {
public:
	BattleDataViewCtrl(const wxString& dataViewName, wxWindow* parent);
	virtual ~BattleDataViewCtrl();

	void AddBattle(IBattle& battle);
	void RemoveBattle(IBattle& battle);
	void UpdateBattle(IBattle& battle);

	void SetTipWindowText(const long item_hit, const wxPoint& position);

private:
	void OnDLMap(wxCommandEvent& event);
	void OnDLMod(wxCommandEvent& event);
	void OnDLEngine(wxCommandEvent& event);
	void OnContextMenu(wxDataViewEvent& event);

private:
	wxMenu* m_popup;

public:
	enum {
		BATTLELIST_DATAVIEW_ID = wxID_HIGHEST,
		BATTLELIST_DATAVIEW_DLMOD,
		BATTLELIST_DATAVIEW_DLMAP,
		BATTLELIST_DATAVIEW_DLENGINE
	};

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

	DECLARE_EVENT_TABLE()
};

#endif /* SRC_GUI_BATTLELIST_BATTLEDATAVIEWCTTRL_H_ */
