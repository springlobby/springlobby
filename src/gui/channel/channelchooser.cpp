/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "channelchooser.h"

#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "channel.h"
#include "channellistview.h"
#include "iserver.h"

BEGIN_EVENT_TABLE(ChannelChooserPanel, wxScrolledWindow)
EVT_TEXT(ID_SEARCH_TEXT, ChannelChooserPanel::OnSearch)
END_EVENT_TABLE()

ChannelChooserPanel::ChannelChooserPanel(wxWindow* parent, wxWindowID id, const wxString& /*unused*/,
					 const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxScrolledWindow(parent, id, pos, size, style, name)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* join_label = new wxStaticText(this, -1, _("Double click to join"));
	m_main_sizer->Add(join_label, 0, wxALL | wxEXPAND, 5);

	m_channellist = new ChannelListView(_T("ChannelChooser_ChannelListView"), this);
	m_main_sizer->Add(m_channellist, 1, wxALL | wxEXPAND, 5);

	wxBoxSizer* search_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* search_label = new wxStaticText(this, -1, _("Find channel:"));
	m_search_text = new wxTextCtrl(this, ID_SEARCH_TEXT);
	search_sizer->Add(search_label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
	search_sizer->Add(m_search_text, 1, wxLEFT | wxEXPAND, 5);
	m_main_sizer->Add(search_sizer, 0, wxALL | wxEXPAND, 5);

	m_info_label = new wxStaticText(this, -1, m_channellist->GetInfo());
	m_main_sizer->Add(m_info_label, 0, wxALL, 5);

	SetSizer(m_main_sizer);
}

ChannelChooserPanel::~ChannelChooserPanel()
{
	wxDELETE(m_channellist);
	//dtor
}

void ChannelChooserPanel::AddChannel(const wxString& name, int usercount, const wxString& topic)
{
	m_channellist->AddChannel(name, usercount, topic);
	m_info_label->SetLabel(m_channellist->GetInfo());
	Layout();
}

void ChannelChooserPanel::ClearChannels()
{
	m_channellist->ClearChannels();
	m_info_label->SetLabel(m_channellist->GetInfo());
	m_search_text->SetValue(wxEmptyString);
}

void ChannelChooserPanel::OnSearch(wxCommandEvent& /*unused*/)
{
	m_channellist->FilterChannel(m_search_text->GetValue());
	m_info_label->SetLabel(m_channellist->GetInfo());
}
