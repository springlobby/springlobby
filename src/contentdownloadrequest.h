/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_CONTENTDOWNLOADREQUEST_H_
#define SRC_CONTENTDOWNLOADREQUEST_H_

#include <string>

class ContentDownloadRequest {
public:
	ContentDownloadRequest();
	virtual ~ContentDownloadRequest();

	void EngineRequired(const std::string& engineVersion);
	void MapRequired(const std::string& mapName, const std::string& mapHash);
	void GameRequired(const std::string& gameName, const std::string& gameHash);

	bool IsSomethingNeeded() const;
	bool NothingIsRequired() const;

	bool IsEngineRequested() const;
	bool IsMapRequested() const;
	bool IsGameRequested() const;

	std::string GetEngineVersion() const;
	std::string GetGameName() const;
	std::string GetGameHash() const;
	std::string GetMapName() const;
	std::string GetMapHash() const;

	std::string GetRequiredContentAsString() const;

private:
	std::string engineVersion;
	std::string mapName;
	std::string mapHash;
	std::string gameName;
	std::string gameHash;
};

#endif /* SRC_CONTENTDOWNLOADREQUEST_H_ */
