/*
 * PlaybackDataView.cpp
 *
 *  Created on: 19 июня 2015 г.
 *      Author: Руслан
 */

#include "playbackdataview.h"

#include <wx/wx.h>
#include <wx/dataview.h>


#include "../../iplaybacklist.h"
#include "../../replaylist.h"
#include "../../servermanager.h"
#include "../../storedgame.h"
#include "../basedataviewmodel.h"
#include "../customdialogs.h"
#include "playbackdatamodel.h"

BEGIN_EVENT_TABLE(PlaybackDataView, BaseDataViewCtrl)
	EVT_DATAVIEW_ITEM_CONTEXT_MENU(REPLAY_DATAVIEW_ID, PlaybackDataView::OnContextMenu)
	EVT_MENU(REPLAY_DATAVIEW_DLMAP_ID, PlaybackDataView::OnDLMap)
	EVT_MENU(REPLAY_DATAVIEW_DLMOD_ID, PlaybackDataView::OnDLMod)
END_EVENT_TABLE()

PlaybackDataView::PlaybackDataView(wxString& dataViewName, wxWindow* parent) : BaseDataViewCtrl(dataViewName, parent, REPLAY_DATAVIEW_ID) {
	// TODO Auto-generated constructor stub

	const int DEFAULT_SIZE = -1;
	AppendIconTextColumn(_("Date"), DATE, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Game"), GAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Map"), MAP, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Players"), PLAYERS, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Duration"), DURATION, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Version"), VERSION, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("Filesize"), FILESIZE, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	AppendTextColumn(_("File"), FILENAME, wxDATAVIEW_CELL_INERT, DEFAULT_SIZE, wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	auto* m_PlaybackDataModel = new PlaybackDataModel();
	AssociateModel(m_PlaybackDataModel);

	m_ContextMenu = new wxMenu(wxEmptyString);
	m_ContextMenu->Append(REPLAY_DATAVIEW_DLMAP_ID, _("Download &map"));
	m_ContextMenu->Append(REPLAY_DATAVIEW_DLMOD_ID, _("Download &game"));

	LoadColumnProperties();
}

PlaybackDataView::~PlaybackDataView() {
	// TODO Auto-generated destructor stub
}

void PlaybackDataView::AddPlayback(const StoredGame& replay) {
	m_DataModel->AddItem(replay);

	Refresh();
}

void PlaybackDataView::RemovePlayback(const StoredGame& replay) {
	m_DataModel->RemoveItem(replay);

	Refresh();
}

void PlaybackDataView::OnContextMenu(wxDataViewEvent& event) {
	const StoredGame* storedGame = GetSelectedItem();

	if (storedGame == nullptr) {
		return;
	}
	PopupMenu(m_ContextMenu);
}

void PlaybackDataView::OnDLMap(wxCommandEvent& event) {
	const StoredGame* storedGame = GetSelectedItem();

	if (storedGame == nullptr) {
		return;
	}

	const OfflineBattle& battle = storedGame->battle;
	ServerManager::Instance()->DownloadContent("map", battle.GetHostMapName(), battle.GetHostMapHash());
}

void PlaybackDataView::OnDLMod(wxCommandEvent& event) {
	const StoredGame* storedGame = GetSelectedItem();

	if (storedGame == nullptr) {
		return;
	}

	const OfflineBattle& battle = storedGame->battle;
	ServerManager::Instance()->DownloadContent("game", battle.GetHostModName(), battle.GetHostModHash());
}

void PlaybackDataView::DeletePlayback() {
	const StoredGame* storedGame = GetSelectedItem();

	if (storedGame == nullptr) {
		return;
	}

	try {
		const int m_sel_replay_id = storedGame->id;
		if (replaylist().DeletePlayback(m_sel_replay_id) == false) {
			wxString fn = wxString(storedGame->Filename);
			customMessageBoxNoModal(SL_MAIN_ICON, _("Could not delete Replay: ") + fn, _("Error"));
		} else {
			RemovePlayback(*storedGame);
		}
	} catch (std::runtime_error&) {
		//Do nothing
	}
}

void PlaybackDataView::SetTipWindowText(const long item_hit,
		const wxPoint& position) {
	//TODO: Implement
}

void PlaybackDataView::HighlightItem(long longInt) {
	//TODO: Implement
}
