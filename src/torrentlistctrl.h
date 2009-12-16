#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H

#ifndef NO_TORRENT_SYSTEM

#include "customvirtlistctrl.h"
#include "torrentwrapper.h"
#include <map>

//struct TorrentInfos;
class wxMenu;
class Battle;
class wxListEvent;
class wxCommandEvent;
class Ui;


/** \brief list all currently active (queued,lecching,seeding) torrents with their infos
 * the list is newly populated every n-seconds from Ui::OnUpdate()
 */
class TorrentListCtrl : public CustomVirtListCtrl<TorrentInfos, TorrentListCtrl>
{
  public:
    TorrentListCtrl( wxWindow* parent );
    ~TorrentListCtrl();

    void Sort();

    void OnListRightClick( wxListEvent& event );

    virtual void SetTipWindowText( const long item_hit, const wxPoint position);
	bool AddTorrentInfo(const DataType& info);
	bool RemoveTorrentInfo(const DataType& info);
	void UpdateTorrentInfo(const DataType& info);
	void RefreshTorrentStatus();

    virtual void HighlightItem( long item );

    void OnCancel( wxCommandEvent& event );
    void OnRetry( wxCommandEvent& event );

	//these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemImage(long item) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr* GetItemAttr(long ) const {return 0;}

  protected:
	static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;
	int GetIndexFromData( const DataType& data ) const;

	bool IsTorrentActive(const DataType& info);

	wxMenu* m_popup;

    enum {
        TLIST_CLICK,
		TLIST_CANCEL,
		TLIST_RETRY
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

