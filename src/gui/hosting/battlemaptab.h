/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H

#include <wx/scrolwin.h>

#include "ibattlemaptab.h"

class Ui;
class IBattle;
class User;
class ChatPanel;
class wxCommandEvent;
class wxBoxSizer;
class wxComboBox;
class wxStaticText;
class wxSplitterWindow;
class wxStaticLine;
class wxButton;
class wxCheckBox;
class MapCtrl;
class wxListCtrl;
class wxChoice;
class wxRadioBox;
class wxMouseEvent;

/** \brief select map, draw startboxes (in sp define startpos)
 * \todo DOCMEMORE */
class BattleMapTab : public IBattleMapTab
{
public:
	BattleMapTab(wxWindow* parent, IBattle* battle);
	virtual ~BattleMapTab();

	virtual void UpdateUser(User& user) override;

	virtual void Update() override;
	virtual void Update(const wxString& Tag) override;
	virtual void ReloadMaplist() override;

	virtual void SetMap(int index) override;
	virtual void SetBattle(IBattle* battle) override;

private:
	IBattle* GetBattle()
	{
		return m_battle;
	}
	void OnMapSelect(wxCommandEvent& event);
	void OnMapBrowse(wxCommandEvent& event);
	void OnStartTypeSelect(wxCommandEvent& event);
	void OnUnitsyncReloaded(wxCommandEvent& /*data*/);

	IBattle* m_battle;
	//LSL::UnitsyncMap m_map;

	MapCtrl* m_minimap;
	wxChoice* m_map_combo;
	wxButton* m_browse_btn;
	wxRadioBox* m_start_radios;
	wxListCtrl* m_map_opts_list;
	wxStaticText* m_map_desc;

	enum {
		BMAP_MAP_SEL = wxID_HIGHEST,
		BMAP_MAP_BROWSE,
		BMAP_START_TYPE
	};

	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLEMAPTAB_H
