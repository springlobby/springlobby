/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_OFFLINESERVER_H
#define SPRINGLOBBY_HEADERGUARD_OFFLINESERVER_H

#include "tasserver.h"
#include <string>

//! @brief TASServer protocol implementation.
class OfflineServer : public TASServer
{
public:
	OfflineServer();
	~OfflineServer();
	void Connect(const std::string& servername, const std::string& addr, const int port);
	void SendCmd(const std::string& command, const std::string& param)
	{
		SendCmd(command, param, false);
	};
	void SendCmd(const std::string& command)
	{
		SendCmd(command, "", false);
	}
	void SendCmd(const std::string& command, const std::string& param, bool relay);
	void Login()
	{
	}
	bool IsConnected()
	{
		return true;
	}

private:
	ServerEvents* m_se;
};

#endif // SPRINGLOBBY_HEADERGUARD_OFFLINESERVER_H
