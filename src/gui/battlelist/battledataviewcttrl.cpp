/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "battledataviewcttrl.h"

#include "battledataviewmodel.h"

BEGIN_EVENT_TABLE(BattleDataViewCtrl, BaseDataViewCtrl)
	EVT_MENU(BATTLELIST_DATAVIEW_DLMAP, BattleDataViewCtrl::OnDLMap)
	EVT_MENU(BATTLELIST_DATAVIEW_DLMOD, BattleDataViewCtrl::OnDLMod)
	EVT_MENU(BATTLELIST_DATAVIEW_DLENGINE, BattleDataViewCtrl::OnDLEngine)
END_EVENT_TABLE()

BattleDataViewCtrl::BattleDataViewCtrl(const wxString& dataViewName, wxWindow* parent) : BaseDataViewCtrl(dataViewName, parent, BATTLELIST_DATAVIEW_ID) {
	m_popup = nullptr;

	const int DEFAULT_SIZE = wxDVC_DEFAULT_WIDTH;
	AppendBitmapColumn(_("Status"), STATUS, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Country"), COUNTRY, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Rank"), RANK, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Description"), DESCRIPTION, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendIconTextColumn(_("Map"), MAP, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendIconTextColumn(_("Game"), GAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Host"), HOST, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Spectators"), SPECTATORS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Players"), PLAYERS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Max"), MAXIMUM, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Running"), RUNNING, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendIconTextColumn(_("Engine"), ENGINE, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	BattleDataViewModel* m_BattleDataModel = new BattleDataViewModel();
	AssociateModel(m_BattleDataModel);

	LoadColumnProperties();
}

BattleDataViewCtrl::~BattleDataViewCtrl() {
}

void BattleDataViewCtrl::AddBattle(IBattle& battle) {
	if (ContainsItem(battle)) {
		return;
	} else {
		m_DataModel->AddItem(battle);
	}
}

void BattleDataViewCtrl::RemoveBattle(IBattle& battle) {
	if (ContainsItem(battle)) {
		m_DataModel->RemoveItem(battle);
	} else {
		return;
	}
}

void BattleDataViewCtrl::UpdateBattle(IBattle& battle) {
	if (ContainsItem(battle)) {
		RefreshItem(battle);
	} else {
		return;
	}
}

void BattleDataViewCtrl::SetTipWindowText(const long /*item_hit*/,
		const wxPoint& /*position*/) {
	//TODO: implement!
}

void BattleDataViewCtrl::OnDLMap(wxCommandEvent& /*event*/) {
}

void BattleDataViewCtrl::OnDLMod(wxCommandEvent& /*event*/) {
}

void BattleDataViewCtrl::OnDLEngine(wxCommandEvent& /*event*/) {
}
