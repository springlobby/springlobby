/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <string>

class ServerManager
{
private:
	ServerManager();
	virtual ~ServerManager();

public:
	static ServerManager* Instance();
	static void Release();

	void ConnectToServer();
	void ReconnectToServer();
	void DisconnectFromServer();
	void RegisterNewUser(const std::string&, const std::string&, const std::string&);
	void DoConnectToServer(const std::string&, const std::string&, const std::string&);
	void JoinChannel(const std::string&, const std::string&);
	bool DownloadContent(const std::string&, const std::string&, const std::string&);

	bool IsConnected();

private:
	static ServerManager* m_Instance;
};

#endif /* SERVERMANAGER_H */
