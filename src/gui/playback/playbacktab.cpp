/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "playbacktab.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>
#include <wx/tglbtn.h>

#include "exception.h"
#include "gui/chatpanel.h"
#include "gui/customdialogs.h"
#include "gui/hosting/battleroomdataviewctrl.h"
#include "gui/mapctrl.h"
#include "gui/ui.h"
#include "gui/uiutils.h"
#include "iconimagelist.h"
#include "log.h"
#include "playbackdataview.h"
#include "playbackfilter.h"
#include "playbackthread.h"
#include "replaylist.h"
#include "storedgame.h"
#include "utils/conversion.h"
#include "utils/globalevents.h"
#include "utils/slconfig.h"

BEGIN_EVENT_TABLE(PlaybackTab, wxPanel)
EVT_BUTTON(PLAYBACK_WATCH, PlaybackTab::OnWatch)
EVT_BUTTON(PLAYBACK_RELOAD, PlaybackTab::OnReload)
EVT_BUTTON(PLAYBACK_DELETE, PlaybackTab::OnDelete)
EVT_DATAVIEW_SELECTION_CHANGED(PlaybackDataView::REPLAY_DATAVIEW_ID, PlaybackTab::OnSelect)
EVT_CHECKBOX(PLAYBACK_LIST_FILTER_ACTIV, PlaybackTab::OnFilterActiv)
EVT_COMMAND(wxID_ANY, PlaybackLoader::PlaybacksLoadedEvt, PlaybackTab::AddAllPlaybacks)
EVT_KEY_DOWN(PlaybackTab::OnChar)
EVT_TOGGLEORCHECK(PLAYBACK_LIST_FILTER_BUTTON, PlaybackTab::OnFilter)
END_EVENT_TABLE()

PlaybackTab::PlaybackTab(wxWindow* parent, bool replay)
    : wxPanel(parent, -1)
    , m_replay_loader(0)
    , m_isreplay(replay)
{
	wxLogMessage(_T( "PlaybackTab::PlaybackTab()" ));


	m_minimap = new MapCtrl(this, 100, 0, true, false, false);

	m_players_lbl  = new wxStaticText(this, wxID_ANY, _("Players:"));
	m_players_text = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_map_lbl      = new wxStaticText(this, wxID_ANY, _("Map:"));
	m_map_text     = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_game_lbl     = new wxStaticText(this, wxID_ANY, _("Game:"));
	m_game_text    = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_engine_lbl   = new wxStaticText(this, wxID_ANY, _("Engine:"));
	m_engine_text  = new wxStaticText(this, wxID_ANY, wxEmptyString);

	wxFlexGridSizer* m_data_sizer = new wxFlexGridSizer(4, 2, 0, 0);
	m_data_sizer->Add(m_map_lbl, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_map_text, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_game_lbl, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_game_text, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_players_lbl, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_players_text, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_engine_lbl, 1, wxALL | wxEXPAND, 5);
	m_data_sizer->Add(m_engine_text, 1, wxALL | wxEXPAND, 5);

	m_players = new BattleroomDataViewCtrl("playback_battleroom_view", this, nullptr /*battle*/, true /*readonly*/, false /*show ingname status*/);

	wxBoxSizer* m_info_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_info_sizer->Add(m_minimap, 0, wxALL, 5);
	m_info_sizer->Add(m_data_sizer, 1, wxEXPAND | wxALL, 0);
	m_info_sizer->Add(m_players, 2, wxALL | wxEXPAND, 0);


	m_filter_show = new wxToggleOrCheck(this, PLAYBACK_LIST_FILTER_BUTTON, _(" Filter "), wxDefaultPosition, wxSize(-1, 28), 0);
	m_filter_activ = new wxCheckBox(this, PLAYBACK_LIST_FILTER_ACTIV, _("Activated"), wxDefaultPosition, wxDefaultSize, 0);
	m_watch_btn = new wxButton(this, PLAYBACK_WATCH, replay ? _("Watch") : _("Load"), wxDefaultPosition, wxSize(-1, 28), 0);
	m_delete_btn = new wxButton(this, PLAYBACK_DELETE, _("Delete"), wxDefaultPosition, wxSize(-1, 28), 0);
	m_reload_btn = new wxButton(this, PLAYBACK_RELOAD, _("Reload list"), wxDefaultPosition, wxSize(-1, 28), 0);

	wxBoxSizer* m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_buttons_sizer->Add(m_filter_show, 0, 0, 5);
	m_buttons_sizer->Add(m_filter_activ, 1, wxALL | wxEXPAND, 5);
	m_buttons_sizer->Add(0, 0, 1, wxEXPAND, 0);
	m_buttons_sizer->Add(m_watch_btn, 0, wxBOTTOM | wxLEFT | wxRIGHT, 5);
	m_buttons_sizer->Add(m_delete_btn, 0, wxBOTTOM | wxRIGHT, 5);
	m_buttons_sizer->Add(m_reload_btn, 0, wxBOTTOM | wxRIGHT, 5);


	m_replay_dataview = new PlaybackDataView("replays_dataview", this);
	m_filter = new PlaybackListFilter(this, wxID_ANY, this, wxDefaultPosition, wxSize(-1, -1), wxEXPAND);
	m_buttons_sep = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);

	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_main_sizer->Add(m_replay_dataview, 1, wxEXPAND);
	m_main_sizer->Add(m_info_sizer, 0, wxEXPAND, 5);
	m_main_sizer->Add(m_filter, 0, wxEXPAND, 5);
	m_main_sizer->Add(m_buttons_sep, 0, wxALL | wxEXPAND, 5);
	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND, 5);

	m_filter->Hide();

	SetSizer(m_main_sizer);

	//none selected --> shouldn't watch/delete that
	Deselect();

	Layout();
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnUnitsyncReloaded, PlaybackTab::OnUnitsyncReloaded);
	SUBSCRIBE_GLOBAL_EVENT(GlobalEventManager::OnSpringTerminated, PlaybackTab::OnSpringTerminated);
}

