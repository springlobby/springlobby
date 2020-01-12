/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "battlelistfilter.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/event.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>

#include "aui/auimanager.h"
#include "battlelistfiltervalues.h"
#include "gui/controls/bitmapcombobox.h"
#include "gui/uiutils.h"
#include "ibattle.h"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"
#include "images/rank7.xpm"
#include "useractions.h"
#include "utils/conversion.h"
#include "utils/slconfig.h"
#include "utils/tasutil.h"

BEGIN_EVENT_TABLE(BattleListFilter, wxPanel)

EVT_BUTTON(BATTLE_FILTER_RANK_BUTTON, BattleListFilter::OnRankButton)
EVT_BUTTON(BATTLE_FILTER_PLAYER_BUTTON, BattleListFilter::OnPlayerButton)
EVT_BUTTON(BATTLE_FILTER_MAXPLAYER_BUTTON, BattleListFilter::OnMaxPlayerButton)
EVT_BUTTON(BATTLE_FILTER_SPECTATOR_BUTTON, BattleListFilter::OnSpectatorButton)
EVT_CHOICE(BATTLE_FILTER_SPECTATOR_CHOICE, BattleListFilter::OnSpectatorChange)
EVT_CHOICE(BATTLE_FILTER_MAXPLAYER_CHOICE, BattleListFilter::OnMaxPlayerChange)
EVT_CHOICE(BATTLE_FILTER_PLAYER_CHOICE, BattleListFilter::OnPlayerChange)
EVT_COMBOBOX(BATTLE_FILTER_RANK_CHOICE, BattleListFilter::OnRankChange)
EVT_CHECKBOX(BATTLE_FILTER_LOCKED, BattleListFilter::OnChange)
EVT_CHECKBOX(BATTLE_FILTER_OPEN, BattleListFilter::OnChange)
EVT_CHECKBOX(BATTLE_FILTER_PASSWORDED, BattleListFilter::OnChange)
EVT_CHECKBOX(BATTLE_FILTER_FULL, BattleListFilter::OnChange)
EVT_CHECKBOX(BATTLE_FILTER_STARTED, BattleListFilter::OnChange)
EVT_TEXT(BATTLE_FILTER_HOST_EDIT, BattleListFilter::OnChangeHost)
EVT_TEXT(BATTLE_FILTER_DESCRIPTION_EDIT, BattleListFilter::OnChangeDescription)
EVT_TEXT(BATTLE_FILTER_MAP_EDIT, BattleListFilter::OnChangeMap)
EVT_TEXT(BATTLE_FILTER_GAME_EDIT, BattleListFilter::OnChangeGame)
EVT_CHECKBOX(BATTLE_FILTER_MAP_SHOW, BattleListFilter::OnChange)
EVT_CHECKBOX(BATTLE_FILTER_GAME_SHOW, BattleListFilter::OnChange)
EVT_CHECKBOX(BATTLE_FILTER_HIGHLIGHTED, BattleListFilter::OnChange)

END_EVENT_TABLE()


