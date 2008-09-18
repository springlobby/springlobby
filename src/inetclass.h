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
};

#endif // SPRINGLOBBY_HEADERGUARD_INETCLASS_H
