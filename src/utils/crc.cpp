/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "crc.h"

unsigned int CRC::crcTable[256];


/** @brief Generate the lookup table used for CRC calculation.
    Code taken from http://paul.rutgers.edu/~rhoads/Code/crc-32b.c */
void CRC::GenerateCRCTable()
{
	unsigned int crc, poly;
	int i, j;

	poly = 0xEDB88320L;
	for (i = 0; i < 256; i++) {
		crc = i;
		for (j = 8; j > 0; j--) {
			if (crc & 1)
				crc = (crc >> 1) ^ poly;
			else
				crc >>= 1;
		}
		crcTable[i] = crc;
	}
}


/** @brief Construct a new CRC object.
    This generates the CRC table if it is the first CRC object being
	constructed. */
CRC::CRC()
    : crc(0xFFFFFFFF)
{
	if (crcTable[1] == 0)
		GenerateCRCTable();
}


/** @brief Update CRC over the data in buf. */
void CRC::UpdateData(const unsigned char* buf, unsigned bytes)
{
	for (size_t i = 0; i < bytes; ++i)
		crc = (crc >> 8) ^ crcTable[(crc ^ (buf[i])) & 0xFF];
}

/** @brief Resets CRC data to original state. */
void CRC::ResetCRC()
{
	crc = 0xFFFFFFFF; // reset crc
}


/** @brief Update CRC over the data in buf. */
void CRC::UpdateData(const std::string& buf)
{
	UpdateData((const unsigned char*)buf.c_str(), buf.size());
}
