/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_DOWNLOADER_DOWNLOADDATAVIEWCTRL_H_
#define SRC_DOWNLOADER_DOWNLOADDATAVIEWCTRL_H_

#include "gui/basedataviewctrl.h"
#include "downloader/prdownloader.h"

class wxString;
class wxWindow;
class wxCommandEvent;
class wxPoint;
class DownloadInfo;

class DownloadDataViewCtrl: public BaseDataViewCtrl<PrDownloader::DownloadProgress> {
public:
	DownloadDataViewCtrl(const wxString dataViewName, wxWindow* parent);
	virtual ~DownloadDataViewCtrl();

	virtual void SetTipWindowText(const long, const wxPoint&);
	virtual void HighlightItem(long);

	void OnCancel(wxCommandEvent&);
	void OnRetry(wxCommandEvent&);

private:
	void OnDownloadStarted();
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
