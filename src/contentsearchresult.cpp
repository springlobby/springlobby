/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "contentsearchresult.h"

ContentSearchResult::ContentSearchResult()
    : image(NULL)
    , filesize(0)
    , is_downloaded(false)
{
}

ContentSearchResult::~ContentSearchResult()
{
	if (image != NULL) {
		delete image;
	}
}
