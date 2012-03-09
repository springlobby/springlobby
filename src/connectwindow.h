#ifndef SPRINGLOBBY_HEADERGUARD_CONNECTWINDOW_H
#define SPRINGLOBBY_HEADERGUARD_CONNECTWINDOW_H

#include <wx/dialog.h>

#include "utils/isink.h"

class wxPanel;
class wxComboBox;
class wxBoxSizer;
class wxStdDialogButtonSizer;
class wxButton;
class wxTextCtrl;
class wxStaticText;
class wxNotebook;
class wxCheckBox;
class wxStaticLine;
class Ui;

//! @brief wxFrame with a connection dialog used to specify username, password, and server. It can also register a new acount.
class ConnectWindow : public wxDialog, public OnQuitSink<ConnectWindow>
{
  public:
    ConnectWindow( wxWindow* parent, Ui& ui );
    ~ConnectWindow();

    // ConnectWindow interface

    void ReloadServerList();

    void OnServerChange( wxCommandEvent& event );

    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnQuit( GlobalEvents::GlobalEventData data );

  protected:
    // ConnectWindow variables

    wxNotebook* m_tabs;             //!< Notebook containing the login and register tabs
    wxPanel* m_login_tab;           //!< The login tab
    wxPanel* m_register_tab;        //!< The register tab

    wxStaticText* m_server_lbl;     //!< Label saying "Server"
    wxComboBox*   m_server_combo;   //!< Combobox where user can select server

    wxStaticLine* m_ser_acc_line;   //!< Line that separates the server setting and the acount settings

    wxStaticText* m_nick_lbl;       //!< Label saying "Nickname"
    wxTextCtrl*   m_nick_text;      //!< Textbox where user can input nickname
    wxStaticText* m_pass_lbl;       //!< Label saying "Password"
    wxTextCtrl*   m_pass_text;      //!< Textbox where user can input password
    wxCheckBox*   m_rpass_check;    //!< Checkbox where user can chose if he wants his password to be saved.
    wxCheckBox*   m_autoconnect_check;    //!< Checkbox where user can chose if he wants to autoconnect.

    wxStaticLine* m_acc_note_line;  //!< Line that separates the account settings from the note

    wxStaticText* m_note_lbl;       //!< Label saying something about register

    wxButton* m_ok_btn;             //!< The ok button
    wxButton* m_cancel_btn;         //!< The cancel button

    wxBoxSizer* m_main_sizer;       //!< The main sizer connecting it all together
    wxBoxSizer* m_login_main_sizer; //!< Login tab main sizer
    wxBoxSizer* m_server_sizer;     //!< Sizer connecting server label and combo
    wxBoxSizer* m_nick_sizer;       //!< Sizer connecting nick label and input
    wxBoxSizer* m_pass_sizer;       //!< Sizer connecting password label and input
    wxBoxSizer* m_rpass_sizer;      //!< Sizer spacing the remember password setting
    wxStdDialogButtonSizer* m_buttons_sizer;    //!< Sizer connecting the ok and cancel buttons

    wxStaticText* m_regnick_lbl;
    wxTextCtrl* m_regnick_text;
    wxStaticLine* m_regpass_sep;
    wxStaticText* m_regpass1_lbl;
    wxTextCtrl* m_regpass1_text;
    wxStaticText* m_regpass2_lbl;
    wxTextCtrl* m_regpass2_text;

    Ui& m_ui;

    enum {
      CON_SERV_SEL = wxID_HIGHEST
    };

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_CONNECTWINDOW_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

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

