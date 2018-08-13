/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "battledataviewctrl.h"

#include <wx/menu.h>

#include "battledataviewmodel.h"
#include "downloader/prdownloader.h"
#include "exception.h"
#include "gui/iconscollection.h"
#include "gui/ui.h"
#include "ibattle.h"
#include "log.h"
#include "servermanager.h"
#include "utils/slpaths.h"

BEGIN_EVENT_TABLE(BattleDataViewCtrl, BaseDataViewCtrl)
EVT_DATAVIEW_ITEM_CONTEXT_MENU(BATTLELIST_DATAVIEW_ID, BattleDataViewCtrl::OnContextMenu)
EVT_MENU(BATTLELIST_DATAVIEW_DL_ENGINE, BattleDataViewCtrl::OnDLEngine)
EVT_MENU(BATTLELIST_DATAVIEW_DL_GAME, BattleDataViewCtrl::OnDLGame)
EVT_MENU(BATTLELIST_DATAVIEW_DL_MAP, BattleDataViewCtrl::OnDLMap)
EVT_MENU(BATTLELIST_DATAVIEW_NOTIFY_WHEN_BATTLE_ENDS, BattleDataViewCtrl::OnNotifyWhenBattleEnds)
END_EVENT_TABLE()

BattleDataViewCtrl::BattleDataViewCtrl(const wxString& dataViewName, wxWindow* parent)
    : BaseDataViewCtrl(dataViewName, parent, BATTLELIST_DATAVIEW_ID)
{
	m_popup = nullptr;

	BattleDataViewModel* m_BattleDataModel = new BattleDataViewModel();
	AssociateModel(m_BattleDataModel);

	const IconsCollection& ici = *IconsCollection::Instance();
	const wxDataViewCellMode& cm = wxDATAVIEW_CELL_INERT;
	const int gds = wxDVC_DEFAULT_MINWIDTH; // graphical default size
	const int asds = wxCOL_WIDTH_AUTOSIZE; // autosize default size
	const int flags = wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE;
	const int flags_hidden = flags | wxDATAVIEW_COL_HIDDEN;

	AppendBitmapColumn(wxEmptyString,   STATUS,      cm, gds, wxALIGN_CENTER, flags);
	AppendBitmapColumn(wxEmptyString,   COUNTRY,     cm, gds, wxALIGN_CENTER, flags);
	AppendBitmapColumn(wxEmptyString,   RANK,        cm, gds, wxALIGN_CENTER, flags_hidden);
	AppendTextColumn(ici.BMP_BROOM,     PLAYERS,     cm, gds, wxALIGN_NOT, flags);
	AppendTextColumn(_("Max"),          MAXIMUM,     cm, asds, wxALIGN_NOT, flags);
	AppendTextColumn(ici.BMP_SPECTATOR, SPECTATORS,  cm, gds,  wxALIGN_NOT, flags);
	AppendTextColumn(_("Running"),      RUNNING,     cm, asds, wxALIGN_NOT, flags_hidden);
	AppendTextColumn(_("Battle Name"),  DESCRIPTION, cm, asds, wxALIGN_NOT, flags);
	AppendIconTextColumn(_("Game"),     GAME,        cm, asds, wxALIGN_NOT, flags);
	AppendIconTextColumn(_("Map"),      MAP,         cm, asds, wxALIGN_NOT, flags);
	AppendTextColumn(_("Host"),         HOST,        cm, asds, wxALIGN_NOT, flags_hidden);
	AppendIconTextColumn(_("Engine"),   ENGINE,      cm, asds, wxALIGN_NOT, flags_hidden);

	LoadColumnProperties();
}

BattleDataViewCtrl::~BattleDataViewCtrl()
{
	m_popup = nullptr;
}

void BattleDataViewCtrl::AddBattle(IBattle& battle)
{
	if (! ContainsItem(battle))
		AddItem(battle);
}

void BattleDataViewCtrl::RemoveBattle(IBattle& battle)
{
	if (ContainsItem(battle))
		RemoveItem(battle);
}

void BattleDataViewCtrl::UpdateBattle(IBattle& battle)
{
	if (ContainsItem(battle))
		RefreshItem(battle);
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

void BattleDataViewCtrl::OnDLGame(wxCommandEvent& /*event*/)
{
	ui().NeedsDownload(GetSelectedItem(), false, DownloadEnum::CAT_GAME);
}

void BattleDataViewCtrl::OnDLEngine(wxCommandEvent& /*event*/)
{
	ui().NeedsDownload(GetSelectedItem(), false, DownloadEnum::CAT_ENGINE);
}

void BattleDataViewCtrl::OnNotifyWhenBattleEnds(wxCommandEvent& /*unused*/)
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

	const bool game_missing = !battle->GameExists(false);
	const bool map_missing = !battle->MapExists(false);
	const bool engine_missing = SlPaths::GetCompatibleVersion(battle->GetEngineVersion()).empty();

	m_popup = new wxMenu(wxEmptyString);

	if (map_missing) {
		m_popup->Append(BATTLELIST_DATAVIEW_DL_MAP, _("Download &map"));
	}

	if (game_missing) {
		m_popup->Append(BATTLELIST_DATAVIEW_DL_GAME, _("Download &game"));
	}

	if (engine_missing) {
		m_popup->Append(BATTLELIST_DATAVIEW_DL_ENGINE, _("Download &engine"));
	}

	m_popup->Append(BATTLELIST_DATAVIEW_NOTIFY_WHEN_BATTLE_ENDS, _("Notify me when the battle has ended"));

	PopupMenu(m_popup);
}
