/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

TODO: remove all wx stuff & merge / move to

lsl/networking/socket.cpp

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
**/
#include "socket.h"

#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/convauto.h>
#include <wx/log.h>
#include <wx/socket.h>
#include <wx/string.h>
#include <algorithm>
#include <stdexcept>

#ifdef SSL_SUPPORT
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#endif

#include "address.h"
#include "inetclass.h"
#include "utils/conversion.h"

std::string _GetHandle()
{
	std::vector<unsigned char> mac;
	std::string res;
	if (GetMac(mac)) {
		res.assign(mac.begin(), mac.end());
		wxLogDebug(wxString::Format("Found mac: %s", MacToString(mac).c_str()));
		return res;
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

{

#ifdef SSL_SUPPORT
	m_verified = false;
	m_sslctx = nullptr;
	m_ssl = nullptr;
#endif
}

#ifdef SSL_SUPPORT

// http://roxlu.com/2014/042/using-openssl-with-memory-bios

void Socket::StopTLS()
{
	m_starttls = false;
	m_verified = false;
	if (m_ssl != nullptr) {
		SSL_free(m_ssl);
		m_ssl = nullptr;
		SSL_CTX_free(m_sslctx);
		m_sslctx = nullptr;
	}
	ERR_free_strings();
	EVP_cleanup();
}

void Socket::DoSSLHandshake()
{
	const int ret = SSL_do_handshake(m_ssl);
	wxLogDebug("SSL_do_handshake(): ret %d", ret);
	if (ret < 0) { // fatal error
		const int r = SSL_get_error(m_ssl, ret);
		if (SSL_ERROR_WANT_READ == r) {
			wxLogDebug("SSL_ERROR_WANT_READ");
			int pending = BIO_ctrl_pending(m_outbio);
			if (pending > 0) {
				wxLogDebug("SSL_do_handshake(): m_outbio pending == %d", pending);
				char buf[4096];
				const int read = BIO_read(m_outbio, buf, sizeof(buf));
				if (read > 0) {
					wxLogDebug("BIO_read() m_outbio: %d", read);
					m_sock.Write(buf, read);
				}
			}
			pending = BIO_ctrl_pending(m_inbio);
			if (pending > 0) {
				wxLogWarning("SSL_do_handshake(): m_inbio pending == %d", pending);
			}

		} else {
			wxLogError("SSL_get_error(): %d", r);
			Disconnect();
		}
	} else if (ret == 2) { //controlled shutdown
	} // ret == 1 is successful
}

void Socket::StartTLS(const std::string& fingerprint)
{
	wxLogMessage("Starting TLS... %s", fingerprint.c_str());
	m_excepted_fingerprint = fingerprint;
	assert(!m_starttls);
	m_starttls = true;
	SSL_load_error_strings();
	SSL_library_init();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	m_sslctx = SSL_CTX_new(SSLv23_client_method());
	SSL_CTX_set_options(m_sslctx, SSL_OP_NO_SSLv3);
	m_ssl = SSL_new(m_sslctx);
	SSL_set_connect_state(m_ssl);
	m_inbio = BIO_new(BIO_s_mem());
	m_outbio = BIO_new(BIO_s_mem());

	BIO_set_mem_eof_return(m_inbio, -1);
	BIO_set_mem_eof_return(m_outbio, -1);
	SSL_set_bio(m_ssl, m_inbio, m_outbio);

	DoSSLHandshake();

}

bool Socket::VerifyCertificate()
{
	if (m_verified) {
		return true;
	}

	X509* cert = SSL_get_peer_certificate(m_ssl);
	if (cert == nullptr) {
		return false;
	}

	const EVP_MD* fprint_type = EVP_sha256();
	unsigned char fprint[EVP_MAX_MD_SIZE];
	unsigned int fprint_size = 0;

	if (!X509_digest(cert, fprint_type, fprint, &fprint_size)) {
		return false;
	}

	std::string fingerprint;
	char buf[4];
	for (size_t i = 0; i < fprint_size; i++) {
		snprintf(buf, sizeof(buf), "%02x", fprint[i]);
		fingerprint += buf[0];
		fingerprint += buf[1];
	}
	m_fingerprint = fingerprint;
	wxLogMessage("Expecting fingerprint:   %s", m_excepted_fingerprint.c_str());
	wxLogMessage("Certificate fingerprint: %s", m_fingerprint.c_str());

	const char* version = SSL_get_version(m_ssl);
	wxLogMessage("Using %s", version);

	const char* name = SSL_get_cipher_name(m_ssl);
	wxLogMessage("Using cipher %s", name);

	if (fingerprint != m_excepted_fingerprint) {
		m_net_class.OnInvalidFingerprintReceived(fingerprint, m_excepted_fingerprint);
		return false;
	}

/* // we prefer certificate pinnig

	long res = SSL_get_verify_result(ssl);
	if(!(X509_V_OK == res)) {
	}
*/


	X509_free(cert);
	m_verified = true;
	return true;
}
#else
void Socket::StartTLS(const std::string& /*fingerprint*/)
{
	wxLogWarning("TLS requested but not supported!");
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
#ifdef SSL_SUPPORT
	if (m_starttls) {
		StopTLS();
	}
#endif
	const bool wasconnected = m_sock.IsConnected();
	m_buffer.clear();
	m_sock.Close();
	if (wasconnected) { //.Close() disables all events, fire it manually (as last to prevent recursions loops)
		m_net_class.OnDisconnected(wxSOCKET_NOERROR);
	}
}

//! @brief Send data over connection.
bool Socket::Send(const std::string& data)
{
	wxLogDebug("Socket::Send");
	m_buffer += (const char*)data.c_str();
	int crop = m_buffer.length();
	if (m_rate > 0) {
		int max = m_rate - m_sent;
		if (crop > 0)
			crop = max;
	}
	std::string send = m_buffer.substr(0, crop);
#ifdef SSL_SUPPORT
	if (m_starttls) {
		int res = 0;
		if (!SSL_is_init_finished(m_ssl)) {
			DoSSLHandshake();
		} else {
			if (!VerifyCertificate()) {
				wxLogWarning("Couldn't verify certificate, closing connection");
				Disconnect();
			}
		}

		if (send.length() > 0) {
			const int ret = SSL_write(m_ssl, send.c_str(), send.length());
			if (ret > 0) {
				res += ret;
			} else if (ret == 0) {
				wxLogDebug("SSL_write(); %d", ret);
			} else {
				wxLogWarning("SSL_write(); %d", ret);
			}
		}
		if (BIO_ctrl_pending(m_outbio) > 0) {
			char outbuf[4096];
			const int read = BIO_read(m_outbio, outbuf, sizeof(outbuf));
			assert(read > 0);
			m_sock.Write(outbuf, read);
		}
		m_buffer.erase(0, res);
		m_sent += res;
		return true;
	}
	//wxLogMessage( _T("send: %d  sent: %d  max: %d   :  buff: %d"), send.length() , m_sent, max, m_buffer.length() );
#endif
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

	if (len == 0) {
		return wxEmptyString;
	}
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
	wxLogDebug("Socket::Receive");
	wxString res;
	static const int chunk_size = 4096;
	char buf[chunk_size];
	int readnum = 0;

	do {
		m_sock.Read(buf, chunk_size);
		readnum = m_sock.LastCount();
#ifdef SSL_SUPPORT
		if (!m_starttls && (readnum == 0)) {
			return res;
		}
#endif
		wxLogDebug("Receive() %d", readnum);
#ifdef SSL_SUPPORT
		if (m_starttls) {
			if (readnum > 0) {
				const int written = BIO_write(m_inbio, buf, readnum);
				if (written <= 0) {
					wxLogDebug("BIO_write(): %d", written);
				}
			}
			if (!SSL_is_init_finished(m_ssl)) {
				DoSSLHandshake();
			} else {
				if (!VerifyCertificate()) {
					wxLogWarning("Couldn't verify certificate, closing connection");
					Disconnect();
					return wxEmptyString;
				}
				int ret = 0;
				do {
					ret = SSL_read(m_ssl, buf, chunk_size);
					if (ret >= 0) {
						const std::string str(buf, ret);
						res += convert(buf, ret);
					} else if (ret == 0) {
						wxLogWarning("SSL_read(); %d", ret);
					} else {
						wxLogDebug("SSL_read(): %d", ret);
					}
				} while (ret > 0);
			}
		} else {
#endif
			res += convert(buf, readnum);
#ifdef SSL_SUPPORT
		}
#endif
	} while (readnum > 0);

	return res;
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
