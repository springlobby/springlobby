/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/log.h>

#include <string>

#include "utils/slpaths.h"
#include "contentdownloadrequest.h"
#include "exception.h"
#include "servermanager.h"
#include "utils/slconfig.h"
#include "settings.h"
#include "utils/conversion.h"
#include "httpfile.h"
#include "ibattle.h"
#include "utils/version.h"
#include "utils/platform.h"
#include "updatehelper.h"
#include "downloader/prdownloader.h"
#include "lslunitsync/unitsync.h"
#include "utils/globalevents.h"
#include "downloadinfo.h"

#include "exception.h"
#include "contentdownloadrequest.h"
#include "contentmanager.h"

ContentManager::ContentManager() {
	// TODO Auto-generated constructor stub

}

ContentManager* ContentManager::Instance() {
	if (m_Instance == nullptr) {
		m_Instance = new ContentManager();
	}

	return m_Instance;
}

void ContentManager::Release() {
	if (m_Instance != nullptr) {
		delete m_Instance;
		m_Instance = nullptr;
	}
}

bool ContentManager::IsNewApplicationVersionAvailable() {
	std::string latestVersion = GetLatestApplicationVersionAvailable();
	// Need to replace crap chars or versions will always be inequal

	/*Some error occurred*/
	if (latestVersion.empty()) {
		return false;
	}

	//get current rev w/o AUX_VERSION added
	const std::string myVersion = getSpringlobbyVersion();

	/*Check versions for equality*/
	return (latestVersion != myVersion);
}

bool ContentManager::UpdateApplication(){
	const std::string latestVersion = GetLatestApplicationVersionAvailable();
	const std::string updatedir = SlPaths::GetUpdateDir();
	const size_t mindirlen = 9; // safety, minimal is/should be: C:\update
	if ((updatedir.size() <= mindirlen)) {
		throw Exception(_T("Invalid update dir: ") + updatedir);
	}
	if (wxDirExists(updatedir)) {
		if (!SlPaths::RmDir(updatedir)) {
			throw Exception(_T("Couldn't cleanup ") + updatedir);
		}
	}
	if (!SafeMkdir(updatedir)) {
		throw Exception(_T("couldn't create update directory") + updatedir);
	}

	if (!wxFileName::IsDirWritable(updatedir)) {
		throw Exception(_T("dir not writable: ") + updatedir);
	}

	const std::string dlfilepath = SlPaths::GetLobbyWriteDir() + "springlobby-latest.zip";
	if (wxFileExists(dlfilepath) && !(wxRemoveFile(dlfilepath))) {
		throw Exception(_T("couldn't delete: ") + dlfilepath);
	}
	const std::string dlurl = GetDownloadUrl(latestVersion);
	return prDownloader().Download(dlfilepath, dlurl);
}

bool ContentManager::IsHavingSpringVersion(const std::string& engineString,
		const std::string& versionString) {

	wxASSERT(engineString == _T("spring"));

	if (sett().GetDisableSpringVersionCheck()) {
		return true;
	}

	const std::string ver = SlPaths::GetCompatibleVersion(versionString);
	if (ver.empty() == false) {
		if (SlPaths::GetCurrentUsedSpringIndex() != ver) {
//			wxLogMessage(_T("server enforce usage of version: %s, switching to profile: %s"), ver).c_str(), ver).c_str());
			SlPaths::SetUsedSpringIndex(ver);
			LSL::usync().ReloadUnitSyncLib();
		}
		return true;
	}
	return false; // no compatible version found
}

ContentDownloadRequest ContentManager::WhatContentForBattleIsRequired(
		const IBattle& battle) {

	ContentDownloadRequest contentNeeded = ContentDownloadRequest();

	const std::string engineVersion = battle.GetBattleOptions().engineVersion;
	const std::string engineName = battle.GetBattleOptions().engineName;

	/*Engine is needed*/
	if (IsHavingSpringVersion(engineName, engineVersion) == false) {
		contentNeeded.EngineRequired(engineVersion);
	}

	/*Map is needed*/
	if (battle.MapExists() == false) {
		contentNeeded.MapRequired(battle.GetHostMapName(), battle.GetHostMapHash());
	}

	/*Mod (game) is needed*/
	if (battle.GameExists() == false) {
		contentNeeded.GameRequired(battle.GetHostGameName(), battle.GetHostGameHash());
	}

	return contentNeeded;
}

bool ContentManager::DownloadContent(const ContentDownloadRequest& request) {
	if (request.NothingIsRequired()) {
		return false;
	}

	//TODO: check if content already exists on HDD or was just downloaded!

	if (request.IsEngineRequested()) {
		if (IsContentAlreadyBeingDownloaded(request.GetEngineVersion())) {
			throw Exception(_("Engine being downloaded already! Please wait!"));
		}
		ServerManager::Instance()->DownloadContent(PrDownloader::GetEngineCat(), request.GetEngineVersion(), "");
	}

	if (request.IsMapRequested()) {
		if (IsContentAlreadyBeingDownloaded(request.GetMapName())) {
			throw Exception(_("Map being downloaded alredy! Please wait!"));
		}
		ServerManager::Instance()->DownloadContent("map", request.GetMapName(), request.GetMapHash());
	}

	if (request.IsGameRequested()) {
		if (IsContentAlreadyBeingDownloaded(request.GetGameName())) {
			throw Exception(_("Mod being downloaded alredy! Please wait!"));
		}
		ServerManager::Instance()->DownloadContent("game", request.GetGameName(), request.GetGameHash());
	}

	return true;
}

std::string ContentManager::GetLatestApplicationVersionAvailable() {
	if (latestApplicationVersionAvailable.empty()) {
		std::string version = GetHttpFile(GetLatestVersionUrl());
		latestApplicationVersionAvailable = TowxString(version).Trim().Trim(false);
	} else {
		//TODO: update cache if needed
	}

	return latestApplicationVersionAvailable;
}

bool ContentManager::IsContentAlreadyBeingDownloaded(const std::string& name) {

	if (name.empty()) {
		return false;
	}

	for(auto downloadItem : downloadsList)
	{
		if (downloadItem->IsFinished() == false) {
			if (downloadItem->GetName() == name) {
				return true;
			}
		}
	}

	return false;
}

ContentManager::~ContentManager() {
	// TODO Auto-generated destructor stub
}

ContentManager* ContentManager::m_Instance = nullptr;

void ContentManager::OnDownloadStarted(IDownload* download) {
	wxMutexLocker locker(mutex);

	wxASSERT(download != nullptr);

	for(auto downloadItem : downloadsList)
	{
		if (downloadItem->GetIDownload() == download) {
			wxASSERT(false);
		}
	}

	DownloadInfo* dInfo = new DownloadInfo(download);
	downloadsList.push_back(dInfo);

	GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadStarted, dInfo);
}

void ContentManager::OnDownloadFinished(IDownload* download) {
	wxMutexLocker locker(mutex);

	wxASSERT(download != nullptr);

	for(auto downloadItem : downloadsList)
	{
		if (downloadItem->GetIDownload() == download) {
			downloadItem->DownloadFinished();

			GlobalEventManager::Instance()->Send(GlobalEventManager::OnDownloadComplete, downloadItem);
			return;
		}
	}

	wxASSERT(false);
}
