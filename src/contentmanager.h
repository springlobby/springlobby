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
class Exception;

class ContentManager {

public:
	static ContentManager* Instance();
	static void Release();

	bool IsNewApplicationVersionAvailable();
	bool UpdateApplication();
	wxString GetLatestApplicationVersionAvailable();

	bool IsHavingSpringVersion(const wxString& engineString, const wxString& versionString);
	ContentDownloadRequest WhatContentForBattleIsRequired(const IBattle& battle);
	bool DownloadContent(const ContentDownloadRequest& request);

private:
	ContentManager();
	virtual ~ContentManager();

private:
	wxString latestApplicationVersionAvailable;

private:
	static ContentManager* m_Instance;
};

#endif /* SRC_CONTENTMANAGER_H_ */
