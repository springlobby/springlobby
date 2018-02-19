/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE lobbyid
#include <boost/test/unit_test.hpp>

#include <stdio.h>
#include "utils/crc.h"
#include "address.h"

BOOST_AUTO_TEST_CASE(lobbyid)
{

	const int count = 3;
	const unsigned char mac[count][6] = {
	    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05},
	    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
	};

	const unsigned int c[count] = {
	    820760394,
	    2982322595,
	    1104801024};

	CRC m_crc;
	for (int i = 0; i < count; i++) {
		m_crc.ResetCRC();
		m_crc.UpdateData(mac[i], 6);
		printf("Checksum of the mac ");
		for (int j = 0; j < 6; j++) {
			if (j > 0)
				printf(":");
			printf("%02x", (int)(mac[i][j]));
		}
		printf(" is %u\n", m_crc.GetCRC());
		BOOST_CHECK(m_crc.GetCRC() == c[i]);
	}

	m_crc.ResetCRC();
	m_crc.UpdateData("The quick brown fox jumps over the lazy dog");
	BOOST_CHECK(m_crc.GetCRC() == 1095738169); // == 414fa339, http://rosettacode.org/wiki/CRC-32#C.2B.2B
}

BOOST_AUTO_TEST_CASE(getmac)
{
	std::vector<unsigned char> mac;
	GetMac(mac);
	const std::string smac = MacToString(mac);
	printf("%s\n", smac.c_str());
}
