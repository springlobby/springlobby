#ifndef SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

#include "battlelistfilter.h"
#include "battlelist.h"

#include "customvirtlistctrl.h"

class wxMenu;
class IBattle;
class wxListEvent;
class wxCommandEvent;
class Ui;

/** \brief The ListCtrll contained in BattleListTab dispalying all currently active battles with their infos
 * \todo DOCMEMORE */
class BattleListCtrl : public CustomVirtListCtrl< IBattle *, BattleListCtrl>
{
  public:
    BattleListCtrl( wxWindow* parent );
    ~BattleListCtrl();

    void AddBattle( IBattle& battle );
    void RemoveBattle( IBattle& battle );
    void UpdateBattle( IBattle& battle );


    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);

    //these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemImage(long item) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr * GetItemAttr(long item) const;

    enum {
        BLIST_LIST = wxID_HIGHEST,
        BLIST_DLMOD,
        BLIST_DLMAP
    };


  protected:
    static int CompareStatus( DataType u1, DataType u2 );
    static int ComparePlayer( DataType u1, DataType u2 );

    static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;
    int GetIndexFromData( const DataType& data ) const;

    wxMenu* m_popup;

    virtual void Sort();

    DECLARE_EVENT_TABLE()
};



#endif // SPRINGLOBBY_HEADERGUARD_BATTLELISTCTRL_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

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

