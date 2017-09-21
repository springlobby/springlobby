/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/networking/socket.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/

#include <wx/socket.h>
#include <wx/string.h>
#include <wx/convauto.h>
#include <wx/log.h>

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER


#include <stdexcept>
#include <algorithm>

#include "socket.h"
#include "inetclass.h"
#include "utils/conversion.h"

#ifdef __WXMSW__
#include <iphlpapi.h>
#elif defined(linux)
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#define SSL_SUPPORT 1
#ifdef SSL_SUPPORT
//#include <openssl/applink.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#endif

#ifdef __WXMSW__

bool GetMac(std::vector<unsigned char>& mac)
{
	IP_ADAPTER_INFO AdapterInfo[16];      // Allocate information for 16 cards
	DWORD dwBufLen = sizeof(AdapterInfo); // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen); // Get info
	if (dwStatus != NO_ERROR)
		return false; // Check status
	for (size_t i = 0; i < sizeof(AdapterInfo); i++) {
		mac.resize(AdapterInfo[i].AddressLength);
		mac.assign(AdapterInfo[i].Address, AdapterInfo[i].Address + AdapterInfo[i].AddressLength);
		for (size_t j = 0; j < mac.size(); j++) {
			if (mac[j] != 0) {
				return true;
			}
		}
	}
	return false;
}

#elif defined(__APPLE__)

bool GetMac(std::vector<unsigned char>& mac)
{
	//FIXME: implement this, http://lists.freebsd.org/pipermail/freebsd-hackers/2004-June/007415.html
	return false;
}

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <ifaddrs.h>

bool GetMac(std::vector<unsigned char>& mac)
{
	ifaddrs* ifap = 0;
	if (getifaddrs(&ifap) == 0) {
		ifaddrs* iter = ifap;
		while (iter) {
			sockaddr_ll* sal = reinterpret_cast<sockaddr_ll*>(iter->ifa_addr);
			if (sal->sll_family == AF_PACKET) {
				mac.resize(sal->sll_halen);
				mac.assign(sal->sll_addr, sal->sll_addr + sal->sll_halen);
				for (size_t i = 0; i < mac.size(); i++) {
					if (mac[i] != 0) {
						freeifaddrs(ifap);
						return true;
					}
				}
			}
			iter = iter->ifa_next;
		}
		freeifaddrs(ifap);
	}
	return false;
}

#endif

std::string MacToString(std::vector<unsigned char>& mac)
{
	std::string res;
	for (size_t i = 0; i < mac.size(); i++) {
		char buf[3];
		snprintf(buf, sizeof(buf), "%02X", mac[i]);
		if (!res.empty())
			res += ":";
		res.append(buf, 2);
	}
	return res;
}

std::string _GetHandle()
{
	std::vector<unsigned char> mac;
	std::string res;
	if (GetMac(mac)) {
		res.assign(mac.begin(), mac.end());
		wxLogDebug(_T("Found mac: %s"), TowxString(MacToString(mac)).c_str());
		return std::string(mac.begin(), mac.end());
	}
	return res;
}

BEGIN_EVENT_TABLE(Socket, wxEvtHandler)
EVT_SOCKET(SOCKET_ID, Socket::OnSocketEvent)
END_EVENT_TABLE()


void Socket::OnSocketEvent(wxSocketEvent& event)
{
	switch (event.GetSocketEvent()) {
		case wxSOCKET_INPUT:
			m_net_class.OnDataReceived();
			break;
		case wxSOCKET_LOST:
			m_net_class.OnDisconnected(m_sock.LastError());
			m_sock.Close();
			break;
		case wxSOCKET_CONNECTION:
			m_net_class.OnConnected();
			break;
		default:
			m_net_class.OnError("Unknown socket event.");
			m_sock.Close();
			break;
	}
}


//! @brief Constructor
Socket::Socket(iNetClass& netclass)
    : m_handle(_GetHandle())
    , m_connecting(false)
    , m_net_class(netclass)
    , m_rate(-1)
    , m_sent(0)
    , m_starttls(false)
    , m_sslctx(nullptr)
    , m_ssl(nullptr)

{
}

#ifdef SSL_SUPPORT

// http://roxlu.com/2014/042/using-openssl-with-memory-bios

void InitializeSSL()
{
	SSL_load_error_strings();
	SSL_library_init();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
}

void DestroySSL()
{
    ERR_free_strings();
    EVP_cleanup();
}

void ShutdownSSL(SSL *ssl)
{
    SSL_shutdown(ssl);
    SSL_free(ssl);
}

void Socket::StartTLS()
{
	wxLogMessage("Starting TLS...");
	m_starttls = true;
	InitializeSSL();
	m_sslctx = SSL_CTX_new(SSLv23_client_method());
	SSL_CTX_set_options(m_sslctx, SSL_OP_NO_SSLv3);
	m_ssl = SSL_new(m_sslctx);
	SSL_set_connect_state(m_ssl);
	m_inbio = BIO_new(BIO_s_mem());
	m_outbio = BIO_new(BIO_s_mem());

	BIO_set_mem_eof_return(m_inbio, -1);
	BIO_set_mem_eof_return(m_outbio, -1);
	SSL_set_bio(m_ssl, m_inbio, m_outbio);

	Send("");
}


#endif


//! @brief Destructor
Socket::~Socket()
{
	Disconnect();
}


//! @brief Creates an TCP socket and sets it up.
void Socket::InitSocket(wxSocketClient& sock)
{
	sock.SetFlags(wxSOCKET_NOWAIT);
	sock.SetEventHandler(*this, SOCKET_ID);
	sock.SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	sock.Notify(true);
}

