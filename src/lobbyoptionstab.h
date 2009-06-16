#ifndef LOBBYOPTIONSTAB_H
#define LOBBYOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxCheckBox;
class wxBoxSizer;
class wxStaticText;
class wxRadioButton;
class wxTextCtrl;
class wxButton;
class wxStaticBoxSizer;
class wxChoice;

class LobbyOptionsTab : public wxScrolledWindow
{
    public:
        LobbyOptionsTab(wxWindow* parent);
        virtual ~LobbyOptionsTab();

        void OnApply( wxCommandEvent& event );
        void OnRestore( wxCommandEvent& event );

        void OnBrowseWeb( wxCommandEvent& event );
        void OnDefaultWeb( wxCommandEvent& event );
        void HandleWebloc( bool defloc );

    protected:

        wxCheckBox* m_autojoin;
        wxCheckBox* m_reportstats;
        wxCheckBox* m_updater;
        wxCheckBox* m_show_tooltips;
        wxCheckBox* m_use_tabicons;

        wxStaticText* m_autoconnect_label;
        wxStaticText* m_reportstats_label;
        wxStaticText* m_updater_label;
        wxStaticText* m_show_tooltips_label;
        wxStaticText* m_web_loc_text;
        wxTextCtrl* m_web_edit;

        wxStaticText* m_complete_method_label;
        wxRadioButton* m_complete_method_old;
        wxRadioButton* m_complete_method_new;
        wxButton* m_web_browse_btn;
        wxButton* m_select_locale;
        wxRadioButton* m_web_def_radio;
        wxRadioButton* m_web_spec_radio;


        wxBoxSizer* m_main_sizer;
        wxStaticBoxSizer* m_web_box_sizer;
        wxBoxSizer* m_web_loc_sizer;

        wxChoice* m_start_tab;

    enum
    {
        SPRING_WEBBROWSE = wxID_HIGHEST,
        SPRING_DEFWEB
    };


        DECLARE_EVENT_TABLE();
};

#endif // LOBBYOPTIONSTAB_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

