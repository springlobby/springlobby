/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H

#include "gui/customvirtlistctrl.h"
#include "prdownloader.h"
#include "downloadsobserver.h"

#include <map>

//struct TorrentInfos;
class wxMenu;
class Battle;
class wxListEvent;
class wxCommandEvent;
class Ui;


/** \brief list all currently active and finished donwloads with their infos
 * the list is newly populated every n-seconds from Ui::OnUpdate()
 */
class DownloadListCtrl : public CustomVirtListCtrl<ObserverDownloadInfo, DownloadListCtrl>
{
  public:
	DownloadListCtrl( wxWindow* parent );
	~DownloadListCtrl();

    void Sort();

    void OnListRightClick( wxListEvent& event );

    virtual void SetTipWindowText( const long item_hit, const wxPoint& position);
	bool AddTorrentInfo(const DataType& info);
	bool RemoveTorrentInfo(const DataType& info);
	void UpdateTorrentInfo(const DataType& info);

	//! Called by Ui::OnUpdate()
	//! Use DownloadsObserver to get informations about downloads
    void UpdateTorrentsList();

    virtual void HighlightItem( long item );

    void OnCancel( wxCommandEvent& event );
    void OnRetry( wxCommandEvent& event );

	//these are overloaded to use list in virtual style
    wxString GetItemText(long item, long column) const;
    int GetItemImage(long item) const;
    int GetItemColumnImage(long item, long column) const;
    wxListItemAttr* GetItemAttr(long ) const {return 0;}

  protected:
	int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) const;
	int GetIndexFromData( const DataType& data ) const;

	bool IsTorrentActive(const DataType& info) const;

	wxMenu* m_popup;

    enum {
        TLIST_CLICK,
		TLIST_CANCEL,
		TLIST_RETRY
    };

    DECLARE_EVENT_TABLE()
};

#endif // SPRINGLOBBY_HEADERGUARD_TORRENTLISTCTRL_H

