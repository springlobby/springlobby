/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CRC_H
#define CRC_H

#include <string>
/** @brief Object representing an updateable CRC-32 checksum. */
class CRC
{
public:
	CRC();

	void UpdateData(const unsigned char* buf, unsigned bytes);
	void UpdateData(const std::string& buf);
	void ResetCRC();

	unsigned int GetCRC() const
	{
		return crc ^ 0xFFFFFFFF;
	}

private:
	static unsigned int crcTable[256];
	static void GenerateCRCTable();

	unsigned int crc;
};

#endif // !CRC_H
