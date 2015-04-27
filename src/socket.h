/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_SOCKET_H
#define SPRINGLOBBY_HEADERGUARD_SOCKET_H


/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

DO NOT CHANGE THIS FILE!

this file is deprecated and will be replaced with

lsl/networking/socket.h

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/


#include <wx/string.h>
#include <wx/event.h>
#include <wx/socket.h>
#include <string>

class iNetClass;
class wxCriticalSection;

enum SockState {
	SS_Closed,
	SS_Connecting,
	SS_Open
};

enum SockError {
	SE_No_Error,
	SE_NotConnected,
	SE_Resolve_Host_Failed,
	SE_Connect_Host_Failed
};

const int SOCKET_ID = 100;


class wxSocketEvent;

//! @brief Class that implements a TCP client socket.
class Socket : public wxEvtHandler
{
public:
	Socket(iNetClass& netclass);
	~Socket();

	// Socket interface

	void Connect(const wxString& addr, const int port);
	void Disconnect();

	bool Send(const wxString& data);
	wxString Receive();
	std::string GetLocalAddress() const;
	std::string GetHandle() const
	{
		return m_handle;
	}

	SockState State();
	SockError Error() const;

	void SetSendRateLimit(int Bps = -1);
	int GetSendRateLimit()
	{
		return m_rate;
	}
	void Update(int mselapsed);

	void SetTimeout(const int seconds);

private:
	void OnSocketEvent(wxSocketEvent& event);
	bool _Send(const wxString& data);
	void InitSocket(wxSocketClient& socket);

	// Socket variables

	wxSocketClient m_sock;
	wxString m_ping_msg;
	std::string m_handle;
	bool m_connecting;
	iNetClass& m_net_class;
	int m_rate;
	int m_sent;
	std::string m_buffer;

	DECLARE_EVENT_TABLE();
};


#endif // SPRINGLOBBY_HEADERGUARD_SOCKET_H
