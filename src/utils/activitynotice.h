#ifndef SPRINGLOBBY_HEADERGUARD_ACTIVITYNOTICE_H
#define SPRINGLOBBY_HEADERGUARD_ACTIVITYNOTICE_H

#include <memory>

#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/dialog.h>

class wxGauge;
class wxStaticText;

class ActivityNoticePanel: public wxPanel
{
    public:
        ActivityNoticePanel(wxWindow* parent,const wxString& file,const wxString& format = _("Caching file %s please wait") );
        virtual ~ActivityNoticePanel() {}

        void SetString(const wxString& file);
        virtual bool Show(bool show = true);
    protected:
        wxString m_filename;
        wxString m_format;
        wxGauge* m_gauge;
        wxStaticText* m_message;
//        wxStaticText* m_format;
        wxTimer m_timer;

        void OnTimer(wxTimerEvent& event);

        DECLARE_EVENT_TABLE()
};

class ActivityNotice: public wxDialog
{
    public:
        ActivityNotice(wxWindow* parent,const wxString& file, const wxString& format = _("Caching file %s please wait"));
        virtual ~ActivityNotice() {}
    protected:
        ActivityNoticePanel* m_panel;
};

//! use this for a notice that closes automatically when going out of scope
class ActivityNoticeContainer
{
    public:
        ActivityNoticeContainer(wxWindow* parent,const wxString& file, const wxString& format = _("Caching file %s please wait"));
        ~ActivityNoticeContainer();

    protected:
        ActivityNotice* m_window;
};

typedef std::auto_ptr<ActivityNoticeContainer>
    ScopedActivityNotice;

ScopedActivityNotice scopedActivityNotice(wxWindow* parent,const wxString& file, const wxString& format = _("Caching file %s please wait"));

#endif // SPRINGLOBBY_HEADERGUARD_ACTIVITYNOTICE_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/
