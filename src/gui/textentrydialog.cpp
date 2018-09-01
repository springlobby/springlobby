/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "textentrydialog.h"

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

TextEntryDialog::TextEntryDialog(wxWindow* parent, const wxString& title, const wxString& heading, const wxString& answer, bool multiline)
    : wxDialog(parent, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxStaticText* text = new wxStaticText(this, -1, heading);
	long inputstyle = 0;
	if (multiline)
		inputstyle = wxTE_MULTILINE;
	input = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, inputstyle);
	if (multiline)
		input->SetInitialSize(wxSize(400, 300));
	SetValue(answer);
	wxSizer* buttons = CreateButtonSizer(wxOK | wxCANCEL);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(text, 0, wxALL, 5);
	sizer->Add(input, 1, wxEXPAND | wxALL, 5);
	sizer->Add(buttons, 0, wxEXPAND | wxALL, 5);
	SetSizerAndFit(sizer);
	input->SetFocus();
}

TextEntryDialog::~TextEntryDialog()
{
}

void TextEntryDialog::SetValue(const wxString& value)
{
	input->SetValue(value);
}

const wxString TextEntryDialog::GetValue()
{
	return input->GetValue();
}
