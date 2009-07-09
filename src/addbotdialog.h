#ifndef SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;
class wxStaticText;
class wxChoice;
class wxStaticLine;
class wxButton;
class wxCommandEvent;
class IBattle;
class wxListCtrl;
class wxBoxSizer;

/** \brief used in SP/MP BattletAB to present choice of AIs to add
 * \todo DOCMEMORE */
class AddBotDialog : public wxDialog
{
  public:

    AddBotDialog( wxWindow* parent, IBattle& battle, bool singleplayer = false );

    wxString GetNick();
    wxString GetAIShortName();
    wxString GetAIVersion();
    int GetAIType();

    void ReloadAIList();
    void ShowAIInfo();

    void OnClose( wxCommandEvent& event );
    void OnAddBot( wxCommandEvent& event );
    void OnSelectBot( wxCommandEvent& event );

  protected:
    AddBotDialog( const AddBotDialog& );

    wxStaticText* m_nick_lbl;
    wxTextCtrl* m_nick;
    wxStaticText* m_ai_lbl;
    wxChoice* m_ai;
    wxStaticLine* m_buttons_sep;
    wxButton* m_cancel_btn;
    wxButton* m_add_btn;
    wxListCtrl* m_ai_infos_lst;

    wxBoxSizer* m_main_sizer;
    wxBoxSizer* m_info_sizer;


    IBattle& m_battle;

    wxArrayString m_ais;

    bool m_sp;

    wxString RefineAIName( const wxString& name );

    enum
    {
      ADDBOT_ADD = wxID_HIGHEST,
      ADDBOT_CANCEL,
      ADDBOT_AI
    };

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

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

