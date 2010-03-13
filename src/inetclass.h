#ifndef SPRINGLOBBY_HEADERGUARD_INETCLASS_H
#define SPRINGLOBBY_HEADERGUARD_INETCLASS_H

class Socket;

//! @brief Abstract baseclass that is used when needed to interface with socket class
class iNetClass
{
  public:

    iNetClass(){}
    virtual ~iNetClass(){}

    virtual void OnConnected( Socket* sock ) = 0;
    virtual void OnDisconnected( Socket* sock ) = 0;
    virtual void OnDataReceived( Socket* sock ) = 0;
	virtual void Ping() = 0;
};

class FakeNetClass : public iNetClass
{
  public:

    FakeNetClass(){}
    ~FakeNetClass(){}

    void OnConnected( Socket* /*unused*/ ) {}
    void OnDisconnected( Socket* /*unused*/ ) {}
    void OnDataReceived( Socket* /*unused*/ ) {}
	void Ping() {}
};

#endif // SPRINGLOBBY_HEADERGUARD_INETCLASS_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

