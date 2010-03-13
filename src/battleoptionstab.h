#ifndef SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H
#define SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

#include <wx/scrolwin.h>

class Ui;
class IBattle;
class wxCommandEvent;
class wxBoxSizer;
class wxStaticText;
class wxSplitterWindow;
class wxButton;
class wxCheckBox;
class wxListBox;
class wxRadioBox;
class wxCheckListBox;

/** \brief manipulate "standard" engine options, unit restriction list
 * \todo DOCMEMORE */
class BattleOptionsTab : public wxScrolledWindow
{
  public:
    BattleOptionsTab( wxWindow* parent, IBattle* battle );
    ~BattleOptionsTab();

    void UpdateBattle( const wxString& Tag );
    void ReloadRestrictions();

    int GetAllowedUnitIndex( const wxString& name );
    int GetRestrictedUnitIndex( const wxString& name );
    bool IsRestricted( const wxString& name );
    void Restrict( const wxString& name, int count );
    void Allow( const wxString& name );
    void Restrict( int index, int count );
    void Allow( int index );


    void OnRestrict( wxCommandEvent& event );
    void OnAllow( wxCommandEvent& event );
    void OnClearRestrictions( wxCommandEvent& event );

    void SetBattle( IBattle* battle );
    IBattle* GetBattle();

  protected:

    void UpdateBattle();

    IBattle* m_battle;

    wxStaticText* m_aloowed_lbl;
    wxListBox* m_allowed_list;
    wxButton* m_restrict_btn;
    wxButton* m_allow_btn;
    wxStaticText* m_restricted_lbl;
    wxListBox* m_restrict_list;
    wxButton* m_clear_btn;

    enum {
      BOPTS_END = wxID_HIGHEST,
      BOPTS_OPTS,
      BOPTS_SLIDE,

      BOPTS_RESTRICT,
      BOPTS_ALLOW,
      BOPTS_CLEARRES,

      BOPTS_LOADPRES,
      BOPTS_SAVEPRES,
      BOPTS_DELETEPRES,
      BOPTS_SETDEFAULTPRES,
      BOPTS_CHOSEPRES
    };

    DECLARE_EVENT_TABLE()
};


enum
{
  BOPTS_RESTRICT = wxID_HIGHEST,
  BOPTS_ALLOW,
  BOPTS_CLEARRES
};


#endif // SPRINGLOBBY_HEADERGUARD_BATTLEOPTIONSTAB_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

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

