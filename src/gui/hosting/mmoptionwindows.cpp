/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "mmoptionwindows.h"

#include <lslunitsync/optionswrapper.h>
#include <lslunitsync/unitsync.h>
#include <lslutils/misc.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/log.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/tipwin.h>
#include <wx/tooltip.h>

#include "gui/controls.h"
#include "gui/ui.h"
#include "ibattle.h"
#include "utils/conversion.h"
#include "utils/lslconversion.h"

SingleOptionDialog::SingleOptionDialog(IBattle& battle, const wxString& optiontag)
    : m_battle(battle)
    , m_tag(optiontag)
    , m_checkbox(NULL)
    , m_combobox(NULL)
    , m_spinctrl(NULL)
    , m_textctrl(NULL)
    , m_cancel_button(NULL)
    , m_ok_button(NULL)
{
	LSL::OptionsWrapper& optWrap = m_battle.CustomBattleOptions();
	LSL::Enum::GameOption optFlag = (LSL::Enum::GameOption)FromwxString(optiontag.BeforeFirst('_'));
	const auto key = STD_STRING(optiontag.AfterFirst('_'));
	LSL::Enum::OptionType type = optWrap.GetSingleOptionType(key);
	Create((wxWindow*)&ui().mw(), wxID_ANY, _("Change option"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T( "OptionDialog" ));
	if (!optWrap.keyExists(key, optFlag, false, type)) {
		EndModal(wxID_CANCEL);
		return;
	}

	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);

	// wxStaticText* m_labelctrl = wxStaticText();

	switch (type) {
		case LSL::Enum::opt_bool: {
			const auto opt = optWrap.m_opts[optFlag].bool_map[key];
			m_checkbox = new wxCheckBox(this, wxID_ANY, TowxString(opt.name));
			m_checkbox->SetToolTip(opt.description);
			m_checkbox->SetValue(opt.value);
			m_main_sizer->Add(m_checkbox, 0, wxEXPAND);
			break;
		}
		case LSL::Enum::opt_float: {
			const auto opt = optWrap.m_opts[optFlag].float_map[key];
			m_spinctrl = new wxSpinCtrlDouble(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
							  wxSP_ARROW_KEYS, double(opt.min), double(opt.max),
							  double(opt.value), double(opt.stepping), TowxString(opt.key));
			m_spinctrl->SetToolTip(opt.description);
			m_main_sizer->Add(m_spinctrl, 0, wxEXPAND);
			break;
		}
		case LSL::Enum::opt_string: {
			const auto opt = optWrap.m_opts[optFlag].string_map[key];
			m_textctrl = new wxTextCtrl(this, wxID_ANY, TowxString(opt.value), wxDefaultPosition,
						    wxDefaultSize, 0, wxDefaultValidator, TowxString(opt.key));
			m_textctrl->SetToolTip(opt.description);
			m_main_sizer->Add(m_textctrl, 0, wxEXPAND);
			break;
		}
		case LSL::Enum::opt_list: {
			const auto opt = optWrap.m_opts[optFlag].list_map[key];
			const int temp = int(opt.cbx_choices.size() - 1);
			const int index = LSL::Util::Clamp(opt.cur_choice_index, 0, temp);
			m_combobox = new wxComboBox(this, wxID_ANY, TowxString(opt.cbx_choices[index]), wxDefaultPosition, wxDefaultSize,
						    lslTowxArrayString(opt.cbx_choices), wxCB_READONLY, wxDefaultValidator);
			std::string tooltip = opt.description + "\n";
			for (const auto itor : opt.listitems) {
				tooltip += "\n" + itor.name + ": " + itor.desc;
			}
			m_combobox->SetToolTip(tooltip);
			m_main_sizer->Add(m_combobox, 0, wxEXPAND);
			break;
		}
		default: {
			EndModal(wxID_CANCEL);
			return;
		}
	}

	wxSize __SpacerSize_1 = wxDLG_UNIT(this, wxSize(0, 0));
	m_main_sizer->Add(__SpacerSize_1.GetWidth(), __SpacerSize_1.GetHeight(), 0, wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	wxStaticLine* m_separator1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(10, -1), wxLI_HORIZONTAL, _T( "ID_STATICLINE1" ));
	m_main_sizer->Add(m_separator1, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	wxBoxSizer* m_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	m_cancel_button = new wxButton(this, ID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T( "ID_CANCEL" ));
	m_buttons_sizer->Add(m_cancel_button, 0, wxALL | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 5);
	m_buttons_sizer->Add(0, 0, 1, wxALL | wxEXPAND | wxSHAPED | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	m_ok_button = new wxButton(this, ID_OK, _("Ok"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T( "ID_OK" ));
	m_buttons_sizer->Add(m_ok_button, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 5);
	m_main_sizer->Add(m_buttons_sizer, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);


	m_main_sizer->Fit(this);
	m_main_sizer->SetSizeHints(this);

	SetSizer(m_main_sizer);
	Layout();

	Connect(ID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SingleOptionDialog::OnCancel);
	Connect(ID_OK, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&SingleOptionDialog::OnOk);
}

void SingleOptionDialog::OnCancel(wxCommandEvent& /*unused*/)
{
	EndModal(wxID_CANCEL);
}

void SingleOptionDialog::OnOk(wxCommandEvent& /*unused*/)
{
	const auto optFlag = (LSL::Enum::GameOption)FromwxString(m_tag.BeforeFirst('_'));
	const std::string key = STD_STRING(m_tag.AfterFirst('_'));
	wxString value;
	if (m_textctrl)
		value = m_textctrl->GetValue();
	else if (m_combobox)
		value = TowxString(m_battle.CustomBattleOptions()
				       .GetNameListOptItemKey(key, STD_STRING(m_combobox->GetValue()), optFlag));
	else if (m_spinctrl) {
		double d = m_spinctrl->GetValue();
		value = wxString::Format(_T("%f"), d);
		wxLogMessage(_T("Got VALUE: %s -- %f"), value.c_str(), d);
	} else if (m_checkbox)
		value = TowxString(m_checkbox->GetValue());
	m_battle.CustomBattleOptions().setSingleOption(key, STD_STRING(value), optFlag);
	m_battle.SendHostInfo(STD_STRING(m_tag));
	EndModal(wxID_OK);
}
