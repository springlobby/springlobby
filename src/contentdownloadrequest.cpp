/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "contentdownloadrequest.h"

#include <vector>

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

std::string ContentDownloadRequest::GetRequiredContentAsString() const {

	std::vector<std::string> itemsNeeded;

	if (IsMapRequested()) {
		itemsNeeded.push_back("map");
	}
	if (IsGameRequested()) {
		itemsNeeded.push_back("game");
	}
	if (IsEngineRequested()) {
		itemsNeeded.push_back("engine");
	}

	std::string resultingString;
	std::string prefixString;

	if (IsSomethingNeeded()) {
		resultingString = "This battle requires";

		for (size_t i = 0; i < itemsNeeded.size(); ++i) {
			if (i == (itemsNeeded.size() - 1) && i!=0 ) {
				prefixString = " and ";
			} else if (i == 0) {
				prefixString = " ";
			} else {
				prefixString = ", ";
			}
			resultingString += prefixString + itemsNeeded[i];
		}

		resultingString += " to be downloaded! ";
	}

	return resultingString;
}
