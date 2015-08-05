/*
 * downloadinfo.h
 *
 *  Created on: 12 июля 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_DOWNLOADINFO_H_
#define SRC_DOWNLOADINFO_H_

#include <wx/string.h>
#include <wx/thread.h>

class IDownload;

class DownloadInfo {
public:
	DownloadInfo(IDownload* download);
	virtual ~DownloadInfo();

	IDownload* GetIDownload() const;
	void DownloadFinished();
	wxString GetName() const;
	bool IsFinished() const;
	int GetSize() const;
	int GetProgressPercent() const;
	void UpdateInfo();

private:
	IDownload* iDownload;
	wxString downloadName;
	mutable wxMutex mutex;

	bool finishedFlag;
	int totalSize;
	int downloadedSize;
};

#endif /* SRC_DOWNLOADINFO_H_ */
