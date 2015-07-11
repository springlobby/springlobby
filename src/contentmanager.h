/*
 * contentmanager.h
 *
 *  Created on: 11 июля 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_CONTENTMANAGER_H_
#define SRC_CONTENTMANAGER_H_

class wxString;
class ContentDownloadRequest;
class IBattle;

class ContentManager {

public:
	ContentManager* Instance();
	void Release();

	bool IsNewApplicationVersionAvailable();
	bool UpdateApplication(const wxString& latestVersion);

	bool IsHavingSpringVersion(const wxString& engineString, const wxString& versionString);
	ContentDownloadRequest WhatContentForBattleIsRequired(const IBattle& battle);
	bool DownloadContent(const ContentDownloadRequest& request);

private:
	ContentManager();
	virtual ~ContentManager();

private:
	static ContentManager* m_Instance;
};

#endif /* SRC_CONTENTMANAGER_H_ */
