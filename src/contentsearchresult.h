/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef CONTENTSEARCHRESULT_H
#define CONTENTSEARCHRESULT_H
#include <wx/string.h>
#include <wx/image.h>
#include "downloader/lib/src/Downloader/DownloadEnum.h"
class ContentSearchResult
{
public:
	ContentSearchResult();
	~ContentSearchResult();
	wxString name;
	DownloadEnum::Category category;
	wxString type;
	wxImage* image;
	unsigned int filesize;
	bool is_downloaded;
};

#endif // CONTENTSEARCHRESULT_H
