/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: ConnectFrame
// Created on: Mon May  7 18:29:36 2007
//

#ifndef _CONNECTWINDOW_H_
#define _CONNECTWINDOW_H_

#include <wx/wx.h>
#include <wx/checkbox.h>
#include <wx/statline.h>

//! @brief wxFrame with a connection dialog used to specify username, password, and server. It can also register a new acount.
class ConnectWindow : public wxFrame
{
  public:
    ConnectWindow( wxWindow* parent );
    virtual ~ConnectWindow();
  
    // ConnectWindow interface
  
    void OnOk(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
  
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
    wxBoxSizer* m_buttons_sizer;    //!< Sizer connecting the ok and cancel buttons
    
    DECLARE_EVENT_TABLE()
};


#endif  //_CONNECTWINDOW_H_

