/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H
#define SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H

#include <wx/scrolwin.h>
#include "singleplayerbattle.h"
class Ui;
class MapCtrl;
class SinglePlayerBattle;

class wxWindow;
class wxBoxSizer;
class wxButton;
class wxStaticLine;
class wxChoice;
class wxStaticText;
class wxListCtrl;
class wxCommandEvent;
class wxCheckBox;
class wxMouseEvent;
class ColorButton;

class SinglePlayerTab : public wxPanel
{
public:
	SinglePlayerTab(wxWindow* parent, MainSinglePlayerTab& msptab);
	~SinglePlayerTab();

	void UpdateMinimap();
	void ReloadMaplist();
	void ReloadModlist();
	void ReloadEngineList();
	void UpdatePresetList();

	IBattle& GetBattle()
	{
		return m_battle;
	}

	bool ValidSetup() const;

	void OnMapSelect(wxCommandEvent& event);
	void OnModSelect(wxCommandEvent& event);
	void OnEngineSelect(wxCommandEvent& event);
	void OnMapBrowse(wxCommandEvent& event);
	void OnAddBot(wxCommandEvent& event);
	void OnStart(wxCommandEvent& event);
	void OnRandomCheck(wxCommandEvent& event);
	void OnSpectatorCheck(wxCommandEvent& event);
	void OnColorButton(wxCommandEvent& event);
	void OnReset(wxCommandEvent& event);

	void OnUnitsyncReloaded(wxCommandEvent& /*data*/);
	void ResetUsername();

	void SetEngine(unsigned int index);
	void SetMod(unsigned int index);
	void SetMap(unsigned int index);

private:
	wxBoxSizer* m_main_sizer;
	SinglePlayerBattle m_battle;

	MapCtrl* m_minimap;
	wxWindow* m_nominimap;
	wxChoice* m_map_pick;
	wxChoice* m_engine_choice;
	wxChoice* m_game_choice;
	wxStaticText* m_map_lbl;
	wxStaticText* m_mod_lbl;
	wxButton* m_select_btn;
	wxButton* m_addbot_btn;
	wxStaticLine* m_buttons_sep;
	//    wxButton* m_reset_btn;
	wxCheckBox* m_random_check;
	wxCheckBox* m_spectator_check;
	wxButton* m_start_btn;
	ColorButton* m_color_btn;

	wxListCtrl* m_map_opts_list;
	wxStaticText* m_map_desc;

	enum {
		SP_MAP_PICK = wxID_HIGHEST,
		SP_MOD_PICK,
		SP_ENGINE_PICK,
		SP_BROWSE_MAP,
		SP_ADD_BOT,
		SP_RESET,
		SP_START,
		SP_RANDOM,
		SP_SPECTATE,
		SP_COLOUR
	};
	DECLARE_EVENT_TABLE()
};
#endif // SPRINGLOBBY_HEADERGUARD_SINGLEPLAYERTAB_H
