/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "playbackfilter.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/event.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif

#include "battle.h"
#include "gui/uiutils.h"
#include "offlinebattle.h"
#include "playbackfiltervalues.h"
#include "playbacktab.h"
#include "replaylist.h"
#include "settings.h"
#include "storedgame.h"
#include "utils/conversion.h"
#include "utils/tasutil.h"
//#include "PlaybackListFiltervalues.h"
///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(PlaybackListFilter, wxPanel)

EVT_BUTTON(PLAYBACK_FILTER_PLAYER_BUTTON, PlaybackListFilter::OnPlayerButton)
EVT_BUTTON(PLAYBACK_FILTER_DURATION_BUTTON, PlaybackListFilter::OnDurationButton)
EVT_BUTTON(PLAYBACK_FILTER_FILESIZE_BUTTON, PlaybackListFilter::OnFilesizeButton)
EVT_CHOICE(PLAYBACK_FILTER_PLAYER_CHOICE, PlaybackListFilter::OnPlayerChange)
EVT_TEXT(PLAYBACK_FILTER_DURATION_EDIT, PlaybackListFilter::OnChangeDuration)
EVT_TEXT(PLAYBACK_FILTER_FILESIZE_EDIT, PlaybackListFilter::OnChangeFilesize)
EVT_TEXT(PLAYBACK_FILTER_MAP_EDIT, PlaybackListFilter::OnChangeMap)
EVT_TEXT(PLAYBACK_FILTER_MOD_EDIT, PlaybackListFilter::OnChangeMod)
EVT_CHECKBOX(PLAYBACK_FILTER_MAP_SHOW, PlaybackListFilter::OnChange)
EVT_CHECKBOX(PLAYBACK_FILTER_MOD_SHOW, PlaybackListFilter::OnChange)

END_EVENT_TABLE()


