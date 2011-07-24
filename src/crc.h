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
	bool UpdateFile(const std::string& filename);

	void ResetCRC();

	unsigned int GetCRC() const { return crc ^ 0xFFFFFFFF; }

private:
	static unsigned int crcTable[256];
	static void GenerateCRCTable();

	unsigned int crc;
};

#endif // !CRC_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

