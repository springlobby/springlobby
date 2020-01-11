/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "addbotdialog.h"

#include <lslunitsync/unitsync.h>
#include <lslutils/conversion.h>
#include <lslutils/misc.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "gui/controls.h"
#include "gui/customdialogs.h"
#include "gui/uiutils.h"
#include "ibattle.h"
#include "mmoptionwindows.h"
#include "settings.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"

static const char *banned_AIs[] = {
	"AAI", "CircuitAI", "CppTestAI",
	"E323AI", "KAIK", "RAI", "Shard"
};

BEGIN_EVENT_TABLE(AddBotDialog, wxDialog)
EVT_BUTTON(ADDBOT_CANCEL, AddBotDialog::OnClose)
EVT_BUTTON(ADDBOT_ADD, AddBotDialog::OnAddBot)
EVT_CHOICE(ADDBOT_AI, AddBotDialog::OnSelectBot)
EVT_LIST_ITEM_ACTIVATED(ADDBOT_OPTIONLIST, AddBotDialog::OnOptionActivate)
END_EVENT_TABLE()


AddBotDialog::AddBotDialog(wxWindow* parent, IBattle& battle, bool singleplayer)
    : wxDialog(parent, wxID_ANY, _("Add bot"), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER | wxDEFAULT_DIALOG_STYLE)
    , WindowAttributesPickle(_T("ADDBOTDIALOG"), this, wxSize(-1, 255))
    , m_battle(battle)
    , m_sp(singleplayer)
{
	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	if (battle.GetNumBots() >= 2)
		SetTitle(_("Add bot: high nimber of bots can cause performance problems"));

	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* m_nick_sizer;
	m_nick_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_nick_lbl = new wxStaticText(this, wxID_ANY, _("Nickname:"), wxDefaultPosition, wxDefaultSize, 0);
	m_nick_sizer->Add(m_nick_lbl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	int bot = 1;
	wxString botname = wxString::Format(_T("Bot%d"), bot);
	while (m_battle.UserExists(STD_STRING(botname))) {
		bot++;
		botname = wxString::Format(_T("Bot%d"), bot);
	}

	m_nick = new wxTextCtrl(this, wxID_ANY, botname, wxDefaultPosition, wxDefaultSize, 0);
	m_nick_sizer->Add(m_nick, 2, wxALL, 5);

	m_main_sizer->Add(m_nick_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* m_ai_sizer;
	m_ai_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_ai_lbl = new wxStaticText(this, wxID_ANY, _("AI:"), wxDefaultPosition, wxDefaultSize, 0);
	m_ai_sizer->Add(m_ai_lbl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_ai = new wxChoice(this, ADDBOT_AI);
	m_ai->SetToolTip(_("Choose the AI library to use with this bot."));

	m_ai_sizer->Add(m_ai, 2, wxALL, 5);

	m_main_sizer->Add(m_ai_sizer, 0, wxEXPAND, 5);

	m_ai_infos_lst = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER);
	wxListItem col;
	col.SetText(_("property"));
	col.SetImage(-1);
	m_ai_infos_lst->InsertColumn(0, col);
	wxListItem col2;
	col2.SetText(_("value"));
	col2.SetImage(-1);
	m_ai_infos_lst->InsertColumn(1, col2);

	m_opts_list = new wxListCtrl(this, ADDBOT_OPTIONLIST, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER);
	wxListItem col3;
	col3.SetText(_("property"));
	col3.SetImage(-1);
	m_opts_list->InsertColumn(0, col3);
	wxListItem col4;
	col4.SetText(_("value"));
	col4.SetImage(-1);
	m_opts_list->InsertColumn(1, col4);

	m_info_sizer = new wxBoxSizer(wxVERTICAL);
	m_info_sizer->Add(m_ai_infos_lst, 1, wxALL | wxEXPAND);
	m_info_sizer->Add(m_opts_list, 1, wxALL | wxEXPAND);
	m_main_sizer->Add(m_info_sizer, 1, wxALL | wxEXPAND);


	m_buttons_sep = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	m_main_sizer->Add(m_buttons_sep, 0, wxALL | wxEXPAND);

	wxBoxSizer* m_buttons_sizer;
	m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_cancel_btn = new wxButton(this, ADDBOT_CANCEL, _("Cancel"), wxDefaultPosition, wxSize(-1, CONTROL_HEIGHT), 0);
	m_buttons_sizer->Add(m_cancel_btn, 0, wxALL);

	m_buttons_sizer->Add(0, 0, 1, wxEXPAND);

	m_add_btn = new wxButton(this, ADDBOT_ADD, _("Add Bot"), wxDefaultPosition, wxSize(-1, CONTROL_HEIGHT), 0);
	m_buttons_sizer->Add(m_add_btn, 0, wxALL);

	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND);

	ReloadAIList();

	SetSizer(m_main_sizer);
	Fit();
	m_add_btn->SetFocus();
}


AddBotDialog::~AddBotDialog()
{
}

wxString AddBotDialog::GetNick()
{
	wxString s = m_nick->GetValue();
	s.Replace(_T(" "), _T("_"));
	return s;
}


wxString AddBotDialog::Get(const std::string& section)
{
	const auto sel = GetSelectedAIType();
	const auto infos = LSL::usync().GetAIInfos(sel);
	const auto namepos = LSL::Util::IndexInSequence(infos, section);
	if (namepos == LSL::lslNotFound)
		return m_ais[sel];
	return TowxString(infos[namepos + 1]);
}

wxString AddBotDialog::GetAIShortName()
{
	return Get("shortName");
}

wxString AddBotDialog::GetAIVersion()
{
	return Get("version");
}

int AddBotDialog::GetSelectedAIType()
{
	return m_valid_ai_index_map.at (m_ai->GetSelection());
}

wxString AddBotDialog::RefineAIName(const wxString& name)
{
	wxString ret = name;
	if (m_ai->FindString(ret) == wxNOT_FOUND)
		return ret;
	wxString ret2;
	int i = 2;
	do {
		ret2 = ret + wxString::Format(_T(" (%d)"), i);
		i++;
	} while (m_ai->FindString(ret2) != wxNOT_FOUND);
	return ret2;
}

wxString AddBotDialog::GetAiRawName()
{
	//    if ( m_ais.GetCount() < m_ai->GetSelection() )
	//        return wxEmptyString;
	return m_ais [GetSelectedAIType()];
}

void AddBotDialog::ReloadAIList()
{
	LSL::StringVector ais;
	try {
		ais = LSL::usync().GetAIList(m_battle.GetHostGameNameAndVersion());
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception while loading AIs: %s"), e.what());
	}

	// m_ai is the wxChoice display list
	m_ai->Clear();
	// m_ais is a wxArrayString of all AIs, even blocked ones.
	m_ais.clear();
	m_valid_ai_index_map.clear();
	for (unsigned int i = 0; i < ais.size(); ++i) {
		const std::string& AINameVersion = ais.at(i);
		bool matched = false; // either this or goto...

		for (unsigned int j = 0; j < sizeof (banned_AIs) / sizeof(char *); ++j) {
			const char *ai_str = banned_AIs[j];
			if (std::string::npos != AINameVersion.find(ai_str)) {
				matched = true;
				break;
			}
		}

		wxString wxAINV (AINameVersion);
		m_ais.Add(wxAINV);
		if (!matched || (sett().GetUserLevel() >= Settings::UserLevel::Professional)) {
			m_ai->Append(RefineAIName(wxAINV));
			m_valid_ai_index_map.push_back(i);
		}
	}
	if (m_ais.GetCount() > 0) {
		m_ai->SetStringSelection(sett().GetLastAI());
		if (m_ai->GetStringSelection() == wxEmptyString)
			m_ai->SetSelection(0);
	} else {
		customMessageBox(SL_MAIN_ICON, _("No AI bots found in your Spring installation."), _("No bot-libs found"), wxOK);
	}
	m_add_btn->Enable(m_ai->GetStringSelection() != wxEmptyString);
	ShowAIInfo();
	ShowAIOptions();
}


void AddBotDialog::OnClose(wxCommandEvent& /*event*/)
{
	EndModal(wxID_CANCEL);
}


void AddBotDialog::OnAddBot(wxCommandEvent& /*event*/)
{
	if (m_nick->GetValue().empty()) {
		return;
	}
	sett().SetLastAI(m_ai->GetStringSelection());
	EndModal(wxID_OK);
}


void AddBotDialog::OnSelectBot(wxCommandEvent& /*unused*/)
{
	ShowAIInfo();
	ShowAIOptions();
}

void AddBotDialog::ShowAIInfo()
{
	m_add_btn->Enable(m_ai->GetStringSelection() != wxEmptyString);
	m_ai_infos_lst->DeleteAllItems();
	const wxArrayString info = lslTowxArrayString(LSL::usync().GetAIInfos(GetSelectedAIType()));
	int count = info.GetCount();
	for (int i = 0; i < count; i = i + 3) {
		long index = m_ai_infos_lst->InsertItem(i, info[i]);
		m_ai_infos_lst->SetItem(index, 0, info[i]);
		m_ai_infos_lst->SetItem(index, 1, info[i + 1]);
	}
	m_ai_infos_lst->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_ai_infos_lst->SetColumnWidth(1, wxLIST_AUTOSIZE);
	Layout();
	SetSize(wxDefaultSize);
}


void AddBotDialog::ShowAIOptions()
{
	m_opts_list->DeleteAllItems();
	m_opt_list_map.clear();
	m_battle.CustomBattleOptions().loadAIOptions(m_battle.GetHostGameNameAndVersion(), GetSelectedAIType(), STD_STRING(GetNick()));
	AddMMOptionsToList(0, m_battle.CustomBattleOptions().GetAIOptionIndex(STD_STRING(GetNick())));
	m_opts_list->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_opts_list->SetColumnWidth(1, wxLIST_AUTOSIZE);
	Layout();
	SetSize(wxDefaultSize);
}

long AddBotDialog::AddMMOptionsToList(long pos, int optFlag)
{
	for (const auto& it : m_battle.CustomBattleOptions().getOptions((LSL::Enum::GameOption)optFlag)) {
		m_opts_list->InsertItem(pos, TowxString(it.second.first));
		const wxString tag = wxString::Format(_T( "%d_%s"), optFlag, TowxString(it.first).c_str());
		m_opt_list_map[tag] = pos;
		UpdateOption(tag);
		pos++;
	}
	return pos;
}


void AddBotDialog::UpdateOption(const wxString& Tag)
{
	const long index = m_opt_list_map[Tag];
	const LSL::Enum::GameOption type = (LSL::Enum::GameOption)FromwxString(Tag.BeforeFirst('_'));
	const std::string key = STD_STRING(Tag.AfterFirst('_'));
	std::string value;

	const auto DataType = m_battle.CustomBattleOptions().GetSingleOptionType(key);
	value = m_battle.CustomBattleOptions().getSingleValue(key, (LSL::Enum::GameOption)type);
	if (m_battle.CustomBattleOptions().getDefaultValue(key, type) == value)
		m_opts_list->SetItemFont(index, wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT));
	else
		m_opts_list->SetItemFont(index, wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	if (DataType == LSL::Enum::opt_bool) {
		value = STD_STRING(bool2yn(LSL::Util::FromIntString(value))); // convert from 0/1 to literal Yes/No
	} else if (DataType == LSL::Enum::opt_list) {
		value = m_battle.CustomBattleOptions().GetNameListOptValue(key, type); // get the key full name not short key
	}
	m_opts_list->SetItem(index, 1, TowxString(value));
	m_opts_list->SetColumnWidth(1, wxLIST_AUTOSIZE);
}

void AddBotDialog::OnOptionActivate(wxListEvent& event)
{
	long index = event.GetIndex();
	wxString tag;
	for (OptionListMap::const_iterator itor = m_opt_list_map.begin(); itor != m_opt_list_map.end(); ++itor) {
		if (itor->second == index) {
			tag = itor->first;
			break;
		}
	}
	LSL::OptionsWrapper& optWrap = m_battle.CustomBattleOptions();
	const LSL::Enum::GameOption optFlag = (LSL::Enum::GameOption)FromwxString(tag.BeforeFirst('_'));
	const std::string key = STD_STRING(tag.AfterFirst('_'));
	LSL::Enum::OptionType type = optWrap.GetSingleOptionType(key);
	if (!optWrap.keyExists(key, optFlag, false, type))
		return;
	SingleOptionDialog dlg(m_battle, tag);
	dlg.ShowModal();
	UpdateOption(tag);
}
