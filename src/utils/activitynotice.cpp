/* Copyright (C) 2007-2009 The SpringLobby Team. All rights reserved. */

#include "activitynotice.h"

#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/app.h>

BEGIN_EVENT_TABLE(ActivityNoticePanel,wxPanel)
    	EVT_TIMER(wxID_ANY, ActivityNoticePanel::OnTimer)
END_EVENT_TABLE()


 ActivityNotice::ActivityNotice(wxWindow* parent,const wxString& file,const wxString& format )
    :wxDialog ( parent,wxID_ANY,_T(""),wxDefaultPosition, wxSize(190,60),wxBORDER_NONE|wxSTAY_ON_TOP)
{
    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_panel = new ActivityNoticePanel(this,file,format);
    m_main_sizer->Add( m_panel, 1 , wxALL|wxEXPAND, 0 );
    SetSizer(m_main_sizer);
    Layout();
}

 ActivityNoticePanel::ActivityNoticePanel(wxWindow* parent,const wxString& file,const wxString& format )
    :wxPanel ( parent,wxID_ANY,wxDefaultPosition, wxSize(190,60),wxBORDER_NONE),
    m_filename(file),m_format(format)
{
    static int timer_inc = 0;
    m_timer.SetOwner(this,timer_inc++);
    wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );
    m_gauge = new wxGauge(this, wxID_ANY,120,wxDefaultPosition,wxSize(80,5) );
    m_gauge->Pulse();
    m_message = new wxStaticText(this,wxID_ANY,_T("") );
    m_main_sizer->Add( m_message, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
    m_main_sizer->Add( m_gauge, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
    SetSizer( m_main_sizer );
    SetString( m_filename );
    Show(true);
}

void ActivityNoticePanel::SetString(const wxString& file)
{
    m_filename = file;
    m_message->SetLabel(wxString::Format( m_format, m_filename.c_str() ) );
    Layout();
}

bool ActivityNoticePanel::Show(bool show )
{
    if ( show )
        m_timer.Start( 80, wxTIMER_CONTINUOUS );
    else
        m_timer.Stop ();
    bool ret = wxPanel::Show(show);
    Layout();
    return ret;
}

void ActivityNoticePanel::OnTimer(wxTimerEvent& event)
{
    m_gauge->Pulse();
}

 ActivityNoticeContainer::ActivityNoticeContainer(wxWindow* parent_,const wxString& file,const wxString& format )
{
    wxWindow* parent = ( parent_ ) ? parent_ : ( wxTheApp->GetTopWindow() ) ;
    m_window = new ActivityNotice( parent, file, format);
    m_window->Show(true);
}

 ActivityNoticeContainer::~ActivityNoticeContainer()
{
    m_window->Destroy();
}

ScopedActivityNotice scopedActivityNotice(wxWindow* parent,const wxString& file, const wxString& format )
{
    return ScopedActivityNotice( new ActivityNoticeContainer( parent, file, format ) );
}
