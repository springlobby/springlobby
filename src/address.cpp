/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "address.h"

#ifdef WIN32
#include <iphlpapi.h>
#elif defined(linux)
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#ifdef WIN32

bool GetMacType(std::vector<unsigned char>& mac, const unsigned int mactype)
{
	IP_ADAPTER_INFO AdapterInfo[16];      // Allocate information for 16 cards
	DWORD dwBufLen = sizeof(AdapterInfo); // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen); // Get info
	if (dwStatus == ERROR_BUFFER_OVERFLOW) {
		wxLogError(wxString::Format("To small buffer size: %d", dwStatus));
		return false;
	}
	if (dwStatus != NO_ERROR)
		return false; // Check status

	for (size_t i = 0; i < dwBufLen / sizeof(AdapterInfo); i++) {
		if ((mactype != 0) && (AdapterInfo[i].Type != mactype)) //skip not wanted type
			continue;
		if (AdapterInfo[i].AddressLength == 0) {
			wxLogWarning("Zero Address length for adapter");
			continue;
		}
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


bool GetMac(std::vector<unsigned char>& mac)
{
	if (GetMacType(mac, MIB_IF_TYPE_ETHERNET))
		return true;
	if (GetMacType(mac, IF_TYPE_IEEE80211))
		return true;
	return (GetMacType(mac, 0));
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

