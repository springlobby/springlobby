/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "battledataviewcttrl.h"

#include <wx/menu.h>

#include "battledataviewmodel.h"
#include "utils/slpaths.h"
#include "servermanager.h"
#include "ibattle.h"
#include "downloader/prdownloader.h"
#include "log.h"
#include "exception.h"
#include "contentmanager.h"
#include "contentdownloadrequest.h"

BEGIN_EVENT_TABLE(BattleDataViewCtrl, BaseDataViewCtrl)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(BATTLELIST_DATAVIEW_ID, BattleDataViewCtrl::OnContextMenu)
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
		AddItem(battle);
	}
}

void BattleDataViewCtrl::RemoveBattle(IBattle& battle) {
	if (ContainsItem(battle)) {
		RemoveItem(battle);
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
	const IBattle* battle = GetSelectedItem();

	if (battle == nullptr) {
		return;
	} else {
		ContentDownloadRequest req;
		req.MapRequired(battle->GetHostMapName(), battle->GetHostMapHash());
		try{
			ContentManager::Instance()->DownloadContent(req);
		}catch(Exception& e) {
			wxLogError(_("Failed to download map: ") + e.Reason());
		}
	}
}

void BattleDataViewCtrl::OnDLMod(wxCommandEvent& /*event*/) {
	const IBattle* battle = GetSelectedItem();

	if (battle == nullptr) {
		return;
	} else {
		ContentDownloadRequest req;
		req.GameRequired(battle->GetHostGameName(), battle->GetHostGameHash());
		try{
			ContentManager::Instance()->DownloadContent(req);
		}catch(Exception& e) {
			wxLogError(_("Failed to download mod: ") + e.Reason());
		}
	}
}

void BattleDataViewCtrl::OnDLEngine(wxCommandEvent& /*event*/) {
	const IBattle* battle = GetSelectedItem();

	if (battle == nullptr) {
		return;
	} else {
		ContentDownloadRequest req;
		req.EngineRequired(battle->GetEngineVersion());
		try{
			ContentManager::Instance()->DownloadContent(req);
		}catch(Exception& e) {
			wxLogError(_("Failed to download engine: ") + e.Reason());
		}
	}
}

void BattleDataViewCtrl::OnContextMenu(wxDataViewEvent& /*event*/) {

	const IBattle* battle = GetSelectedItem();

	if (battle == nullptr) {
		return;
	}

	const bool mod_missing = battle->GameExists(false) == false;
	const bool map_missing = battle->MapExists(false) == false;
	const bool engine_missing = SlPaths::GetCompatibleVersion(battle->GetEngineVersion()).empty();

	m_popup = new wxMenu(wxEmptyString);

	if (map_missing) {
		m_popup->Append(BATTLELIST_DATAVIEW_DLMAP, _("Download &map"));
	}

	if (mod_missing) {
		m_popup->Append(BATTLELIST_DATAVIEW_DLMOD, _("Download &game"));
	}

	if (engine_missing) {
		m_popup->Append(BATTLELIST_DATAVIEW_DLENGINE, _("Download &engine"));
	}

	if (map_missing || mod_missing || engine_missing) {
		PopupMenu(m_popup);
	}
}
