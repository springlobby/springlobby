/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#include "flagimages.h"

#include <wx/bitmap.h>
#include <wx/imaglist.h>
#include <wx/log.h>
#include <map>

#include "flagimagedata.h"

typedef std::map<std::string, int> NameToPosMap;
static NameToPosMap m_name_to_pos;

int GetFlagIndex(const std::string& flag)
{
	if ((flag.empty()) ||
	    (flag == "??") || // unknown
	    (flag == "XX") || // not sure where this come from, very likely from incomplete bootstrap at login
	    (flag == "A1") || // anonymous proxy
	    (flag == "A2") || // satellite provider
	    (flag == "O1"))   // other country
		return FLAG_NONE;

	NameToPosMap::const_iterator itor = m_name_to_pos.find(flag);
	if (itor != m_name_to_pos.end())
		return itor->second;

	wxLogMessage(_T( "%s flag not found!" ), flag.c_str());
	return FLAG_NONE;
}

int AddFlagImages(wxImageList& imgs)
{
	int poszero = 0;
	for (int i = 0; flag_xpm[i]; ++i) {
		const int index = imgs.Add(wxBitmap(const_cast<const char**>(flag_xpm[i])));
		if (i == 0)
			poszero = index;
		m_name_to_pos[flag_str[i]] = i;
	}
	return poszero;
}
