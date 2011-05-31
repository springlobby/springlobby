/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "FileHandler.h"

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <limits.h>
#include <boost/regex.hpp>

#include <QFileInfo>

using std::string;

/******************************************************************************/
/******************************************************************************/

CFileHandler::CFileHandler(const char* filename, const char* modes)
: ifs(NULL), filePos(0), fileSize(-1)
{
	Init(filename, modes);
}


CFileHandler::CFileHandler(const string& filename, const string& modes)
: ifs(NULL), filePos(0), fileSize(-1)
{
	Init(filename, modes);
}


CFileHandler::~CFileHandler(void)
{
	delete ifs;
}


/******************************************************************************/

bool CFileHandler::TryRawFS(const string& filename)
{
	const string rawpath = filename;//filesystem.LocateFile(filename);
	ifs = new std::ifstream(rawpath.c_str(), std::ios::in | std::ios::binary);
	if (ifs && !ifs->bad() && ifs->is_open()) {
		ifs->seekg(0, std::ios_base::end);
		fileSize = ifs->tellg();
		ifs->seekg(0, std::ios_base::beg);
		return true;
	}
	delete ifs;
	ifs = NULL;
	return false;
}

void CFileHandler::Init(const string& _filename, const string& /*modes*/)
{
	filename = _filename;
	if (TryRawFS(filename)){ return; }
}

/******************************************************************************/

bool CFileHandler::FileExists() const
{
	return (fileSize >= 0);
}


int CFileHandler::Read(void* buf,int length)
{
	if (ifs) {
		ifs->read((char*)buf, length);
		return ifs->gcount ();
	}
	else if (!fileBuffer.empty()) {
		if ((length + filePos) > fileSize) {
			length = fileSize - filePos;
		}
		if (length > 0) {
			assert(int(fileBuffer.size()) >= filePos+length);
			memcpy(buf, &fileBuffer[filePos], length);
			filePos += length;
		}
		return length;
	}

	return 0;
}


void CFileHandler::Seek(int length, std::ios_base::seekdir where)
{
	if (ifs)
	{
		// on some machines, EOF bit does not get reset when seeking to another pos
		ifs->clear();
		ifs->seekg(length, where);
	}
	else if (!fileBuffer.empty())
	{
		if (where == std::ios_base::beg)
		{
			filePos = length;
		}
		else if (where == std::ios_base::cur)
		{
			filePos += length;
		}
		else if (where == std::ios_base::end)
		{
			filePos = fileSize + length;
		}
	}
}


int CFileHandler::Peek() const
{
	if (ifs) {
		return ifs->peek();
	}
	else if (!fileBuffer.empty()){
		if (filePos < fileSize) {
			return fileBuffer.at(filePos);
		} else {
			return EOF;
		}
	}
	return EOF;
}


bool CFileHandler::Eof() const
{
	if (ifs) {
		return ifs->eof();
	}
	if (!fileBuffer.empty()) {
		return (filePos >= fileSize);
	}
	return true;
}


int CFileHandler::FileSize() const
{
   return fileSize;
}


int CFileHandler::GetPos() const
{
	if (ifs) {
		return ifs->tellg();
	} else {
		return filePos;
	}
}


bool CFileHandler::LoadStringData(string& data)
{
	if (!FileExists()) {
		return false;
	}
	char* buf = new char[fileSize];
	Read(buf, fileSize);
	data.append(buf, fileSize);
	delete[] buf;
	return true;
}

std::string CFileHandler::GetFileExt() const
{
	return QFileInfo( QString(filename.c_str()) ).suffix().toStdString();
}
