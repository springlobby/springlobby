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
#include "gui/ui.h"

BEGIN_EVENT_TABLE(BattleDataViewCtrl, BaseDataViewCtrl)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(BATTLELIST_DATAVIEW_ID, BattleDataViewCtrl::OnContextMenu)
EVT_MENU(BATTLELIST_DATAVIEW_DLMAP, BattleDataViewCtrl::OnDLMap)
EVT_MENU(BATTLELIST_DATAVIEW_DLMOD, BattleDataViewCtrl::OnDLMod)
EVT_MENU(BATTLELIST_DATAVIEW_DLENGINE, BattleDataViewCtrl::OnDLEngine)
EVT_MENU(BATTLELIST_DATAVIEW_NOTIFYGAMEENDS, BattleDataViewCtrl::OnNotifyGameEnd)
END_EVENT_TABLE()

BattleDataViewCtrl::BattleDataViewCtrl(const wxString& dataViewName, wxWindow* parent)
    : BaseDataViewCtrl(dataViewName, parent, BATTLELIST_DATAVIEW_ID)
{
	m_popup = nullptr;

	BattleDataViewModel* m_BattleDataModel = new BattleDataViewModel();
	AssociateModel(m_BattleDataModel);

	const int DEFAULT_SIZE = wxCOL_WIDTH_AUTOSIZE;
	AppendBitmapColumn(_("S"), STATUS, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("C"), COUNTRY, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendBitmapColumn(_("R"), RANK, wxDATAVIEW_CELL_INERT, wxDVC_DEFAULT_MINWIDTH, wxALIGN_CENTER, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Description"), DESCRIPTION, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendIconTextColumn(_("Map"), MAP, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendIconTextColumn(_("Game"), GAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Host"), HOST, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Spectators"), SPECTATORS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Players"), PLAYERS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Max"), MAXIMUM, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Running"), RUNNING, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendIconTextColumn(_("Engine"), ENGINE, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	LoadColumnProperties();
}

BattleDataViewCtrl::~BattleDataViewCtrl()
{
	m_popup = nullptr;
}

void BattleDataViewCtrl::AddBattle(IBattle& battle)
{
	if (ContainsItem(battle)) {
		return;
	} else {
		AddItem(battle);
	}
}

void BattleDataViewCtrl::RemoveBattle(IBattle& battle)
{
	if (ContainsItem(battle)) {
		RemoveItem(battle);
	} else {
		return;
	}
}

void BattleDataViewCtrl::UpdateBattle(IBattle& battle)
{
	if (ContainsItem(battle)) {
		RefreshItem(battle);
	} else {
		return;
	}
}

void BattleDataViewCtrl::SetTipWindowText(const long /*item_hit*/,
					  const wxPoint& /*position*/)
{
	//TODO: implement!
}

void BattleDataViewCtrl::OnDLMap(wxCommandEvent& /*event*/)
{
	ui().NeedsDownload(GetSelectedItem(), false, DownloadEnum::CAT_MAP);
}

void BattleDataViewCtrl::OnDLMod(wxCommandEvent& /*event*/)
{
	ui().NeedsDownload(GetSelectedItem(), false, DownloadEnum::CAT_GAME);
}

void BattleDataViewCtrl::OnDLEngine(wxCommandEvent& /*event*/)
{
	ui().NeedsDownload(GetSelectedItem(), false, DownloadEnum::CAT_ENGINE);
}

void BattleDataViewCtrl::OnNotifyGameEnd(wxCommandEvent& /*unused*/)
{
	const IBattle* battle = GetSelectedItem();

	if (battle == nullptr) {
		return;
	}

	//Send user (belived to be autohost bot) private message with "!notify" command
	User founder = battle->GetFounder();
	founder.Say("!notify");
}

void BattleDataViewCtrl::OnContextMenu(wxDataViewEvent& /*event*/)
{

	const IBattle* battle = GetSelectedItem();

	if (battle == nullptr) {
		return;
	}

	const bool mod_missing = !battle->GameExists(false);
	const bool map_missing = !battle->MapExists(false);
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

	m_popup->Append(BATTLELIST_DATAVIEW_NOTIFYGAMEENDS, _("Notify me when game has ended"));

	PopupMenu(m_popup);
}
