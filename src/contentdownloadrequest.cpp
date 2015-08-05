/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentdownloadrequest.h"

ContentDownloadRequest::ContentDownloadRequest()
{
	engineVersion.clear();
	mapName.clear();
	gameName.clear();
}

ContentDownloadRequest::~ContentDownloadRequest()
{
}

void ContentDownloadRequest::EngineRequired(const std::string& engineVersion) {
	this->engineVersion = engineVersion;
}

void ContentDownloadRequest::MapRequired(const std::string& mapName, const std::string& mapHash) {
	this->mapName = mapName;
	this->mapHash = mapHash;
}

void ContentDownloadRequest::GameRequired(const std::string& gameName, const std::string& gameHash) {
	this->gameName = gameName;
	this->gameHash = gameHash;
}

bool ContentDownloadRequest::IsSomethingNeeded() const {
	if (!engineVersion.empty() ||
		!mapName.empty()||
		!gameName.empty() ) {
		return true;
	} else {
		return false;
	}
}

bool ContentDownloadRequest::NothingIsRequired() const {
	return (IsSomethingNeeded() == false);
}

bool ContentDownloadRequest::IsEngineRequested() const {
	return (!engineVersion.empty());
}

bool ContentDownloadRequest::IsMapRequested() const {
	return (!mapName.empty());
}

bool ContentDownloadRequest::IsGameRequested() const {
	return (!gameName.empty());
}

std::string ContentDownloadRequest::GetEngineVersion() const {
	return engineVersion;
}

std::string ContentDownloadRequest::GetMapName() const {
	return mapName;
}

std::string ContentDownloadRequest::GetGameName() const {
	return gameName;
}

std::string ContentDownloadRequest::GetGameHash() const {
	return gameHash;
}

std::string ContentDownloadRequest::GetMapHash() const {
	return mapHash;
}
