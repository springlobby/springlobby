#ifndef SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_ADDBOTDIALOG_H

#include <wx/dialog.h>
#include "gui/windowattributespickle.h"
#include <map>

class wxTextCtrl;
class wxStaticText;
class wxChoice;
class wxStaticLine;
class wxButton;
class wxCommandEvent;
class IBattle;
class wxListCtrl;
class wxBoxSizer;
class wxListEvent;

/** \brief used in SP/MP BattletAB to present choice of AIs to add
 * \todo DOCMEMORE */
class AddBotDialog : public wxDialog, public WindowAttributesPickle
{
  public:

    AddBotDialog( wxWindow* parent, IBattle& battle, bool singleplayer = false );
    ~AddBotDialog( );

    wxString GetNick();
    wxString GetAIShortName();
    wxString GetAiRawName();
    wxString GetAIVersion();
    int GetAIType();

    void ReloadAIList();
    void ShowAIInfo();

    void OnClose( wxCommandEvent& event );
    void OnAddBot( wxCommandEvent& event );
    void OnSelectBot( wxCommandEvent& event );

    void OnOptionActivate( wxListEvent& event );
    void UpdateOption( const wxString& Tag );
    long AddMMOptionsToList( long pos, int optFlag );
    void ShowAIOptions();


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
    wxListCtrl* m_opts_list;

    typedef std::map<wxString, long> OptionListMap;
		OptionListMap m_opt_list_map;

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
      ADDBOT_AI,
      ADDBOT_OPTIONLIST,
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

