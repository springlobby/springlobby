/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_DOWNLOADER_DOWNLOADDATAVIEWCTRL_H_
#define SRC_DOWNLOADER_DOWNLOADDATAVIEWCTRL_H_

#include "gui/basedataviewctrl.h"

class ObserverDownloadInfo;
class wxString;
class wxWindow;
class wxCommandEvent;
class wxPoint;

class DownloadDataViewCtrl: public BaseDataViewCtrl<ObserverDownloadInfo> {
public:
	DownloadDataViewCtrl(const wxString dataViewName, wxWindow* parent);
	virtual ~DownloadDataViewCtrl();

	virtual void SetTipWindowText(const long item_hit, const wxPoint& position);
//	bool AddTorrentInfo(const DataType& info);
//	bool RemoveTorrentInfo(const DataType& info);
//	void UpdateTorrentInfo(const DataType& info);

	void UpdateTorrentsList();

	virtual void HighlightItem(long item);

	void OnCancel(wxCommandEvent& event);
	void OnRetry(wxCommandEvent& event);

private:
//	bool IsTorrentActive(const DataType& info) const;

	wxMenu* m_popup;

private:
	enum ColumnIndexes
	{
		NAME = 0,
		STATUS,
		COMPLETE,
		SPEED,
		ETA,
		FILESIZE
	};
	enum {
		DOWNLOAD_DATAVIEW_ID,
		DOWNLOAD_DATAVIEW_CANCEL,
		DOWNLOAD_DATAVIEW_RETRY
	};

	DECLARE_EVENT_TABLE()
};

#endif /* SRC_DOWNLOADER_DOWNLOADDATAVIEWCTRL_H_ */
