/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "battleroomdataviewctrl.h"
#include "battleroomdataviewmodel.h"


BEGIN_EVENT_TABLE(BattleroomDataViewCtrl, BaseDataViewCtrl)

END_EVENT_TABLE()

BattleroomDataViewCtrl::BattleroomDataViewCtrl(const wxString& dataViewName, wxWindow* parent, IBattle* battle, bool readOnly, bool showInGame) :
	BaseDataViewCtrl(dataViewName, parent, BATTLEROOM_VIEW_ID){

	m_ViewIsReadOnly = readOnly;
	m_ShowInGame = showInGame;
	m_Battle = nullptr;

	const int DEFAULT_SIZE = -1;
	AppendBitmapColumn(_("Status"), STATUS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Ingame"), INGAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Faction"), FACTION, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Colour"), COLOUR, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Country"), COUNTRY, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("Rank"), RANK, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Nickname"), NICKNAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("TrueSkill"), TRUESKILL, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Team"), TEAM, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Ally"), ALLY, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Resource Bonus"), BONUS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	BattleroomDataViewModel* model = new BattleroomDataViewModel();
	AssociateModel(model);

	//Hide "ingame" column if not needed
	if (showInGame == false)
	{
		GetColumn(INGAME)->SetHidden(true);
	}
	//ReadOnly means quick info about battle
	if (readOnly == false)
	{
		//TODO: implement!
	}

	LoadColumnProperties();
}

BattleroomDataViewCtrl::~BattleroomDataViewCtrl() {
	//TODO: implement
//	wxDELETE(m_PopupMenu);
	m_Battle = nullptr;

	SaveColumnProperties();
}

void BattleroomDataViewCtrl::SetBattle(IBattle* battle) {
	//TODO: implement
	//Remove all users from view (if any)
	Clear();

	m_Battle = battle;
	//Inform model about new battle
	static_cast<BattleroomDataViewModel*>(GetModel())->SetBattle(battle);
}

void BattleroomDataViewCtrl::AddUser(User& user) {
	//TODO: implement
	if (m_DataModel->ContainsItem(user) == true) {
		return;
	}
	m_DataModel->AddItem(user);
}

void BattleroomDataViewCtrl::RemoveUser(User& user) {
	//TODO: implement
	if (m_DataModel->ContainsItem(user) == false) {
		return;
	}
	m_DataModel->RemoveItem(user);
}

void BattleroomDataViewCtrl::SetTipWindowText(const long item_hit,
		const wxPoint& position) {
	//TODO: implement
}

void BattleroomDataViewCtrl::UpdateUser(User& user) {
	wxASSERT(m_DataModel != nullptr);

	m_DataModel->UpdateItem(user);
}
