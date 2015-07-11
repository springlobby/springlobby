/*
 * contentdownloadrequest.cpp
 *
 *  Created on: 11 июля 2015 г.
 *      Author: Руслан
 */

#include "contentdownloadrequest.h"

ContentDownloadRequest::ContentDownloadRequest() {
	// TODO Auto-generated constructor stub
	engineVersion = wxEmptyString;
	mapName = wxEmptyString;
	modName = wxEmptyString;
}

ContentDownloadRequest::~ContentDownloadRequest() {
	// TODO Auto-generated destructor stub
}

void ContentDownloadRequest::EngineRequired(const wxString& engineVersion) {
	this->engineVersion = engineVersion;
}

void ContentDownloadRequest::MapRequired(const wxString& mapName, const wxString& mapHash) {
	this->mapName = mapName;
	this->mapHash = mapHash;
}

void ContentDownloadRequest::ModRequired(const wxString& modName, const wxString& modHash) {
	this->modName = modName;
	this->modHash = modHash;
}

bool ContentDownloadRequest::IsSomethingNeeded() const {
	if (engineVersion != wxEmptyString ||
		mapName != wxEmptyString 	||
		modName != wxEmptyString ) {
		return true;
	} else {
		return false;
	}
}

bool ContentDownloadRequest::NothingIsRequired() const {
	return (IsSomethingNeeded() == false);
}

bool ContentDownloadRequest::IsEngineRequested() const {
	return (engineVersion != wxEmptyString);
}

bool ContentDownloadRequest::IsMapRequested() const {
	return (mapName != wxEmptyString);
}

bool ContentDownloadRequest::IsModRequested() const {
	return (modName != wxEmptyString);
}

wxString ContentDownloadRequest::GetEngineVersion() const {
	return engineVersion;
}

wxString ContentDownloadRequest::GetMapName() const {
	return mapName;
}

wxString ContentDownloadRequest::GetModName() const {
	return modName;
}

wxString ContentDownloadRequest::GetModHash() const {
	return modHash;
}

wxString ContentDownloadRequest::GetMapHash() const {
	return mapHash;
}