PlaybackListFilter::PlaybackListFilter(wxWindow* parent, wxWindowID id, PlaybackTab* parentTab,
				       const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
    , m_parent_tab(parentTab)
    , m_duration_value(0)
    , m_filter_map_edit(0)
    , m_filter_map_expression(0)
    , m_filter_mod_edit(0)
    , m_filter_mod_expression(0)

{
	PlaybackListFilterValues f_values = sett().GetReplayFilterValues(sett().GetLastReplayFilterProfileName());

	wxBoxSizer* m_filter_sizer;
	m_filter_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* m_filter_body_sizer;
	m_filter_body_sizer = new wxStaticBoxSizer(new wxStaticBox(this, -1, _("Filter settings")), wxVERTICAL);

	wxBoxSizer* m_filter_body_row2_sizer;
	m_filter_body_row2_sizer = new wxBoxSizer(wxHORIZONTAL);

	//	wxBoxSizer* m_filter_player_sizer;
	//	m_filter_player_sizer = new wxBoxSizer( wxHORIZONTAL );

	m_filter_player_text = new wxStaticText(this, wxID_ANY, _("Player:"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_player_text->Wrap(-1);
	m_filter_player_text->SetMinSize(wxSize(90, -1));
	m_filter_body_row2_sizer->Add(m_filter_player_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	wxArrayString m_filter_player_choiceChoices;

	m_filter_player_choiceChoices.Add(_("All"));
	for (wxLongLong i = 0; i <= 32; i++)
		m_filter_player_choiceChoices.Add(i.ToString());

	m_filter_player_choice = new wxChoice(this, PLAYBACK_FILTER_PLAYER_CHOICE, wxDefaultPosition, wxSize(-1, -1), m_filter_player_choiceChoices, 0);
	long value = 0;
	value = f_values.player_num.ToLong(&value);
	m_filter_player_choice->SetSelection(value);
	if (m_filter_player_choice->GetSelection() == -1)
		m_filter_player_choice->SetSelection(0);

	m_filter_player_choice->SetMinSize(wxSize(140, -1));

	m_filter_body_row2_sizer->Add(m_filter_player_choice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_player_button = new wxButton(this, PLAYBACK_FILTER_PLAYER_BUTTON, f_values.player_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_body_row2_sizer->Add(m_filter_player_button, 0, wxALIGN_LEFT | wxALL | wxALIGN_CENTER_VERTICAL, 5);


	////map
	wxBoxSizer* m_filter_body_row3_sizer;
	m_filter_body_row3_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* m_filter_map_sizer;
	m_filter_map_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_filter_map_text = new wxStaticText(this, wxID_ANY, _("Map:"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_map_text->Wrap(-1);
	m_filter_map_text->SetMinSize(wxSize(90, -1));

	m_filter_map_sizer->Add(m_filter_map_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_map_edit = new wxTextCtrl(this, PLAYBACK_FILTER_MAP_EDIT, f_values.map, wxDefaultPosition, wxSize(-1, -1), 0 | wxSIMPLE_BORDER);
	m_filter_map_edit->SetMinSize(wxSize(140, -1));
	m_filter_map_expression = new wxRegEx(m_filter_map_edit->GetValue(), wxRE_ICASE);

	m_filter_map_sizer->Add(m_filter_map_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_body_row3_sizer->Add(m_filter_map_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* m_filter_only_map_sizer;
	m_filter_only_map_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_filter_map_show = new wxCheckBox(this, PLAYBACK_FILTER_MAP_SHOW, _("Only maps I have"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_map_show->SetValue(f_values.map_show);
	m_filter_map_show->SetMinSize(wxSize(140, -1));

	m_filter_only_map_sizer->Add(m_filter_map_show, 0, wxEXPAND | wxALIGN_LEFT | wxALL, 5);

	m_filter_body_row3_sizer->Add(m_filter_only_map_sizer, 0, wxEXPAND, 5);


	//// mod
	wxBoxSizer* m_filter_body_row4_sizer;
	m_filter_body_row4_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* m_filter_mod_sizer;
	m_filter_mod_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_filter_mod_text = new wxStaticText(this, wxID_ANY, _("Game:"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_mod_text->Wrap(-1);
	m_filter_mod_text->SetMinSize(wxSize(90, -1));

	m_filter_mod_sizer->Add(m_filter_mod_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_mod_edit = new wxTextCtrl(this, PLAYBACK_FILTER_MOD_EDIT, f_values.mod, wxDefaultPosition, wxSize(-1, -1), 0 | wxSIMPLE_BORDER);
	m_filter_mod_edit->SetMinSize(wxSize(140, -1));
	m_filter_mod_expression = new wxRegEx(m_filter_mod_edit->GetValue(), wxRE_ICASE);

	m_filter_mod_sizer->Add(m_filter_mod_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_body_row4_sizer->Add(m_filter_mod_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* m_filter_only_mod_sizer;
	m_filter_only_mod_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_filter_mod_show = new wxCheckBox(this, PLAYBACK_FILTER_MOD_SHOW, _("Only games I have"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_mod_show->SetValue(f_values.mod_show);
	m_filter_mod_show->SetMinSize(wxSize(140, -1));

	m_filter_only_mod_sizer->Add(m_filter_mod_show, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 5);

	m_filter_body_row4_sizer->Add(m_filter_only_mod_sizer, 0, wxALIGN_LEFT | wxEXPAND, 5);


	/////

	///// filesize
	wxBoxSizer* m_filter_body_row5_sizer;
	m_filter_body_row5_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* m_filter_filesize_sizer;
	m_filter_filesize_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_filter_filesize_text = new wxStaticText(this, wxID_ANY, _("Filesize in KB:"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_filesize_text->Wrap(-1);
	m_filter_filesize_text->SetMinSize(wxSize(90, -1));

	m_filter_filesize_sizer->Add(m_filter_filesize_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_filesize_edit = new wxTextCtrl(this, PLAYBACK_FILTER_FILESIZE_EDIT, f_values.filesize, wxDefaultPosition, wxSize(-1, -1), 0 | wxSIMPLE_BORDER);
	m_filter_filesize_edit->SetMinSize(wxSize(140, -1));

	m_filter_filesize_sizer->Add(m_filter_filesize_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_body_row5_sizer->Add(m_filter_filesize_sizer, 0, wxEXPAND, 5);

	//button here
	m_filter_filesize_button = new wxButton(this, PLAYBACK_FILTER_FILESIZE_BUTTON, f_values.filesize_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_body_row5_sizer->Add(m_filter_filesize_button, 0, wxALIGN_LEFT | wxALL | wxALIGN_CENTER_VERTICAL, 5);

	/////

	///// duration
	wxBoxSizer* m_filter_body_row6_sizer;
	m_filter_body_row6_sizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* m_filter_duration_sizer;
	m_filter_duration_sizer = new wxBoxSizer(wxHORIZONTAL);

	m_filter_duration_text = new wxStaticText(this, wxID_ANY, _("Duration (hh:mm:ss):"), wxDefaultPosition, wxSize(-1, -1), 0);
	m_filter_duration_text->Wrap(-1);
	m_filter_duration_text->SetMinSize(wxSize(90, -1));

	m_filter_duration_sizer->Add(m_filter_duration_text, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_duration_edit = new wxTextCtrl(this, PLAYBACK_FILTER_DURATION_EDIT, f_values.duration, wxDefaultPosition, wxSize(-1, -1), 0 | wxSIMPLE_BORDER);
	m_filter_duration_edit->SetMinSize(wxSize(140, -1));

	m_filter_duration_sizer->Add(m_filter_duration_edit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	m_filter_body_row6_sizer->Add(m_filter_duration_sizer, 0, wxEXPAND, 5);

	m_filter_duration_button = new wxButton(this, PLAYBACK_FILTER_DURATION_BUTTON, f_values.duration_mode, wxDefaultPosition, wxSize(25, 25), 0);
	m_filter_body_row6_sizer->Add(m_filter_duration_button, 0, wxALIGN_LEFT | wxALL | wxALIGN_CENTER_VERTICAL, 5);

	/////

	//bring all sizers together

	wxBoxSizer* m_col1_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* m_col2_sizer = new wxBoxSizer(wxVERTICAL);

	m_col1_sizer->Add(m_filter_body_row2_sizer, 1, wxEXPAND, 5);
	m_col1_sizer->Add(m_filter_body_row3_sizer, 1, wxEXPAND, 5);
	m_col1_sizer->Add(m_filter_body_row4_sizer, 1, wxEXPAND, 5);
	m_col2_sizer->Add(m_filter_body_row5_sizer, 1, wxEXPAND, 5);
	m_col2_sizer->Add(m_filter_body_row6_sizer, 1, wxEXPAND, 5);


	m_filter_sizer->Add(m_col1_sizer, 1, wxEXPAND, 5);
	m_filter_sizer->Add(m_col2_sizer, 1, wxEXPAND, 5);

	m_activ = false;
	m_filter_player_mode = _GetButtonMode(f_values.player_mode);
	m_filter_duration_mode = _GetButtonMode(f_values.duration_mode);
	m_filter_filesize_mode = _GetButtonMode(f_values.filesize_mode);
	m_filter_player_choice_value = m_filter_player_choice->GetSelection() - 1;

	m_filter_body_sizer->Add(m_filter_sizer);
	this->SetSizer(m_filter_body_sizer);
	this->Layout();
	m_filter_sizer->Fit(this);

	delete m_filter_map_expression;
	m_filter_map_expression = new wxRegEx(m_filter_map_edit->GetValue(), wxRE_ICASE);
	delete m_filter_mod_expression;
	m_filter_mod_expression = new wxRegEx(m_filter_mod_edit->GetValue(), wxRE_ICASE);

	wxCommandEvent dummy;
	OnChange(dummy);
}

PlaybackListFilter::~PlaybackListFilter()
{
	delete m_filter_map_expression;
	delete m_filter_mod_expression;
}

PlaybackListFilter::m_button_mode PlaybackListFilter::_GetButtonMode(wxString sign)
{
	if (sign == _T("<"))
		return m_smaller;
	if (sign == _T(">"))
		return m_bigger;
	return m_equal;
}

wxString PlaybackListFilter::_GetButtonSign(m_button_mode value)
{
	switch (value) {
		case m_equal:
			return _T("=");
		case m_smaller:
			return _T("<");
		case m_bigger:
		default:
			return _T(">");
	}
}

PlaybackListFilter::m_button_mode PlaybackListFilter::_GetNextMode(m_button_mode value)
{
	switch (value) {
		case m_equal:
			return m_smaller;
		case m_smaller:
			return m_bigger;
		case m_bigger:
		default:
			return m_equal;
	}
}

void PlaybackListFilter::OnPlayerButton(wxCommandEvent& event)
{
	m_filter_player_mode = _GetNextMode(m_filter_player_mode);
	m_filter_player_button->SetLabel(_GetButtonSign(m_filter_player_mode));
	OnChange(event);
}

void PlaybackListFilter::SetActiv(bool state)
{
	m_activ = state;
	if (m_parent_tab != 0) {
		m_parent_tab->UpdateList();
	}
}

bool PlaybackListFilter::_IntCompare(int a, int b, m_button_mode mode)
{
	switch (mode) {
		case m_equal:
			return (a == b);
		case m_smaller:
			return (a < b);
		case m_bigger:
			return (a > b);
		default:
			return false;
	}
}

bool PlaybackListFilter::FilterPlayback(const StoredGame& playback)
{

	if (!m_activ)
		return true;

	const OfflineBattle& battle = playback.battle;
	//Player Check
	if ((m_filter_player_choice_value != -1) && !_IntCompare(battle.GetNumUsers() - battle.GetSpectators(), m_filter_player_choice_value, m_filter_player_mode))
		return false;

	//Only Maps i have Check
	if (m_filter_map_show->GetValue() && !battle.MapExists())
		return false;

	//Only Mods i have Check
	if (m_filter_mod_show->GetValue() && !battle.GameExists())
		return false;

	//Strings Plain Text & RegEx Check (Case insensitiv)

	//Map:
	if (!TowxString(battle.GetHostMapName()).Upper().Contains(m_filter_map_edit->GetValue().Upper()) && !m_filter_map_expression->Matches(TowxString(battle.GetHostMapName())))
		return false;

	//Mod:
	if (!TowxString(battle.GetHostGameNameAndVersion()).Upper().Contains(m_filter_mod_edit->GetValue().Upper()) && !TowxString(battle.GetHostGameNameAndVersion()).Upper().Contains(m_filter_mod_edit->GetValue().Upper()) && !m_filter_mod_expression->Matches(TowxString(battle.GetHostGameNameAndVersion())))
		return false;

	if ((!m_filter_filesize_edit->GetValue().IsEmpty()) && !_IntCompare(playback.size, 1024 * FromwxString(m_filter_filesize_edit->GetValue()), m_filter_filesize_mode))
		return false;

	//duration
	if ((!m_filter_duration_edit->GetValue().IsEmpty()) && !_IntCompare(playback.duration, m_duration_value, m_filter_duration_mode))
		return false;

	return true;
}

void PlaybackListFilter::OnChange(wxCommandEvent& /*unused*/)
{
	if (!m_activ)
		return;
	m_parent_tab->UpdateList();
}

void PlaybackListFilter::OnChangeMap(wxCommandEvent& event)
{
	if (m_filter_map_edit == NULL)
		return;
	delete m_filter_map_expression;
	m_filter_map_expression = new wxRegEx(m_filter_map_edit->GetValue(), wxRE_ICASE);
	OnChange(event);
}

void PlaybackListFilter::OnChangeMod(wxCommandEvent& event)
{
	if (m_filter_mod_edit == NULL)
		return;
	delete m_filter_mod_expression;
	m_filter_mod_expression = new wxRegEx(m_filter_mod_edit->GetValue(), wxRE_ICASE);
	OnChange(event);
}

void PlaybackListFilter::OnPlayerChange(wxCommandEvent& event)
{
	m_filter_player_choice_value = m_filter_player_choice->GetSelection() - 1;
	OnChange(event);
}

void PlaybackListFilter::OnChangeDuration(wxCommandEvent& event)
{
	SetDurationValue();
	OnChange(event);
}

void PlaybackListFilter::OnChangeFilesize(wxCommandEvent& event)
{
	OnChange(event);
}

void PlaybackListFilter::OnDurationButton(wxCommandEvent& event)
{
	m_filter_duration_mode = _GetNextMode(m_filter_duration_mode);
	m_filter_duration_button->SetLabel(_GetButtonSign(m_filter_duration_mode));
	OnChange(event);
}

void PlaybackListFilter::OnFilesizeButton(wxCommandEvent& event)
{
	m_filter_filesize_mode = _GetNextMode(m_filter_filesize_mode);
	m_filter_filesize_button->SetLabel(_GetButtonSign(m_filter_filesize_mode));
	OnChange(event);
}

bool PlaybackListFilter::GetActiv() const
{
	return m_activ;
}

void PlaybackListFilter::SaveFilterValues()
{
	PlaybackListFilterValues filtervalues;
	filtervalues.duration = m_filter_duration_edit->GetValue();
	filtervalues.map = m_filter_map_edit->GetValue();
	filtervalues.map_show = m_filter_map_show->GetValue();
	filtervalues.map = m_filter_map_edit->GetValue();
	filtervalues.mod = m_filter_mod_edit->GetValue();
	filtervalues.mod_show = m_filter_mod_show->GetValue();
	filtervalues.player_mode = _GetButtonSign(m_filter_player_mode);
	filtervalues.player_num = wxString::Format(_T("%d"), m_filter_player_choice->GetSelection());
	filtervalues.duration_mode = _GetButtonSign(m_filter_duration_mode);
	filtervalues.filesize = m_filter_filesize_edit->GetValue();
	filtervalues.filesize_mode = _GetButtonSign(m_filter_filesize_mode);
	sett().SetReplayFilterValues(filtervalues);
}

void PlaybackListFilter::SetDurationValue()
{

	wxString dur = m_filter_duration_edit->GetValue();
	const wxChar* mysep = _T(":");
	int sep_count = dur.Replace(mysep, mysep); //i know, i know
	switch (sep_count) {
		default:
			break;

		case 0:
			m_duration_value = FromwxString(dur);
			break;
		case 1:
			m_duration_value = FromwxString(dur.AfterFirst(*mysep)) + (FromwxString(dur.BeforeFirst(*mysep)) * 60);
			break;
		case 2:
			m_duration_value = FromwxString(dur.AfterLast(*mysep)) + (FromwxString(dur.AfterFirst(*mysep).BeforeFirst(*mysep)) * 60) + (FromwxString(dur.BeforeFirst(*mysep)) * 3600);
			break;
	}
}
