/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_ACTIVITYNOTICE_H
#define SPRINGLOBBY_HEADERGUARD_ACTIVITYNOTICE_H

#include <memory>

#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/dialog.h>

#ifdef __WXMSW__
#include <wx/gauge.h> //cannot forward this cause msw mixes up the types otherwise
#else
class wxGauge;
#endif

#include "utils/mixins.h"

class wxStaticText;

class ActivityNoticePanel : public wxPanel, public SL::NonCopyable
{
public:
	ActivityNoticePanel(wxWindow* parent, const wxString& notice, const wxSize& panel_size = wxSize(190, 6), const wxSize& gauge_size = wxSize(80, 5));
	virtual ~ActivityNoticePanel()
	{
	}

	void SetString(const wxString& file);
	virtual bool Show(bool show = true);

private:
	wxString m_notice;
	wxGauge* m_gauge;
	wxStaticText* m_message;
	wxTimer m_timer;

	void OnTimer(wxTimerEvent& event);

	DECLARE_EVENT_TABLE()
};

class ActivityNotice : public wxDialog
{
public:
	ActivityNotice(wxWindow* parent, const wxString& notice, const wxSize& gauge_size = wxSize(80, 5));
	virtual ~ActivityNotice()
	{
	}

private:
	ActivityNoticePanel* m_panel;
};

//! use this for a notice that closes automatically when going out of scope
class ActivityNoticeContainer
{
public:
	ActivityNoticeContainer(wxWindow* parent, const wxString& notice, const wxSize& gauge_size = wxSize(80, 5));
	~ActivityNoticeContainer();

private:
	ActivityNotice* m_window;
};

typedef std::unique_ptr<ActivityNoticeContainer>
    ScopedActivityNotice;

ScopedActivityNotice scopedActivityNotice(wxWindow* parent, const wxString& notice, const wxSize& gauge_size = wxSize(80, 5));

#endif // SPRINGLOBBY_HEADERGUARD_ACTIVITYNOTICE_H
