/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "channelchooserdialog.h"

#include <wx/sizer.h>

#include "channelchooser.h"
#include "settings.h"

ChannelChooserDialog::ChannelChooserDialog(wxWindow* parent, wxWindowID id,
					   const wxString& title, long style)
    : wxDialog(parent, id, title, wxDefaultPosition, wxDefaultSize, style)
    , WindowAttributesPickle(_T("CHANNELCHOOSER"), this, wxSize(470, 400))
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	m_chooser_panel = new ChannelChooserPanel(this, -1, _("Choose channels to join"));
	m_main_sizer->Add(m_chooser_panel, 1, wxALL | wxEXPAND, 5);

	SetSizer(m_main_sizer);
	Layout();
}

ChannelChooserDialog::~ChannelChooserDialog()
{
}

void ChannelChooserDialog::AddChannel(const wxString& name, int usercount, const wxString& topic)
{
	m_chooser_panel->AddChannel(name, usercount, topic);
	m_main_sizer->FitInside(this);
	Layout();
}

void ChannelChooserDialog::ClearChannels()
{
	m_chooser_panel->ClearChannels();
}
