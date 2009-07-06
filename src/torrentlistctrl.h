#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H

#ifndef NO_TORRENT_SYSTEM

#include "customlistctrl.h"

#include <map>

struct TorrentInfos;
class wxMenu;
class Battle;
class wxListEvent;
class wxCommandEvent;
class Ui;

typedef std::map<int,TorrentInfos> map_infos;

/** \brief list all currently active (queued,lecching,seeding) torrents with their infos
 * the list is newly populated every n-seconds from Ui::OnUpdate()
 */
class TorrentListCtrl : public CustomListCtrl
{
  public:
    TorrentListCtrl( wxWindow* parent );
    ~TorrentListCtrl();

    void Sort();

    void OnListRightClick( wxListEvent& event );

    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
    void OnColClick( wxListEvent& event );
    void SetInfoMap( map_infos* map);
    virtual void HighlightItem( long item );

  protected:
    static int wxCALLBACK CompareNameUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareNameDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCompletedUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCompletedDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSizeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSizeDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSizeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSizeDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSpeedUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareDownSpeedDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSpeedUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareUpSpeedDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareLeechUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareLeechDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareEtaUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareEtaDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCopiesUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareCopiesDOWN(long item1, long item2, long sortData);
    static int wxCALLBACK CompareFileSizeUP(long item1, long item2, long sortData);
    static int wxCALLBACK CompareFileSizeDOWN(long item1, long item2, long sortData);
    static map_infos* m_info_map;


    enum {
        TLIST_CLICK
    };

    DECLARE_EVENT_TABLE()
};



#endif

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H

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

