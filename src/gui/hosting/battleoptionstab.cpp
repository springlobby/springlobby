/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: BattleOptionsTab
//

#include "battleoptionstab.h"

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/window.h>
#include <wx/listbox.h>
#include <wx/arrstr.h>
#include <wx/choice.h>
#include <wx/tokenzr.h>
#include <wx/checklst.h>
#include <wx/numdlg.h>

#include "battleoptionstab.h"
#include "ibattle.h"
#include "gui/controls.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"
#include "iserver.h"
#include <lslunitsync/optionswrapper.h>
#include <lslunitsync/unitsync.h>
#include "aui/auimanager.h"


BEGIN_EVENT_TABLE(BattleOptionsTab, wxPanel)

EVT_BUTTON(BOPTS_RESTRICT, BattleOptionsTab::OnRestrict)
EVT_BUTTON(BOPTS_ALLOW, BattleOptionsTab::OnAllow)
EVT_BUTTON(BOPTS_CLEARRES, BattleOptionsTab::OnClearRestrictions)

END_EVENT_TABLE()


BattleOptionsTab::BattleOptionsTab(wxWindow* parent, IBattle* battle)
    : wxScrolledWindow(parent, -1)
    , m_battle(battle)
{
	GetAui().manager->AddPane(this, wxLEFT, _T( "battleoptionstab" ));

	wxBoxSizer* m_main_sizer;
	m_main_sizer = new wxBoxSizer(wxHORIZONTAL);

	//	wxBoxSizer* m_main_options_sizer;
	//	m_main_options_sizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* m_restr_box;
	m_restr_box = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Unit restrictions")), wxVERTICAL);

	wxBoxSizer* m_top_restr_sizer;
	m_top_restr_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* m_allowed_sizer;
	m_allowed_sizer = new wxBoxSizer(wxVERTICAL);

	m_aloowed_lbl = new wxStaticText(this, wxID_ANY, _("Allowed units"), wxDefaultPosition, wxDefaultSize, 0);
	m_allowed_sizer->Add(m_aloowed_lbl, 0, wxALL, 5);

	m_allowed_list = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE | wxLB_NEEDED_SB | wxLB_SORT);
	m_allowed_list->SetToolTip(_("Units in this list will be available in the game."));

	m_allowed_sizer->Add(m_allowed_list, 1, wxALL | wxEXPAND, 5);

	m_top_restr_sizer->Add(m_allowed_sizer, 1, wxEXPAND, 5);

	wxBoxSizer* m_mid_btn_sizer;
	m_mid_btn_sizer = new wxBoxSizer(wxVERTICAL);

	m_mid_btn_sizer->Add(0, 50, 0, wxEXPAND, 0);

	m_restrict_btn = new wxButton(this, BOPTS_RESTRICT, _T( ">" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_restrict_btn->SetToolTip(_("Disable selected units."));
	m_mid_btn_sizer->Add(m_restrict_btn, 0, wxALL, 5);

	m_allow_btn = new wxButton(this, BOPTS_ALLOW, _T( "<" ), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_allow_btn->SetToolTip(_("Re-enable selected units."));
	m_mid_btn_sizer->Add(m_allow_btn, 0, wxALL, 5);

	m_clear_btn = new wxButton(this, BOPTS_CLEARRES, _("<<"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_clear_btn->SetToolTip(_("Enable all units."));

	m_mid_btn_sizer->Add(m_clear_btn, 0, wxALL, 5);

	m_top_restr_sizer->Add(m_mid_btn_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* m_restricted_sizer;
	m_restricted_sizer = new wxBoxSizer(wxVERTICAL);

	m_restricted_lbl = new wxStaticText(this, wxID_ANY, _("Restricted units"), wxDefaultPosition, wxDefaultSize, 0);
	m_restricted_sizer->Add(m_restricted_lbl, 0, wxALL, 5);

	m_restrict_list = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE | wxLB_NEEDED_SB | wxLB_SORT);
	m_restrict_list->SetToolTip(_("Units in this list will not be available in the game."));

	m_restricted_sizer->Add(m_restrict_list, 1, wxALL | wxEXPAND, 5);

	m_top_restr_sizer->Add(m_restricted_sizer, 1, wxEXPAND, 5);

	m_restr_box->Add(m_top_restr_sizer, 1, wxEXPAND, 5);

	m_main_sizer->Add(m_restr_box, 1, wxALL | wxEXPAND, 5);

	SetScrollRate(SCROLL_RATE, SCROLL_RATE);
	this->SetSizer(m_main_sizer);
	this->Layout();
}


BattleOptionsTab::~BattleOptionsTab()
{
	if (GetAui().manager)
		GetAui().manager->DetachPane(this);
}


void BattleOptionsTab::UpdateBattle(const wxString& Tag)
{
	if (!m_battle)
		return;
	long type;
	Tag.BeforeFirst('_').ToLong(&type);
	wxString key = Tag.AfterFirst('_');
	if (type == LSL::Enum::PrivateOptions) {
		if (key == _T( "restrictions" ))
			ReloadRestrictions();
	}
}

void BattleOptionsTab::ReloadRestrictions()
{
	if (!m_battle)
		return;
	m_allowed_list->Clear();
	m_restrict_list->Clear();
	if (m_battle->GetHostModName().empty())
		return;

	try {
		wxArrayString items = lslTowxArrayString(LSL::usync().GetUnitsList(m_battle->GetHostModName()));
		m_allowed_list->Append(items);
	} catch (...) {
	}
	std::map<std::string, int> units = m_battle->RestrictedUnits();

	for (std::map<std::string, int>::const_iterator itor = units.begin(); itor != units.end(); ++itor)
		Restrict(TowxString(itor->first), itor->second);
}


int BattleOptionsTab::GetAllowedUnitIndex(const wxString& name)
{
	if (!m_battle)
		return -1;
	for (unsigned int i = 0; i < m_allowed_list->GetCount(); i++) {
		wxString tmp = m_allowed_list->GetString(i);
		tmp = tmp.AfterLast('(');
		tmp = tmp.BeforeLast(')');
		if (name == tmp)
			return i;
	}
	return -1;
}


int BattleOptionsTab::GetRestrictedUnitIndex(const wxString& name)
{
	if (!m_battle)
		return -1;
	for (unsigned int i = 0; i < m_restrict_list->GetCount(); i++) {
		wxString tmp = m_restrict_list->GetString(i);
		tmp = tmp.AfterLast('(');
		tmp = tmp.BeforeLast(')');
		if (name == tmp)
			return i;
	}
	return -1;
}


bool BattleOptionsTab::IsRestricted(const wxString& name)
{
	return (GetRestrictedUnitIndex(name) >= 0);
}


void BattleOptionsTab::Restrict(const wxString& name, int count)
{
	if (!m_battle)
		return;
	int i = GetAllowedUnitIndex(name);
	Restrict(i, count);
}


void BattleOptionsTab::Allow(const wxString& name)
{
	if (!m_battle)
		return;
	int i = GetRestrictedUnitIndex(name);
	Allow(i);
}


void BattleOptionsTab::Restrict(int index, int count)
{
	if (!m_battle)
		return;
	if (index >= 0) {
		wxString unit = m_allowed_list->GetString(index);
		m_restrict_list->Append(unit << _T( " [" ) << count << _T( "]" ));
		m_allowed_list->Delete(index);
	}
}


void BattleOptionsTab::Allow(int index)
{
	if (!m_battle)
		return;
	if (index >= 0) {
		wxString unit = m_restrict_list->GetString(index);
		m_allowed_list->Append(unit);
		m_restrict_list->Delete(index);
	}
}


//////////////////////////////////////////////////////////////////////////
//  EVENTS
//////////////////////////////////////////////////////////////////////////


void BattleOptionsTab::OnRestrict(wxCommandEvent& /*unused*/)
{
	if (!m_battle)
		return;
	wxArrayInt sel;
	wxArrayString names;

	m_allowed_list->GetSelections(sel);
	for (unsigned int i = 0; i < sel.Count(); i++) {
		wxString name = m_allowed_list->GetString(sel.Item(i));
		name = name.AfterLast('(');
		name = name.BeforeLast(')');
		names.Add(name);
	}
	for (unsigned int i = 0; i < names.Count(); i++) {
		wxString unit = names.Item(i);
		int count = wxGetNumberFromUser(_("How many units of this type do you wish to allow?"), wxEmptyString, _("Unit restriction"), 0, 0, 500000);
		if (count >= 0)
			m_battle->RestrictUnit(STD_STRING(unit), count);
	}
	if (names.Count() > 0)
		m_battle->SendHostInfo(IBattle::HI_Restrictions);
}


void BattleOptionsTab::OnAllow(wxCommandEvent& /*unused*/)
{
	if (!m_battle)
		return;
	wxArrayInt sel;
	wxArrayString names;

	m_restrict_list->GetSelections(sel);
	for (unsigned int i = 0; i < sel.Count(); i++) {
		wxString name = m_restrict_list->GetString(sel.Item(i));
		name = name.AfterLast('(');
		name = name.BeforeLast(')');
		names.Add(name);
	}
	for (unsigned int i = 0; i < names.Count(); i++) {
		const wxString unit = names.Item(i);
		m_battle->UnrestrictUnit(STD_STRING(unit));
	}
	if (names.Count() > 0)
		m_battle->SendHostInfo(IBattle::HI_Restrictions);
}


void BattleOptionsTab::OnClearRestrictions(wxCommandEvent& /*unused*/)
{
	if (!m_battle)
		return;
	m_battle->UnrestrictAllUnits();
	ReloadRestrictions();
}

void BattleOptionsTab::SetBattle(IBattle* battle)
{
	m_battle = battle;
	bool isBattleEnabled = (battle != nullptr);

	m_restrict_btn->Enable(isBattleEnabled);
	m_allow_btn->Enable(isBattleEnabled);
	m_clear_btn->Enable(isBattleEnabled);
	m_allowed_list->Enable(isBattleEnabled);
	m_restrict_list->Enable(isBattleEnabled);

	if (isBattleEnabled) {
		ReloadRestrictions();

		if (!m_battle->IsFounderMe()) {
			m_restrict_btn->Disable();
			m_allow_btn->Disable();
			m_clear_btn->Disable();
		}
	}
}

IBattle* BattleOptionsTab::GetBattle()
{
	return m_battle;
}
