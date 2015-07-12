/*
 * contentmanager.cpp
 *
 *  Created on: 11 июля 2015 г.
 *      Author: Руслан
 */

#include <wx/string.h>

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
	wxString latestVersion = GetLatestApplicationVersionAvailable();
	// Need to replace crap chars or versions will always be inequal
	latestVersion.Replace(_T(" "), wxEmptyString, true);
	latestVersion.Replace(_T("\n"), wxEmptyString, true);
	latestVersion.Replace(_T("\t"), wxEmptyString, true);

	/*Some error occurred*/
	if (latestVersion.empty()) {
		return false;
	}

	//get current rev w/o AUX_VERSION added
	const wxString myVersion = TowxString(getSpringlobbyVersion());

	/*Check versions for equality*/
	if (latestVersion.IsSameAs(myVersion, false) == false) {
		return true;
	} else {
		return false;
	}
}

bool ContentManager::UpdateApplication(){
	const wxString latestVersion = GetLatestApplicationVersionAvailable();
	const wxString updatedir = TowxString(SlPaths::GetUpdateDir());
	const size_t mindirlen = 9; // safety, minimal is/should be: C:\update
	if ((updatedir.size() <= mindirlen)) {
		throw new Exception(_T("Invalid update dir: ") + updatedir);
		return false;
	}
	if (wxDirExists(updatedir)) {
		if (!SlPaths::RmDir(updatedir.ToStdString())) {
			throw new Exception(_T("Couldn't cleanup ") + updatedir);
		}
	}
	if (!SafeMkdir(updatedir)) {
		throw new Exception(_T("couldn't create update directory") + updatedir);
	}

	if (!wxFileName::IsDirWritable(updatedir)) {
		throw new Exception(_T("dir not writable: ") + updatedir);
	}

	const std::string dlfilepath = SlPaths::GetLobbyWriteDir() + "springlobby-latest.zip";
	if (wxFileExists(TowxString(dlfilepath)) && !(wxRemoveFile(TowxString(dlfilepath)))) {
		throw new Exception(_T("couldn't delete: ") + TowxString(dlfilepath));
	}
	const std::string dlurl = GetDownloadUrl(latestVersion.ToStdString());
	return prDownloader().Download(dlfilepath, dlurl);
}

bool ContentManager::IsHavingSpringVersion(const wxString& engineString,
		const wxString& versionString) {

	wxASSERT(engineString == _T("spring"));

	if (sett().GetDisableSpringVersionCheck()) {
		return true;
	}

	const std::string ver = SlPaths::GetCompatibleVersion(versionString.ToStdString());
	if (ver.empty() == false) {
		if (SlPaths::GetCurrentUsedSpringIndex() != ver) {
//			wxLogMessage(_T("server enforce usage of version: %s, switching to profile: %s"), TowxString(ver).c_str(), TowxString(ver).c_str());
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
	if (battle.ModExists() == false) {
		contentNeeded.ModRequired(battle.GetHostModName(), battle.GetHostModHash());
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
			throw new Exception(_("Engine being downloaded alredy! Please wait!"));
		}
		ServerManager::Instance()->DownloadContent(PrDownloader::GetEngineCat(), request.GetEngineVersion().ToStdString(), "");
	}

	if (request.IsMapRequested()) {
		if (IsContentAlreadyBeingDownloaded(request.GetMapName())) {
			throw new Exception(_("Map being downloaded alredy! Please wait!"));
		}
		ServerManager::Instance()->DownloadContent("map", request.GetMapName().ToStdString(), request.GetMapHash().ToStdString());
	}

	if (request.IsModRequested()) {
		if (IsContentAlreadyBeingDownloaded(request.GetModName())) {
			throw new Exception(_("Mod being downloaded alredy! Please wait!"));
		}
		ServerManager::Instance()->DownloadContent("game", request.GetModName().ToStdString(), request.GetModHash().ToStdString());
	}

	return true;
}

wxString ContentManager::GetLatestApplicationVersionAvailable() {
	if (latestApplicationVersionAvailable == wxEmptyString) {
		latestApplicationVersionAvailable = GetHttpFile(cfg().ReadString("/General/UpdateUrl"));
	} else {
		//TODO: update cache if needed
	}

	return latestApplicationVersionAvailable;
}

bool ContentManager::IsContentAlreadyBeingDownloaded(const wxString& name) {

	if (name == wxEmptyString) {
		return false;
	}

	for(auto downloadItem : downloadsList)
	{
		if (downloadItem->IsFinished() == false) {
			if (downloadItem->GetName().Lower() == name.Lower()) {
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
