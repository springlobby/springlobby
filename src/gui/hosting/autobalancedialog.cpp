/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//(*InternalHeaders(AutoBalanceDialog)
#include "autobalancedialog.h"

#include <wx/button.h>
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>

#include "gui/controls.h"
#include "settings.h"
#include "utils/conversion.h"
//*)

const long AutoBalanceDialog::ID_STATICTEXT1 = wxNewId();
const long AutoBalanceDialog::ID_METHOD = wxNewId();
const long AutoBalanceDialog::ID_STATICTEXT2 = wxNewId();
const long AutoBalanceDialog::ID_STATICTEXT3 = wxNewId();
const long AutoBalanceDialog::ID_CHOICE1 = wxNewId();
const long AutoBalanceDialog::ID_CHOICE2 = wxNewId();
const long AutoBalanceDialog::ID_STATICLINE1 = wxNewId();
const long AutoBalanceDialog::ID_CANCEL = wxNewId();
const long AutoBalanceDialog::ID_OK = wxNewId();

BEGIN_EVENT_TABLE(AutoBalanceDialog, wxDialog)
END_EVENT_TABLE()


AutoBalanceDialog::AutoBalanceDialog(wxWindow* parent, const BalanceOptions& defaultval, unsigned int maxnum)
{
	wxFlexGridSizer* m_choices_sizer;
	wxBoxSizer* m_buttons_sizer;
	wxBoxSizer* m_main_sizer;

	Create(parent, wxID_ANY, _("Autobalance players into teams"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_choices_sizer = new wxFlexGridSizer(0, 2, wxDLG_UNIT(this, wxSize(4, 0)).GetWidth(), wxDLG_UNIT(this, wxSize(10, 0)).GetWidth());
	m_method_label = new wxStaticText(this, ID_STATICTEXT1, _("Method"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	m_choices_sizer->Add(m_method_label, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 0);
	m_method_choice = new wxChoice(this, ID_METHOD, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_METHOD"));
	m_method_choice->SetSelection(m_method_choice->Append(_("Divide ranks evenly")));
	m_method_choice->Append(_("Random"));
	m_choices_sizer->Add(m_method_choice, 0, wxALL | wxEXPAND | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT2, _("Clans"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	m_choices_sizer->Add(StaticText1, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 0);
	m_clans_choice = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	m_clans_choice->SetSelection(m_clans_choice->Append(_("None")));
	m_clans_choice->Append(_("Fair"));
	m_clans_choice->Append(_("Always"));
	m_clans_choice->SetToolTip(_("Put members of same clan ( users having same clantag, like \'[smurfzor]Alice\' and \'[smurfzor]Bob\' ) together into same alliance. \nNone: nothing special for clans.\nFair: put clanmembers into alliance, unless this makes alliances unfair.\nAlways: always put clanmembers into alliance, even if that alliance is unfair."));
	m_choices_sizer->Add(m_clans_choice, 0, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 0);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT3, _("Number of allies"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	m_choices_sizer->Add(StaticText2, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 0);
	m_group_choice = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	m_group_choice->Append(_("Auto select"));
	for (unsigned int i = 1; i < (maxnum + 1); i++)
		m_group_choice->Append(TowxString(i));
	m_choices_sizer->Add(m_group_choice, 0, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 0);
	m_main_sizer->Add(m_choices_sizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	wxSize __SpacerSize_1 = wxDLG_UNIT(this, wxSize(0, 0));
	m_main_sizer->Add(__SpacerSize_1.GetWidth(), __SpacerSize_1.GetHeight(), 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	m_separator1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10, -1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	m_main_sizer->Add(m_separator1, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_cancel_button = new wxButton(this, ID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CANCEL"));
	m_buttons_sizer->Add(m_cancel_button, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	m_buttons_sizer->Add(0, 0, 1, wxALL | wxEXPAND | wxSHAPED | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	m_ok_button = new wxButton(this, ID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_OK"));
	m_buttons_sizer->Add(m_ok_button, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);
	m_main_sizer->Add(m_buttons_sizer, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(m_main_sizer);
	m_main_sizer->Fit(this);
	m_main_sizer->SetSizeHints(this);

	Connect(ID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AutoBalanceDialog::OnCancel);
	Connect(ID_OK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&AutoBalanceDialog::OnOk);

	m_method_choice->SetSelection(defaultval.type);
	m_clans_choice->SetSelection(defaultval.respectclans + (defaultval.respectclans && defaultval.strongclans));
	m_group_choice->SetSelection(defaultval.groupingsize);
}

AutoBalanceDialog::~AutoBalanceDialog()
{
}


void AutoBalanceDialog::OnCancel(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_CANCEL);
}

void AutoBalanceDialog::OnOk(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_OK);
}

AutoBalanceDialog::BalanceOptions AutoBalanceDialog::GetResult()
{
	BalanceOptions ret;
	ret.type = (IBattle::BalanceType)m_method_choice->GetSelection();
	ret.respectclans = (m_clans_choice->GetSelection() > 0);
	ret.strongclans = (m_clans_choice->GetSelection() > 1);
	ret.groupingsize = m_group_choice->GetSelection();
	return ret;
}
