/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_INETCLASS_H
#define SPRINGLOBBY_HEADERGUARD_INETCLASS_H

class Socket;

//! @brief Abstract baseclass that is used when needed to interface with socket class
class iNetClass
{
  public:

    iNetClass(){}
    virtual ~iNetClass(){}

    virtual void OnConnected( Socket& sock ) = 0;
    virtual void OnDisconnected( Socket& sock ) = 0;
    virtual void OnDataReceived( Socket& sock ) = 0;
	virtual void Ping() = 0;
};

class FakeNetClass : public iNetClass
{
  public:

    FakeNetClass(){}
    ~FakeNetClass(){}

    void OnConnected( Socket& /*unused*/ ) {}
    void OnDisconnected( Socket& /*unused*/ ) {}
    void OnDataReceived( Socket& /*unused*/ ) {}
	void Ping() {}
};

#endif // SPRINGLOBBY_HEADERGUARD_INETCLASS_H