BattleListFilter::BattleListFilter(wxWindow* parent, wxWindowID id, BattleListTab* parentBattleListTab,
				   const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
    , m_parent_battlelisttab(parentBattleListTab)
    , m_filter_host_edit(0)
    , m_filter_host_expression(0)
    , m_filter_description_edit(0)
    , m_filter_description_expression(0)
    , m_filter_map_edit(0)
    , m_filter_map_expression(0)
    , m_filter_game_edit(0)
    , m_filter_game_expression(0)
    , m_filter_highlighted(0)

{
	GetAui().manager->AddPane(this, wxLEFT, _T( "battlelistfilter" ));

	BattleListFilterValues f_values = GetBattleFilterValues(GetLastBattleFilterProfileName());

	wxArrayString choiceArray;
	choiceArray.Add(_("All"));
	for (wxLongLong i = 0; i <= 32; i++)
		choiceArray.Add(i.ToString());

	m_filter_status_text = new wxStaticText(this, wxID_ANY, _("Status:"));
	m_filter_status_text->Wrap(-1);
	m_filter_status_locked = new wxCheckBox(this, BATTLE_FILTER_LOCKED, _("Locked"));
	m_filter_status_locked->SetValue(f_values.status_locked);
	m_filter_status_pass = new wxCheckBox(this, BATTLE_FILTER_PASSWORDED, _("Passworded"));
	m_filter_status_pass->SetValue(f_values.status_passworded);
	m_filter_highlighted = new wxCheckBox(this, BATTLE_FILTER_HIGHLIGHTED, _("Highlighted only"));
	m_filter_highlighted->SetValue(f_values.highlighted_only);

	wxBoxSizer* m_filter_status_sizer1 = new wxBoxSizer(wxHORIZONTAL);
	m_filter_status_sizer1->Add(m_filter_status_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_status_sizer1->Add(m_filter_status_locked, 0, wxALL | wxEXPAND, 5);
	m_filter_status_sizer1->Add(m_filter_status_pass, 0, wxALL | wxEXPAND, 5);
	m_filter_status_sizer1->Add(m_filter_highlighted, 0, wxALL | wxEXPAND, 5);


	m_filter_rank_text = new wxStaticText(this, wxID_ANY, _("Rank Limit:"));
	m_filter_rank_text->Wrap(-1);
	m_filter_rank_button = new wxButton(this, BATTLE_FILTER_RANK_BUTTON, f_values.rank_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_rank_choice = new GUI::Controls::BitmapComboBox(this, BATTLE_FILTER_RANK_CHOICE, _("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
	m_filter_rank_choice->Append(_("All"));
	m_filter_rank_choice->Append(_T("1"), wxBitmap(rank0_xpm));
	m_filter_rank_choice->Append(_T("2"), wxBitmap(rank1_xpm));
	m_filter_rank_choice->Append(_T("3"), wxBitmap(rank2_xpm));
	m_filter_rank_choice->Append(_T("4"), wxBitmap(rank3_xpm));
	m_filter_rank_choice->Append(_T("5"), wxBitmap(rank4_xpm));
	m_filter_rank_choice->Append(_T("6"), wxBitmap(rank5_xpm));
	m_filter_rank_choice->Append(_T("7"), wxBitmap(rank6_xpm));
	m_filter_rank_choice->Append(_T("8"), wxBitmap(rank7_xpm));
	long value = 0;
	f_values.rank.ToLong(&value);
	m_filter_rank_choice->SetSelection(value);
	m_filter_rank_choice->SetMinSize(wxSize(60, -1));

	wxBoxSizer* m_filter_rank_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_rank_sizer->Add(m_filter_rank_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_rank_sizer->Add(m_filter_rank_button, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_rank_sizer->Add(m_filter_rank_choice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	m_filter_description_text = new wxStaticText(this, wxID_ANY, _("Description:"));
	m_filter_description_text->Wrap(-1);
	m_filter_description_text->SetMinSize(wxSize(90, -1));
	m_filter_description_edit = new wxTextCtrl(this, BATTLE_FILTER_DESCRIPTION_EDIT, f_values.description, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	m_filter_description_edit->SetMinSize(wxSize(220, -1));

	wxBoxSizer* m_filter_description_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_description_sizer->Add(m_filter_description_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_description_sizer->Add(m_filter_description_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	m_filter_status_text1 = new wxStaticText(this, wxID_ANY, _("Status:"), wxPoint(-1, -1));
	m_filter_status_text1->Wrap(-1);
	m_filter_status_start = new wxCheckBox(this, BATTLE_FILTER_STARTED, _("Started"));
	m_filter_status_start->SetValue(f_values.status_start);
	m_filter_status_full = new wxCheckBox(this, BATTLE_FILTER_FULL, _("Full"));
	m_filter_status_full->SetValue(f_values.status_full);
	m_filter_status_open = new wxCheckBox(this, BATTLE_FILTER_OPEN, _("Open"));
	m_filter_status_open->SetValue(f_values.status_open);

	wxBoxSizer* m_filter_sizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_filter_sizer2->Add(m_filter_status_text1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_sizer2->Add(m_filter_status_start, 0, wxALL | wxEXPAND, 5);
	m_filter_sizer2->Add(m_filter_status_full, 0, wxALL | wxEXPAND, 5);
	m_filter_sizer2->Add(m_filter_status_open, 0, wxALL | wxEXPAND, 5);


	m_filter_player_text = new wxStaticText(this, wxID_ANY, _("Player:"));
	m_filter_player_text->Wrap(-1);
	m_filter_player_button = new wxButton(this, BATTLE_FILTER_PLAYER_BUTTON, f_values.player_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_player_choice = new wxChoice(this, BATTLE_FILTER_PLAYER_CHOICE, wxDefaultPosition, wxDefaultSize, choiceArray);
	value = 0;
	f_values.player_num.ToLong(&value);
	m_filter_player_choice->SetSelection(value);
	m_filter_player_choice->SetMinSize(wxSize(60, -1));

	wxBoxSizer* m_filter_player_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_player_sizer->Add(m_filter_player_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_player_sizer->Add(m_filter_player_button, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_player_sizer->Add(m_filter_player_choice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	wxBoxSizer* m_filter_body_row2_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_body_row2_sizer->Add(m_filter_description_sizer, 0, wxEXPAND, 5);
	m_filter_body_row2_sizer->Add(m_filter_sizer2, 1, wxEXPAND, 5);
	m_filter_body_row2_sizer->Add(m_filter_player_sizer, 0, wxEXPAND | wxLEFT, 5);


	m_filter_map_text = new wxStaticText(this, wxID_ANY, _("Map:"));
	m_filter_map_text->Wrap(-1);
	m_filter_map_text->SetMinSize(wxSize(90, -1));
	m_filter_map_edit = new wxTextCtrl(this, BATTLE_FILTER_MAP_EDIT, f_values.map, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	m_filter_map_edit->SetMinSize(wxSize(220, -1));
	m_filter_map_show = new wxCheckBox(this, BATTLE_FILTER_MAP_SHOW, _("Only maps I have"));
	m_filter_map_show->SetValue(f_values.map_show);
	m_filter_map_show->SetMinSize(wxSize(220, -1));

	wxBoxSizer* m_filter_map_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_map_sizer->Add(m_filter_map_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_map_sizer->Add(m_filter_map_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_map_sizer->Add(m_filter_map_show, 1, wxEXPAND | wxALL, 5);


	m_filter_maxplayer_text = new wxStaticText(this, wxID_ANY, _("Max Players:"));
	m_filter_maxplayer_text->Wrap(-1);
	m_filter_maxplayer_button = new wxButton(this, BATTLE_FILTER_MAXPLAYER_BUTTON, f_values.maxplayer_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_maxplayer_choice = new wxChoice(this, BATTLE_FILTER_MAXPLAYER_CHOICE, wxDefaultPosition, wxDefaultSize, choiceArray);
	value = 0;
	f_values.maxplayer.ToLong(&value);
	m_filter_maxplayer_choice->SetSelection(value);
	m_filter_maxplayer_choice->SetMinSize(wxSize(60, -1));

	wxBoxSizer* m_filter_maxplayer_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_maxplayer_sizer->Add(m_filter_maxplayer_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_maxplayer_sizer->Add(m_filter_maxplayer_button, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_maxplayer_sizer->Add(m_filter_maxplayer_choice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	wxBoxSizer* m_filter_body_row3_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_body_row3_sizer->Add(m_filter_map_sizer, 1, wxEXPAND, 5);
	m_filter_body_row3_sizer->Add(m_filter_maxplayer_sizer, 0, wxEXPAND, 5);


	m_filter_game_text = new wxStaticText(this, wxID_ANY, _("Game:"));
	m_filter_game_text->Wrap(-1);
	m_filter_game_text->SetMinSize(wxSize(90, -1));
	m_filter_game_edit = new wxTextCtrl(this, BATTLE_FILTER_GAME_EDIT, f_values.game, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
	m_filter_game_edit->SetMinSize(wxSize(220, -1));
	m_filter_game_show = new wxCheckBox(this, BATTLE_FILTER_GAME_SHOW, _("Only games I have"));
	m_filter_game_show->SetValue(f_values.game_show);
	m_filter_game_show->SetMinSize(wxSize(220, -1));

	wxBoxSizer* m_filter_game_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_game_sizer->Add(m_filter_game_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_game_sizer->Add(m_filter_game_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_game_sizer->Add(m_filter_game_show, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	m_filter_spectator_text = new wxStaticText(this, wxID_ANY, _("    Spectator:"));
	m_filter_spectator_text->Wrap(-1);
	m_filter_spectator_button = new wxButton(this, BATTLE_FILTER_SPECTATOR_BUTTON, f_values.spectator_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_spectator_choice = new wxChoice(this, BATTLE_FILTER_SPECTATOR_CHOICE, wxDefaultPosition, wxDefaultSize, choiceArray);
	value = 0;
	f_values.spectator.ToLong(&value);
	m_filter_spectator_choice->SetSelection(value);
	m_filter_spectator_choice->SetMinSize(wxSize(60, -1));

	wxBoxSizer* m_filter_spectator_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_spectator_sizer->Add(m_filter_spectator_text, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
	m_filter_spectator_sizer->Add(m_filter_spectator_button, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_spectator_sizer->Add(m_filter_spectator_choice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	wxBoxSizer* m_filter_body_row4_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_body_row4_sizer->Add(m_filter_game_sizer, 1, wxEXPAND, 5);
	m_filter_body_row4_sizer->Add(m_filter_spectator_sizer, 0, wxEXPAND, 5);


	m_filter_host_edit = new wxTextCtrl(this, BATTLE_FILTER_HOST_EDIT, f_values.host, wxDefaultPosition, wxDefaultSize, 0 | wxSIMPLE_BORDER);
	m_filter_host_edit->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, wxFONTWEIGHT_NORMAL, false, wxEmptyString));
	m_filter_host_edit->SetMinSize(wxSize(220, -1));
	m_filter_host_text = new wxStaticText(this, wxID_ANY, _("Host:"));
	m_filter_host_text->Wrap(-1);
	m_filter_host_text->SetMinSize(wxSize(90, -1));

	wxBoxSizer* m_filter_column_1 = new wxBoxSizer(wxHORIZONTAL);
	m_filter_column_1->Add(m_filter_host_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	m_filter_column_1->Add(m_filter_host_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	wxBoxSizer* m_filter_body_row1_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_filter_body_row1_sizer->Add(m_filter_column_1, 0, wxEXPAND, 5);
	m_filter_body_row1_sizer->Add(m_filter_status_sizer1, 1, wxEXPAND, 5);
	m_filter_body_row1_sizer->Add(m_filter_rank_sizer, 0, wxEXPAND, 5);


	wxStaticBoxSizer* m_filter_body_sizer;
	m_filter_body_sizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, wxEmptyString), wxVERTICAL);
	m_filter_body_sizer->Add(m_filter_body_row1_sizer, 1, wxEXPAND, 5);
	m_filter_body_sizer->Add(m_filter_body_row2_sizer, 1, wxEXPAND, 5);
	m_filter_body_sizer->Add(m_filter_body_row3_sizer, 1, wxEXPAND, 5);
	m_filter_body_sizer->Add(m_filter_body_row4_sizer, 1, wxEXPAND, 5);


	m_activ = false;
	m_filter_rank_mode = _GetButtonMode(f_values.rank_mode);
	m_filter_player_mode = _GetButtonMode(f_values.player_mode);
	m_filter_maxplayer_mode = _GetButtonMode(f_values.maxplayer_mode);
	m_filter_spectator_mode = _GetButtonMode(f_values.spectator_mode);

	//Validate choises
	if (m_filter_rank_choice->GetSelection() == -1)
		m_filter_rank_choice->SetSelection(0);
	if (m_filter_player_choice->GetSelection() == -1)
		m_filter_player_choice->SetSelection(0);
	if (m_filter_maxplayer_choice->GetSelection() == -1)
		m_filter_maxplayer_choice->SetSelection(0);
	if (m_filter_spectator_choice->GetSelection() == -1)
		m_filter_spectator_choice->SetSelection(0);

	m_filter_rank_choice_value = m_filter_rank_choice->GetSelection() - 1;
	m_filter_player_choice_value = m_filter_player_choice->GetSelection() - 1;
	m_filter_maxplayer_choice_value = m_filter_maxplayer_choice->GetSelection() - 1;
	m_filter_spectator_choice_value = m_filter_spectator_choice->GetSelection() - 1;

	SetSizer(m_filter_body_sizer);
	Layout();
	m_filter_body_sizer->Fit(this);

	m_filter_map_expression = new wxRegEx(m_filter_map_edit->GetValue(), wxRE_ICASE);
	m_filter_game_expression = new wxRegEx(m_filter_game_edit->GetValue(), wxRE_ICASE);
	m_filter_description_expression = new wxRegEx(m_filter_description_edit->GetValue(), wxRE_ICASE);
	m_filter_host_expression = new wxRegEx(m_filter_host_edit->GetValue(), wxRE_ICASE);

	wxCommandEvent dummy;
	OnChange(dummy);
}

BattleListFilter::~BattleListFilter()
{
	delete m_filter_map_expression;
	delete m_filter_game_expression;
	delete m_filter_description_expression;
	delete m_filter_host_expression;
}

BattleListFilter::ButtonMode BattleListFilter::_GetButtonMode(const wxString& sign)
{
	if (sign == _T( "<" ))
		return BattleListFilter::BUTTON_MODE_SMALLER;
	if (sign == _T( ">" ))
		return BattleListFilter::BUTTON_MODE_BIGGER;
	return BattleListFilter::BUTTON_MODE_EQUAL;
}

wxString BattleListFilter::_GetButtonSign(const BattleListFilter::ButtonMode value)
{
	switch (value) {
		case BUTTON_MODE_EQUAL:
			return _T( "=" );
		case BUTTON_MODE_SMALLER:
			return _T( "<" );
		case BUTTON_MODE_BIGGER:
		default:
			return _T( ">" );
	}
}


BattleListFilter::ButtonMode BattleListFilter::_GetNextMode(const BattleListFilter::ButtonMode value)
{
	switch (value) {
		case BUTTON_MODE_EQUAL:
			return BUTTON_MODE_SMALLER;
		case BUTTON_MODE_SMALLER:
			return BUTTON_MODE_BIGGER;
		case BUTTON_MODE_BIGGER:
		default:
			return BUTTON_MODE_EQUAL;
	}
}

bool BattleListFilter::_IntCompare(const int a, const int b, const BattleListFilter::ButtonMode mode)
{
	switch (mode) {
		case BUTTON_MODE_EQUAL:
			return (a == b);
		case BUTTON_MODE_SMALLER:
			return (a < b);
		case BUTTON_MODE_BIGGER:
			return (a > b);
		default:
			return false;
	}
}

bool BattleListFilter::StringMatches(const wxString& input, const wxString& filter_string, const wxRegEx* filter_regex, StringTransformFunction additional_transform, bool case_sensitive)
{
	if (filter_string.Len() < 1 || filter_string == wxEmptyString)
		return true;

	wxString input_cased(input);
	wxString filter_cased(filter_string);
	bool use_regex(filter_regex && filter_regex->IsValid());

	/* Case-(in)sensitivize the input and filter strings. */
	if (!case_sensitive) {
		input_cased.MakeUpper();
		filter_cased.MakeUpper();
	}

	if (input_cased.Find(filter_cased) != wxNOT_FOUND || (use_regex && filter_regex->Matches(input)))
		return true;
	else if (additional_transform) {
		/* Try matching using a transformed input string. */
		wxString input_generated(additional_transform(input));
		if (!case_sensitive)
			input_generated.MakeUpper();

		if (input_generated.Find(filter_cased) != wxNOT_FOUND || (use_regex && filter_regex->Matches(input_generated)))
			return true;
	}

	return false;
}

void BattleListFilter::OnRankButton(wxCommandEvent& event)
{
	m_filter_rank_mode = _GetNextMode(m_filter_rank_mode);
	m_filter_rank_button->SetLabel(_GetButtonSign(m_filter_rank_mode));
	OnChange(event);
}

void BattleListFilter::OnPlayerButton(wxCommandEvent& event)
{
	m_filter_player_mode = _GetNextMode(m_filter_player_mode);
	m_filter_player_button->SetLabel(_GetButtonSign(m_filter_player_mode));
	OnChange(event);
}


void BattleListFilter::OnMaxPlayerButton(wxCommandEvent& event)
{
	m_filter_maxplayer_mode = _GetNextMode(m_filter_maxplayer_mode);
	m_filter_maxplayer_button->SetLabel(_GetButtonSign(m_filter_maxplayer_mode));
	OnChange(event);
}


void BattleListFilter::OnSpectatorButton(wxCommandEvent& event)
{
	m_filter_spectator_mode = _GetNextMode(m_filter_spectator_mode);
	m_filter_spectator_button->SetLabel(_GetButtonSign(m_filter_spectator_mode));
	OnChange(event);
}

void BattleListFilter::SetActiv(bool state)
{
	m_activ = state;
	if (m_parent_battlelisttab != 0) {
		m_parent_battlelisttab->UpdateList();
	}
}

bool BattleListFilter::FilterBattle(IBattle& battle)
{
	if (!m_activ)
		return true;

	if (m_filter_highlighted->IsChecked() |
	    m_filter_status_start->IsChecked() |
	    m_filter_status_locked->IsChecked() |
	    m_filter_status_pass->IsChecked() |
	    m_filter_status_full->IsChecked() |
	    m_filter_status_open->IsChecked()) {
		bool bResult = false;

		if (m_filter_highlighted->IsChecked()) {
			try {
				wxString host = TowxString(battle.GetFounder().GetNick());
				bResult = useractions().DoActionOnUser(UserActions::ActHighlight, host);

				if (!bResult)
					for (unsigned int i = 0; i < battle.GetNumUsers(); ++i) {
						wxString name = TowxString(battle.GetUser(i).GetNick());
						if (useractions().DoActionOnUser(UserActions::ActHighlight, name)) {
							bResult = true;
							break;
						}
					}
			} catch (const std::exception& e) {
				wxLogWarning(_T("Exception: %s"), e.what());
			}
		}


		//Battle Status Check
		if (m_filter_status_start->GetValue())
			bResult |= battle.GetInGame();

		if (m_filter_status_locked->GetValue())
			bResult |= battle.IsLocked();

		if (m_filter_status_pass->GetValue())
			bResult |= battle.IsPassworded();

		if (m_filter_status_full->GetValue())
			bResult |= battle.IsFull();

		if (m_filter_status_open->GetValue())
			bResult |= (!battle.IsPassworded() && !battle.IsLocked() && !battle.GetInGame() && !battle.IsFull());

		if (!bResult)
			return false;
	}

	//Rank Check

	/** @fixme Is `nonsenserank' useful, or can it be removed?  Why is
   * it here in the first place?
   */
	/* `Nonsense', in this context, apparently means that the battle
   * requires rank 100, exactly, AND we're filtering for values less
   * than some number.
   */
	bool nonsenserank = (m_filter_rank_mode == BUTTON_MODE_SMALLER) && (battle.GetRankNeeded() == 100);

	if (m_filter_rank_choice_value != -1 /* don't have "all" selected */
	    && !nonsenserank		     /* Nonsensical `nonsenserank' flag isn't set. */
	    && !_IntCompare(battle.GetRankNeeded(), m_filter_rank_choice_value, m_filter_rank_mode))
		return false;

	//Player Check
	if ((m_filter_player_choice_value != -1) && !_IntCompare(battle.GetNumUsers() - battle.GetSpectators(), m_filter_player_choice_value, m_filter_player_mode))
		return false;

	//MaxPlayer Check
	if ((m_filter_maxplayer_choice_value != -1) && !_IntCompare(battle.GetMaxPlayers(), m_filter_maxplayer_choice_value, m_filter_maxplayer_mode))
		return false;

	//Spectator Check
	if ((m_filter_spectator_choice_value != -1) && !_IntCompare(battle.GetSpectators(), m_filter_spectator_choice_value, m_filter_spectator_mode))
		return false;

	//Only Maps I have Check
	if (m_filter_map_show->GetValue() && !battle.MapExists())
		return false;

	//Only Games I have Check
	if (m_filter_game_show->GetValue() && !battle.GameExists())
		return false;

	//Strings Plain Text & RegEx Check (Case insensitiv)

	//Description:
	if (!StringMatches(TowxString(battle.GetDescription()),
			   m_filter_description_edit->GetValue(),
			   m_filter_description_expression))
		return false;

	//Host:
	try { //!TODO
		if (!StringMatches(TowxString(battle.GetFounder().GetNick()),
				   m_filter_host_edit->GetValue(),
				   m_filter_host_expression))
			return false;
	} catch (const std::exception& e) {
		wxLogWarning(_T("Exception: %s"), e.what());
	}

	//Map:
	if (!StringMatches(TowxString(battle.GetHostMapName()),
			   m_filter_map_edit->GetValue(),
			   m_filter_map_expression))
		return false;

	//Game:
	if (!StringMatches(TowxString(battle.GetHostGameNameAndVersion()),
			   m_filter_game_edit->GetValue(),
			   m_filter_game_expression))
		return false;

	return true;
}

void BattleListFilter::OnChange(wxCommandEvent& /*unused*/)
{
	if (!m_activ)
		return;
	m_parent_battlelisttab->UpdateList();
}

void BattleListFilter::OnChangeMap(wxCommandEvent& event)
{
	if (m_filter_map_edit == NULL)
		return;

	if (m_filter_map_expression == NULL)
		m_filter_map_expression = new wxRegEx(m_filter_map_edit->GetValue(), wxRE_ICASE);
	else
		m_filter_map_expression->Compile(m_filter_map_edit->GetValue(), wxRE_ICASE);

	OnChange(event);
}

void BattleListFilter::OnChangeGame(wxCommandEvent& event)
{
	if (m_filter_game_edit == NULL)
		return;

	if (m_filter_game_expression == NULL)
		m_filter_game_expression = new wxRegEx(m_filter_game_edit->GetValue(), wxRE_ICASE);
	else
		m_filter_game_expression->Compile(m_filter_game_edit->GetValue(), wxRE_ICASE);

	OnChange(event);
}

void BattleListFilter::OnChangeDescription(wxCommandEvent& event)
{
	if (m_filter_description_edit == NULL)
		return;

	if (m_filter_description_expression == NULL)
		m_filter_description_expression = new wxRegEx(m_filter_description_edit->GetValue(), wxRE_ICASE);
	else
		m_filter_description_expression->Compile(m_filter_description_edit->GetValue(), wxRE_ICASE);

	OnChange(event);
}

void BattleListFilter::OnChangeHost(wxCommandEvent& event)
{
	if (m_filter_host_edit == NULL)
		return;

	if (m_filter_host_expression == NULL)
		m_filter_host_expression = new wxRegEx(m_filter_host_edit->GetValue(), wxRE_ICASE);
	else
		m_filter_host_expression->Compile(m_filter_host_edit->GetValue(), wxRE_ICASE);

	OnChange(event);
}


void BattleListFilter::OnRankChange(wxCommandEvent& event)
{
	m_filter_rank_choice_value = m_filter_rank_choice->GetSelection() - 1;
	OnChange(event);
}


void BattleListFilter::OnPlayerChange(wxCommandEvent& event)
{
	m_filter_player_choice_value = m_filter_player_choice->GetSelection() - 1;
	OnChange(event);
}


void BattleListFilter::OnMaxPlayerChange(wxCommandEvent& event)
{
	m_filter_maxplayer_choice_value = m_filter_maxplayer_choice->GetSelection() - 1;
	OnChange(event);
}


void BattleListFilter::OnSpectatorChange(wxCommandEvent& event)
{
	m_filter_spectator_choice_value = m_filter_spectator_choice->GetSelection() - 1;
	OnChange(event);
}


bool BattleListFilter::GetActiv() const
{
	return m_activ;
}

void BattleListFilter::SaveFilterValues()
{
	BattleListFilterValues filtervalues;
	filtervalues.description = m_filter_description_edit->GetValue();
	filtervalues.game = m_filter_game_edit->GetValue();
	filtervalues.game_show = m_filter_game_show->GetValue();
	filtervalues.host = m_filter_host_edit->GetValue();
	filtervalues.map = m_filter_map_edit->GetValue();
	filtervalues.map_show = m_filter_map_show->GetValue();
	filtervalues.map = m_filter_map_edit->GetValue();
	filtervalues.maxplayer = wxString::Format(_T("%d"), m_filter_maxplayer_choice->GetSelection());
	filtervalues.maxplayer_mode = _GetButtonSign(m_filter_maxplayer_mode);
	filtervalues.player_mode = _GetButtonSign(m_filter_player_mode);
	filtervalues.player_num = wxString::Format(_T("%d"), m_filter_player_choice->GetSelection());
	filtervalues.rank = wxString::Format(_T("%d"), m_filter_rank_choice->GetSelection());
	filtervalues.rank_mode = _GetButtonSign(m_filter_rank_mode);
	filtervalues.spectator = wxString::Format(_T("%d"), m_filter_spectator_choice->GetSelection());
	filtervalues.spectator_mode = _GetButtonSign(m_filter_spectator_mode);
	filtervalues.status_full = m_filter_status_full->IsChecked();
	filtervalues.status_locked = m_filter_status_locked->IsChecked();
	filtervalues.status_open = m_filter_status_open->IsChecked();
	filtervalues.status_passworded = m_filter_status_pass->IsChecked();
	filtervalues.status_start = m_filter_status_start->IsChecked();
	filtervalues.highlighted_only = m_filter_highlighted->IsChecked();
	SetBattleFilterValues(filtervalues);
}


BattleListFilterValues BattleListFilter::GetBattleFilterValues(const wxString& profile_name)
{
	BattleListFilterValues filtervalues;
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/description" ), &filtervalues.description);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/host" ), &filtervalues.host);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/map" ), &filtervalues.map);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/map_show" ), &filtervalues.map_show);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer" ), &filtervalues.maxplayer);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer_mode" ), &filtervalues.maxplayer_mode);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/mod" ), &filtervalues.game);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/mod_show" ), &filtervalues.game_show);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/player_mode" ), &filtervalues.player_mode);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/player_num" ), &filtervalues.player_num);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/rank" ), &filtervalues.rank);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/rank_mode" ), &filtervalues.rank_mode);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/spectator" ), &filtervalues.spectator);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/spectator_mode" ), &filtervalues.spectator_mode);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/status_full" ), &filtervalues.status_full);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/status_locked" ), &filtervalues.status_locked);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/status_open" ), &filtervalues.status_open);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/status_passworded" ), &filtervalues.status_passworded);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/status_start" ), &filtervalues.status_start);
	cfg().Read(_T( "/BattleFilter/" ) + profile_name + _T( "/highlighted_only" ), &filtervalues.highlighted_only);
	return filtervalues;
}

void BattleListFilter::SetBattleFilterValues(const BattleListFilterValues& filtervalues, const wxString& profile_name)
{
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/description" ), filtervalues.description);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/host" ), filtervalues.host);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/map" ), filtervalues.map);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/map_show" ), filtervalues.map_show);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer" ), filtervalues.maxplayer);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/maxplayer_mode" ), filtervalues.maxplayer_mode);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/mod" ), filtervalues.game);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/mod_show" ), filtervalues.game_show);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/player_mode" ), filtervalues.player_mode);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/player_num" ), filtervalues.player_num);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/rank" ), filtervalues.rank);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/rank_mode" ), filtervalues.rank_mode);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/spectator" ), filtervalues.spectator);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/spectator_mode" ), filtervalues.spectator_mode);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/status_full" ), filtervalues.status_full);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/status_locked" ), filtervalues.status_locked);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/status_open" ), filtervalues.status_open);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/status_passworded" ), filtervalues.status_passworded);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/status_start" ), filtervalues.status_start);
	cfg().Write(_T( "/BattleFilter/" ) + profile_name + _T( "/highlighted_only" ), filtervalues.highlighted_only);
	cfg().Write(_T( "/BattleFilter/lastprofile" ), profile_name);
}

wxString BattleListFilter::GetLastBattleFilterProfileName()
{
	return cfg().Read(_T( "/BattleFilter/lastprofile" ), _T( "default" ));
}
