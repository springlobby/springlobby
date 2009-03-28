#ifndef SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED
#define SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED

#include "../customvirtlistctrl.h"

class wxMenu;
struct Replay;
class wxListEvent;
class wxCommandEvent;
class Ui;
class ReplayList;


class ReplayListCtrl : public CustomVirtListCtrl< const Replay* >
{
  public:
    ReplayListCtrl( wxWindow* parent );
    ~ReplayListCtrl();

    void AddReplay( const Replay& replay );
    void RemoveReplay( const Replay& replay );
    void OnListRightClick( wxListEvent& event );
    void OnDLMap( wxCommandEvent& event );
    void OnDLMod( wxCommandEvent& event );
    void OnMouseMotion(wxMouseEvent& event);
    void OnColClick( wxListEvent& event );

    virtual void HighlightItem( long item ){};

    //these are overloaded to use list in virtual style
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    wxListItemAttr * OnGetItemAttr(long item) const;
    int GetIndexFromData( const DataType& data ) const;

  protected:
    static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;

    virtual void Sort();

    wxMenu* m_popup;

    DECLARE_EVENT_TABLE()
};

enum
{
    RLIST_LIST,
    RLIST_DLMOD,
    RLIST_DLMAP
};

#endif // SPRINGLOBBY_REPLAYLISTCTRL_H_INCLUDED

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