PlaybackTab::~PlaybackTab()
{
	GlobalEventManager::Instance()->UnSubscribeAll(this);

	m_minimap->SetBattle(NULL);
	if (m_filter != 0)
		m_filter->SaveFilterValues();
	delete m_replay_loader;

	wxLogDebug("");
}

void PlaybackTab::AddAllPlaybacks(wxCommandEvent& /*unused*/)
{
	assert(wxThread::IsMain());
	const auto& replays = replaylist().GetPlaybacksMap();

	for (auto i = replays.begin(); i != replays.end(); ++i) {
		AddPlayback(i->second, false);
	}
	m_replay_dataview->Resort();
}

void PlaybackTab::AddPlayback(const StoredGame& replay, bool resortIsNeeded)
{
	if (m_filter->GetActiv() && !m_filter->FilterPlayback(replay)) {
		return;
	}

	m_replay_dataview->AddPlayback(replay, resortIsNeeded);
}

void PlaybackTab::RemovePlayback(const StoredGame& replay)
{
	m_replay_dataview->RemovePlayback(replay);
}

void PlaybackTab::UpdatePlayback(const StoredGame& replay)
{
	if (m_filter->GetActiv() && !m_filter->FilterPlayback(replay)) {
		RemovePlayback(replay);
		return;
	}

	bool contains = m_replay_dataview->ContainsItem(replay);

	if (contains) {
		m_replay_dataview->RefreshItem(replay);
	} else {
		AddPlayback(replay);
	}
}

void PlaybackTab::RemoveAllPlaybacks()
{
	m_replay_dataview->Clear();
	//shouldn't list be cleared too here? (koshi)
}


void PlaybackTab::UpdateList()
{
	const auto& replays = replaylist().GetPlaybacksMap();

	for (auto i = replays.begin(); i != replays.end(); ++i) {
		UpdatePlayback(i->second);
	}
	m_replay_dataview->Refresh();
}


void PlaybackTab::SetFilterActiv(bool activ)
{
	m_filter->SetActiv(activ);
	m_filter_activ->SetValue(activ);
}

void PlaybackTab::OnFilter(wxCommandEvent& /*unused*/)
{
	if (m_filter_show->GetValue()) {
		m_filter->Show();
		this->Layout();
	} else {
		m_filter->Hide();
		this->Layout();
	}
}

