/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "activitynotice.h"

#include <wx/app.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#ifndef __WXMSW__
#include <wx/gauge.h> //cannot forward this cause msw mixes up the types otherwise
#endif

BEGIN_EVENT_TABLE(ActivityNoticePanel, wxPanel)
EVT_TIMER(wxID_ANY, ActivityNoticePanel::OnTimer)
END_EVENT_TABLE()


ActivityNotice::ActivityNotice(wxWindow* parent, const wxString& notice, const wxSize& gauge_size)
    : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(190, 60), wxBORDER_NONE | wxSTAY_ON_TOP)
{
	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_panel = new ActivityNoticePanel(this, notice, gauge_size);
	m_main_sizer->Add(m_panel, 1, wxALL | wxEXPAND, 0);
	SetSizer(m_main_sizer);
	Layout();
}

ActivityNoticePanel::ActivityNoticePanel(wxWindow* parent, const wxString& notice, const wxSize& panel_size, const wxSize& gauge_size)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, panel_size, wxBORDER_NONE)
    , m_notice(notice)
{
	static int timer_inc = 0;
	m_timer.SetOwner(this, timer_inc++);
	wxBoxSizer* m_main_sizer = new wxBoxSizer(wxVERTICAL);
	m_message = new wxStaticText(this, wxID_ANY, wxEmptyString);
	m_main_sizer->Add(m_message, 1, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL, 5);

	m_gauge = new wxGauge(this, wxID_ANY, 120, wxDefaultPosition, gauge_size);
	m_gauge->Pulse();
	m_main_sizer->Add(m_gauge, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

	SetSizer(m_main_sizer);
	SetString(m_notice);
	Show(true);
}

void ActivityNoticePanel::SetString(const wxString& file)
{
	m_notice = file;
	m_message->SetLabel(m_notice);
	Layout();
}

bool ActivityNoticePanel::Show(bool show)
{
	if (show)
		m_timer.Start(80, wxTIMER_CONTINUOUS);
	else
		m_timer.Stop();
	bool ret = wxPanel::Show(show);
	Layout();
	return ret;
}

void ActivityNoticePanel::OnTimer(wxTimerEvent&)
{
	m_gauge->Pulse();
}

ActivityNoticeContainer::ActivityNoticeContainer(wxWindow* parent_, const wxString& notice, const wxSize& gauge_size)
{
	wxWindow* parent = (parent_) ? parent_ : (wxTheApp->GetTopWindow());
	m_window = new ActivityNotice(parent, notice, gauge_size);
	m_window->Show(true);
}

ActivityNoticeContainer::~ActivityNoticeContainer()
{
	m_window->Destroy();
}

ScopedActivityNotice scopedActivityNotice(wxWindow* parent, const wxString& notice, const wxSize& gauge_size)
{
	return ScopedActivityNotice(new ActivityNoticeContainer(parent, notice, gauge_size));
}
