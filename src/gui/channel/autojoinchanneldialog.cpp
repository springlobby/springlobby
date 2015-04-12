/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "autojoinchanneldialog.h"


#include <wx/intl.h>
#include <wx/datetime.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/tokenzr.h>
#include <wx/event.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include "settings.h"

BEGIN_EVENT_TABLE(AutojoinChannelDialog, wxDialog)

EVT_BUTTON(wxID_OK, AutojoinChannelDialog::OnOk)
EVT_BUTTON(wxID_CANCEL, AutojoinChannelDialog::OnCancel)


END_EVENT_TABLE()

AutojoinChannelDialog::AutojoinChannelDialog(wxWindow* parent)
    : wxDialog(parent, -1, _("Edit auto-joined channels"), wxDefaultPosition, wxSize(400, 500),
	       wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* textSizer = new wxBoxSizer(wxVERTICAL);
	m_channel_list = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
					wxTE_MULTILINE | wxTE_DONTWRAP);
	wxSizer* buttonSizer = CreateButtonSizer(wxOK | wxCANCEL | wxALIGN_CENTRE);
	wxStaticLine* seperator = new wxStaticLine(this);
	wxStaticText* hint = new wxStaticText(this, -1, _("Add one channel per line like this:\n"
							  "channelname password\n(passwords for existing channels are not displayed)"));
	textSizer->Add(hint, 0, wxEXPAND | wxALL | wxALIGN_CENTRE, 10);
	textSizer->Add(seperator);
	textSizer->Add(m_channel_list, 1, wxEXPAND | wxALL | wxALIGN_CENTRE, 10);
	mainSizer->Add(textSizer, 1, wxEXPAND | wxALL | wxALIGN_CENTRE, 10);
	mainSizer->Add(seperator);
	mainSizer->Add(buttonSizer, 0, wxALL | wxALIGN_CENTRE, 10);
	SetSizer(mainSizer);
	wxString channels;
	std::vector<ChannelJoinInfo> chanlist = sett().GetChannelsJoin();
	for (size_t i = 0; i < chanlist.size(); i++) {
		ChannelJoinInfo info = chanlist[i];
		channels += info.name;
		if (!info.password.empty()) {
			channels += _T(" ") + info.password;
		}
		channels += _T("\n");
	}
	m_channel_list->SetValue(channels);
}

AutojoinChannelDialog::~AutojoinChannelDialog()
{
	//dtor
}


void AutojoinChannelDialog::OnOk(wxCommandEvent& /*unused*/)
{
	wxString newChannels = m_channel_list->GetValue();

	sett().RemoveAllChannelsJoin();

	//add new channels
	wxStringTokenizer tokenList(newChannels, _T( "\n" ));
	while (tokenList.HasMoreTokens()) {
		wxString line = tokenList.GetNextToken();
		wxString chan;
		if (line.Find(_T(" ")) != wxNOT_FOUND)
			chan = line.BeforeFirst(_T(' '));
		else
			chan = line;
		wxString key = line.AfterFirst(_T(' '));
		sett().AddChannelJoin(chan, key);
	}
	sett().SaveSettings();
	this->Show(false);
}

void AutojoinChannelDialog::OnCancel(wxCommandEvent& /*unused*/)
{
	this->Show(false);
}
