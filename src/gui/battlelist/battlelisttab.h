/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H

#include <wx/scrolwin.h>

#include "utils/mixins.h"

class Battle;
class BattleDataViewCtrl;
class BattleListFilter;
class IBattle;
class MapCtrl;
class NickDataViewCtrl;
class Ui;
class User;
class wxBoxSizer;
class wxButton;
class wxCheckBox;
class wxComboBox;
class wxCommandEvent;
class wxDataViewEvent;
class wxFlexGridSizer;
class wxListEvent;
class wxStaticLine;
class wxStaticText;
class wxToggleButton;

/** \brief The panel containing a BattleListCtrl and a BattleListFilter
 * \todo DOCME */
class BattleListTab : public wxPanel, public SL::NonCopyable
{
	friend class BattleListFilter;

public:
	explicit BattleListTab(wxWindow* parent);
	~BattleListTab();

	void OnConnected();

	void AddBattle(IBattle& battle);
	void RemoveBattle(IBattle& battle);
	void UpdateBattle(IBattle& battle);

	void UserUpdate(User& user);

	void OnDisconnected();

	void UpdateList();

	void SelectBattle(IBattle* battle);

	void OnHost(wxCommandEvent& event);
	void OnFilter(wxCommandEvent& event);
	void OnInfoShow(wxCommandEvent& event);
	void OnFilterActiv(wxCommandEvent& event);
	void SetFilterActiv(bool activ);
	void OnJoin(wxCommandEvent& event);
	void OnListJoin(wxDataViewEvent& event);
	void ShowExtendedInfos(bool show);

	void DoJoin(IBattle& battle);

	void OnSelect(wxDataViewEvent& event);
	void OnUnitsyncReloaded(wxCommandEvent& data);

	void UpdateHighlights();

	void SortBattleList();

private:
	BattleListFilter* m_filter;
	BattleDataViewCtrl* m_battle_list;
	MapCtrl* m_minimap;
	wxStaticText* m_map_lbl;
	wxStaticText* m_map_text;
	wxStaticText* m_mod_lbl;
	wxStaticText* m_mod_text;
	wxStaticText* m_players_lbl;
	wxStaticText* m_players_text;
	wxStaticText* m_spec_lbl;
	wxStaticText* m_spec_text;
	wxStaticText* m_host_lbl;
	wxStaticText* m_host_text;
	wxStaticText* m_engine_lbl;
	wxStaticText* m_engine_text;
	wxStaticText* m_battle_num;
	NickDataViewCtrl* m_players;
	wxStaticLine* m_buttons_sep;
	wxButton* m_host_btn;
	wxButton* m_join_btn;
	wxBoxSizer* m_battlelist_sizer;
	wxFlexGridSizer* m_data_sizer;
	wxBoxSizer* m_main_sizer;
	wxBoxSizer* m_info_sizer;

	wxCheckBox* m_filter_activ;

#if wxUSE_TOGGLEBTN
	wxToggleButton* m_filter_show;
	wxToggleButton* m_info_show;
#else
	wxCheckBox* m_filter_show;
	wxCheckBox* m_info_show;
#endif

	IBattle* m_sel_battle;

	void SetNumDisplayed();

	enum {
		BATTLE_JOIN = wxID_HIGHEST,
		BATTLE_HOST,
		BATTLE_LIST,
		BATTLE_LIST_FILTER_BUTTON,
		BATTLE_LIST_INFO_BUTTON,
		BATTLE_LIST_FILTER_ACTIV
	};

	DECLARE_EVENT_TABLE()
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTTAB_H
