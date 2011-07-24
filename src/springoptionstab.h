#ifndef SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H

#include <wx/scrolwin.h>

class wxStaticBoxSizer;
class wxStaticBox;
class wxStaticText;
class wxRadioButton;
class wxButton;
class wxTextCtrl;
class wxBoxSizer;
class Ui;
class wxCheckBox;

class SpringOptionsTab : public wxScrolledWindow
{
  public:
    SpringOptionsTab( wxWindow* parent );
    ~SpringOptionsTab();

    void DoRestore();

    void OnBrowseExec( wxCommandEvent& event );
    void OnBrowseSync( wxCommandEvent& event );

    void OnApply( wxCommandEvent& event );
    void OnRestore( wxCommandEvent& event );

    void OnAutoConf( wxCommandEvent& event );
    void OnFindExec( wxCommandEvent& event );
    void OnFindSync( wxCommandEvent& event );

    void OnDataDir( wxCommandEvent& event );

    void OnDontSearch( wxCommandEvent& event );

  protected:

		void SetupUserFolders();

    wxStaticText* m_exec_loc_text;
    wxStaticText* m_sync_loc_text;

    wxButton* m_exec_browse_btn;
    wxButton* m_exec_find_btn;
    wxButton* m_sync_browse_btn;
    wxButton* m_sync_find_btn;
    wxButton* m_datadir_btn;

    wxButton* m_auto_btn;

    wxRadioButton* m_exec_def_radio;
    wxRadioButton* m_exec_spec_radio;
    wxRadioButton* m_sync_def_radio;
    wxRadioButton* m_sync_spec_radio;

    wxTextCtrl* m_exec_edit;
    wxTextCtrl* m_sync_edit;

    wxStaticBox* m_exec_box;
    wxStaticBox* m_sync_box;
    wxStaticBox* m_web_box;
    wxStaticBoxSizer* m_exec_box_sizer;
    wxStaticBoxSizer* m_sync_box_sizer;


    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_aconf_sizer;
    wxBoxSizer* m_exec_loc_sizer;
    wxBoxSizer* m_sync_loc_sizer;

    wxCheckBox* m_dontsearch_chkbox;
    wxCheckBox* m_oldlaunch_chkbox;

    enum {
        SPRING_EXECBROWSE = wxID_HIGHEST,
        SPRING_SYNCBROWSE,
        SPRING_WEBBROWSE,
        SPRING_DEFEXE,
        SPRING_DEFUSYNC,
        SPRING_DEFWEB,
        SPRING_AUTOCONF,
        SPRING_EXECFIND,
        SPRING_SYNCFIND,
        SPRING_DATADIR,
        SPRING_DONTSEARCH
    };

    DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_SPRINGOPTIONSTAB_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

