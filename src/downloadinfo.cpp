/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "downloadinfo.h"

#include <wx/debug.h>

#include "downloader/lib/src/Downloader/Download.h"

DownloadInfo::DownloadInfo(IDownload* download) {
	// TODO Auto-generated constructor stub

	wxASSERT(download != nullptr);

	iDownload = download;
	finishedFlag = false;

	UpdateInfo();
}

DownloadInfo::~DownloadInfo() {
	// TODO Auto-generated destructor stub
}

IDownload* DownloadInfo::GetIDownload() const {
	return iDownload;
}

void DownloadInfo::DownloadFinished() {
	iDownload = nullptr;
	finishedFlag = true;
	downloadedSize = totalSize;
}

wxString DownloadInfo::GetName() const {
	return downloadName;
}

bool DownloadInfo::IsFinished() const {
	return finishedFlag;
}

int DownloadInfo::GetSize() const {
	return totalSize;
}

int DownloadInfo::GetProgressPercent() const {

	int computedProgress = static_cast<int>( (downloadedSize * 100.0F) / totalSize );

	return computedProgress;
}

void DownloadInfo::UpdateInfo() {

	if (iDownload == nullptr) {
		return;
	}

	downloadName = iDownload->origin_name;
	totalSize = iDownload->size;
	downloadedSize = iDownload->progress;
}
