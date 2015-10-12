/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_INETCLASS_H
#define SPRINGLOBBY_HEADERGUARD_INETCLASS_H

//! @brief Abstract baseclass that is used when needed to interface with socket class

#include <string>
#include <wx/socket.h>

class iNetClass
{
public:
	iNetClass()
	{
	}
	virtual ~iNetClass()
	{
	}

	virtual void OnConnected()
	{
	}
	virtual void OnDisconnected(wxSocketError)
	{
	}
	virtual void OnDataReceived()
	{
	}
	virtual void OnError(const std::string& /*error*/)
	{
	}
	virtual void Ping()
	{
	}
};

#endif // SPRINGLOBBY_HEADERGUARD_INETCLASS_H
