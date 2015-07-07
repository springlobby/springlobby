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

	virtual void SetTipWindowText(const long, const wxPoint&);
	bool AddDownloadInfo(const ObserverDownloadInfo&);
	bool RemoveDownloadInfo(const ObserverDownloadInfo&);
	void UpdateDownloadInfo(const ObserverDownloadInfo&);
	void UpdateDownloadsList();

	virtual void HighlightItem(long);

	void OnCancel(wxCommandEvent&);
	void OnRetry(wxCommandEvent&);

private:
	bool IsTorrentActive(const ObserverDownloadInfo&) const;

private:
	std::list<ObserverDownloadInfo> downloadInfoList;

private:
	enum ColumnIndexes
	{
		NAME = 0,
		STATUS,
		P_COMPLETE,
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
