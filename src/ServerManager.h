/* 
 * File:   ServerManager.h
 * Author: Руслан
 *
 * Created on 12 июня 2015 г., 15:01
 */

#ifndef SERVERMANAGER_H
#define	SERVERMANAGER_H

#include <string>

class ServerManager {
private:
	ServerManager();
	virtual ~ServerManager();

public:	
	static ServerManager* Instance();
	static void Release();
	
	void ConnectToServer();
	void ReconnectToServer();
	void DisconnectFromServer();
	void RegisterNewUser(const wxString&, const wxString&, const wxString&);
	void DoConnectToServer(const wxString&, const wxString&, const wxString&);
	void JoinChannel(const wxString&, const wxString&);
	bool DownloadContent(const std::string&, const std::string&, const std::string&);
	
	bool IsConnected();
	
private:
	static ServerManager* m_Instance;
};

#endif	/* SERVERMANAGER_H */

