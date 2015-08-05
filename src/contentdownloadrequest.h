/*
 * contentdownloadrequest.h
 *
 *  Created on: 11 июля 2015 г.
 *      Author: Руслан
 */

#ifndef SRC_CONTENTDOWNLOADREQUEST_H_
#define SRC_CONTENTDOWNLOADREQUEST_H_

#include <wx/string.h>

class ContentDownloadRequest {
public:
	ContentDownloadRequest();
	virtual ~ContentDownloadRequest();

	void EngineRequired(const wxString& engineVesrion);
	void MapRequired(const wxString& mapName, const wxString& mapHash);
	void GameRequired(const wxString& modName, const wxString& modHash);

	bool IsSomethingNeeded() const;
	bool NothingIsRequired() const;

	bool IsEngineRequested() const;
	bool IsMapRequested() const;
	bool IsModRequested() const;

	wxString GetEngineVersion() const;
	wxString GetMapName() const;
	wxString GetModName() const;
	wxString GetModHash() const;
	wxString GetMapHash() const;

private:
	wxString engineVersion;
	wxString mapName;
	wxString modName;
	wxString modHash;
	wxString mapHash;
};

#endif /* SRC_CONTENTDOWNLOADREQUEST_H_ */