//! @brief Connect to remote host.
void Socket::Connect(const wxString& addr, const int port)
{
	wxIPV4address wxaddr;
	m_connecting = true;
	m_buffer = "";

	if (!wxaddr.Hostname(addr)) {
		m_net_class.OnError("Invalid Hostname");
		return;
	}
	if (!wxaddr.Service(port)) {
		m_net_class.OnError("Invalid Port");
		return;
	}

	InitSocket(m_sock);


	m_sock.Connect(wxaddr, false);
	m_sock.SetTimeout(40);
}

void Socket::SetTimeout(const int seconds)
{
	m_sock.SetTimeout(seconds);
}

//! @brief Disconnect from remote host if connected.
void Socket::Disconnect()
{
	const bool wasconnected = m_sock.IsConnected();
	m_buffer.clear();
	m_sock.Close();
	if (wasconnected) { //.Close() disables all events, fire it manually (as last to prevent recursions loops)
		m_net_class.OnDisconnected(wxSOCKET_NOERROR);
	}
	if (m_starttls) {
		ShutdownSSL(m_ssl);
	}
}

//! @brief Send data over connection.
bool Socket::Send(const std::string& data)
{
	wxLogWarning("Socket::Send");
	m_buffer += (const char*)data.c_str();
	int crop = m_buffer.length();
	if (m_rate > 0) {
		int max = m_rate - m_sent;
		if (crop > 0)
			crop = max;
	}
	std::string send = m_buffer.substr(0, crop);

	if (m_starttls) {
		if(!SSL_is_init_finished(m_ssl)) {
			SSL_do_handshake(m_ssl);
		}
		const int res = SSL_write(m_ssl, send.c_str(), send.length());
		if (BIO_ctrl_pending(m_outbio) > 0) {
			char outbuf[4096];
			int read = BIO_read(m_outbio, outbuf, sizeof(outbuf));
			assert(read > 0);
			m_sock.Write(outbuf, read);
		}
		m_buffer.erase(0, res);
		m_sent += res;
		return true;
	}
	//wxLogMessage( _T("send: %d  sent: %d  max: %d   :  buff: %d"), send.length() , m_sent, max, m_buffer.length() );

	m_sock.Write(send.c_str(), send.length());
	if (m_sock.Error()) {
		return false;
	}
	wxUint32 sentdata = m_sock.LastCount();
	m_buffer.erase(0, sentdata);
	m_sent += sentdata;
	return true;
}


wxString convert(char* buff, const int len)
{
	wxString ret = wxString(buff, wxConvUTF8, len);
	if (!ret.IsEmpty()) {
		return ret;
	}
	ret = wxString(buff, wxConvLibc, len);
	if (!ret.empty()) {
		return ret;
	}
	ret = wxString(buff, wxConvLocal, len);
	if (!ret.IsEmpty()) {
		return ret;
	}
	ret = wxString(buff, wxConvISO8859_1, len);
	if (!ret.empty()) {
		return ret;
	}
	ret = wxString(buff, wxConvAuto(), len);
	if (!ret.empty()) {
		return ret;
	}
	std::string tmp(buff, len);
	wxLogWarning(_T("Error: invalid charset, replacing invalid chars: '%s'"), TowxString(tmp).c_str());

	//worst case, couldn't convert, replace unknown chars!
	for (int i = 0; i < len; i++) {
		if ((buff[i] < '!') || (buff[i] > '~')) {
			buff[i] = '_';
		}
	}
	ret = wxString(buff, wxConvUTF8, len);
	if (!ret.empty()) {
		return ret;
	}
	wxLogWarning(_T("Fatal Error: couldn't convert: '%s'"), TowxString(tmp).c_str());
	return wxEmptyString;
}

//! @brief Receive data from connection
wxString Socket::Receive()
{
	wxLogWarning("Socket::Receive");
	wxString ret;
	static const int chunk_size = 4096;
	char buf[chunk_size];
	int readnum = 0;

	do {
		m_sock.Read(buf, chunk_size);
		readnum = m_sock.LastCount();
		wxLogWarning("Receive() %d", readnum);
		if (readnum == 0) { return ret; }

		if (m_starttls) {
			BIO_write(m_inbio, buf, readnum);
			if(!SSL_is_init_finished(m_ssl)) {
				SSL_do_handshake(m_ssl);
				Send("");
			} else {
				int decodedbytes = 0;
				do {
				decodedbytes = SSL_read(m_ssl, buf, chunk_size);
				if (decodedbytes >= 0) {
					const std::string str(buf, decodedbytes);
					ret += convert(buf, decodedbytes);
				}
				} while(decodedbytes > 0);
			}
		} else {
			ret += convert(buf, readnum);
		}
	} while (readnum > 0);

	return ret;
}

//! @brief Get curent socket state
SockState Socket::State()
{
	if (m_sock.IsConnected()) {
		m_connecting = false;
		return SS_Open;
	} else {
		if (m_connecting) {
			return SS_Connecting;
		} else {
			return SS_Closed;
		}
	}
}


//! @brief Get socket error code
//! @todo Implement
SockError Socket::Error() const
{
	return (SockError)-1;
}


//! @brief used to retrieve local ip address behind NAT to communicate to the server on login
std::string Socket::GetLocalAddress() const
{
	if (!m_sock.IsConnected())
		return "";

	wxIPV4address localaddr;
	m_sock.GetLocal(localaddr);

	return STD_STRING(localaddr.IPAddress());
}


//! @brief Set the maximum upload ratio.
void Socket::SetSendRateLimit(int Bps)
{
	m_rate = Bps;
}


void Socket::Update(int mselapsed)
{
	if (m_rate > 0) {
		m_sent -= int((mselapsed / 1000.0) * m_rate);
		if (m_sent < 0)
			m_sent = 0;
		if (m_buffer.length() > 0) {
			Send("");
		}
	} else {
		m_sent = 0;
	}
}
