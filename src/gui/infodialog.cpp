/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "infodialog.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/version.h>
#include <utility>

#include <lslutils/misc.h>

#include "settings.h"
#include "sysinfo.h"
#include "utils/conversion.h"
#include "utils/platform.h"

InfoDialog::InfoDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, _("Paths"), wxDefaultPosition, wxSize(620, 400), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxCLOSE_BOX)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);


	wxTextCtrl* out = new wxTextCtrl(this, wxNewId(), wxEmptyString, wxDefaultPosition, wxDefaultSize,
					 wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_AUTO_URL);
	out->SetValue(TowxString(GetSpringlobbyInfo()));
	m_main_sizer->Add(out, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(m_main_sizer);
	Layout();
}

InfoDialog::~InfoDialog()
{
}