void PlaybackTab::OnWatch(wxCommandEvent& /*unused*/)
{
	const StoredGame* storedGame = m_replay_dataview->GetSelectedItem();

	if (storedGame == nullptr) {
		Deselected();
		return;
	}

	const int m_sel_replay_id = storedGame->id;

	wxString type = m_isreplay ? _("replay") : _("savegame");
	wxLogMessage(_T( "Watching %s %d " ), type.c_str(), m_sel_replay_id);
	StoredGame& rep = replaylist().GetPlaybackById(m_sel_replay_id);
	if (ui().NeedsDownload(&rep.battle)) {
		return;
	}

	rep.battle.GetMe().SetNick(STD_STRING(cfg().ReadString("/Spring/DefaultName")));
	rep.battle.StartSpring();
}

void PlaybackTab::AskForceWatch(StoredGame& rep) const
{
	if (customMessageBox(SL_MAIN_ICON, _("I don't think you will be able to watch this replay.\nTry anyways? (MIGHT CRASH!)"), _("Invalid replay"), wxYES_NO | wxICON_QUESTION | wxCANCEL) == wxYES) {
		rep.battle.StartSpring();
	}
}

void PlaybackTab::OnDelete(wxCommandEvent& /*unused*/)
{
	m_replay_dataview->DeletePlayback();
	Deselect();
}

void PlaybackTab::OnFilterActiv(wxCommandEvent& /*unused*/)
{
	m_filter->SetActiv(m_filter_activ->GetValue());
}

void PlaybackTab::OnSelect(wxDataViewEvent& event)
{
	const StoredGame* storedGame = m_replay_dataview->GetSelectedItem();

	if (storedGame == nullptr) {
		Deselect();
	} else {
		try {
			m_watch_btn->Enable(true);
			m_delete_btn->Enable(true);

			//this might seem a bit backwards, but it's currently the only way that doesn't involve casting away constness
			int m_sel_replay_id = storedGame->id;
			StoredGame& rep = replaylist().GetPlaybackById(m_sel_replay_id);

			m_players_text->SetLabel(wxEmptyString);
			m_map_text->SetLabel(TowxString(rep.battle.GetHostMapName()));
			m_game_text->SetLabel(TowxString(rep.battle.GetHostGameName()));
			m_engine_text->SetLabel(rep.battle.GetEngineName() + ' ' + rep.battle.GetEngineVersion());
			m_minimap->SetBattle(&(rep.battle));
			m_minimap->UpdateMinimap();

			m_players->Clear();
			m_players->SetBattle((IBattle*)&rep.battle);
			for (size_t i = 0; i < rep.battle.GetNumUsers(); ++i) {
				try {
					User& usr = rep.battle.GetUser(i);
					m_players->AddUser(usr);
				} catch (...) {
				}
			}
		} catch (...) {
			Deselect();
		}
		event.Skip();
	}
}

void PlaybackTab::Deselect()
{
	m_replay_dataview->UnselectAll();
	Deselected();
}

void PlaybackTab::Deselected()
{
	m_watch_btn->Enable(false);
	m_delete_btn->Enable(false);
	m_players_text->SetLabel(wxEmptyString);
	m_map_text->SetLabel(wxEmptyString);
	m_game_text->SetLabel(wxEmptyString);
	m_engine_text->SetLabel(wxEmptyString);
	m_minimap->SetBattle(NULL);
	m_minimap->UpdateMinimap();
	m_minimap->Refresh();
	m_players->Clear();
	m_players->SetBattle(NULL);
}

void PlaybackTab::ReloadList()
{
	if (!LSL::usync().IsLoaded()) {
		wxLogWarning(_("Unitsync library required"));
		return;
	}
	Deselect();
	m_replay_dataview->Clear();
	if (m_replay_loader == nullptr) {
		m_replay_loader = new PlaybackLoader(this, true);
	}
	m_replay_loader->Run();
}

void PlaybackTab::OnReload(wxCommandEvent& /*unused*/)
{
	ReloadList();
}

void PlaybackTab::OnSpringTerminated(wxCommandEvent& /*data*/)
{
	ReloadList();
}

void PlaybackTab::OnUnitsyncReloaded(wxCommandEvent& /*data*/)
{
	ReloadList();
}

void PlaybackTab::OnChar(wxKeyEvent& event)
{
	const int keyCode = event.GetKeyCode();
	if (keyCode == WXK_DELETE) {
		m_replay_dataview->DeletePlayback();
		Deselect();
	} else {
		event.Skip();
	}
}
