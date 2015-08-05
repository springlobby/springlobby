/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_CONTENTMANAGER_H_
#define SRC_CONTENTMANAGER_H_

#include <vector>

#include <wx/thread.h>
#include <string>

class ContentDownloadRequest;
class IBattle;
class Exception;
class IDownload;
class DownloadInfo;

class ContentManager {

public:
	static ContentManager* Instance();
	static void Release();

	bool IsNewApplicationVersionAvailable();
	bool UpdateApplication();
	std::string GetLatestApplicationVersionAvailable();

	bool IsHavingSpringVersion(const std::string& engineString, const std::string& versionString);
	ContentDownloadRequest WhatContentForBattleIsRequired(const IBattle& battle);
	bool DownloadContent(const ContentDownloadRequest& request);

	void OnDownloadStarted(IDownload* download);
	void OnDownloadFinished(IDownload* download);

	bool IsContentAlreadyBeingDownloaded(const std::string& name);

private:
	ContentManager();
	virtual ~ContentManager();

private:
	std::string latestApplicationVersionAvailable;
	std::vector<DownloadInfo*> downloadsList;
	wxMutex mutex;

private:
	static ContentManager* m_Instance;
};

#endif /* SRC_CONTENTMANAGER_H_ */
