/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE lobbyid
#include <boost/test/unit_test.hpp>

#include "crc.h"


BOOST_AUTO_TEST_CASE( lobbyid )
{
	const unsigned char mac [6] = {0x00,0x01,0x02,0x03,0x04,0x05};
	const std::string data(mac, mac+6);
	CRC m_crc;
	m_crc.UpdateData(data + ":lobby.springrts.com");

	BOOST_CHECK(m_crc.GetCRC() == 1147635839);

	m_crc.ResetCRC();
	m_crc.UpdateData("The quick brown fox jumps over the lazy dog");
	BOOST_CHECK(m_crc.GetCRC() == 1095738169); // == 414fa339, http://rosettacode.org/wiki/CRC-32#C.2B.2B
}
