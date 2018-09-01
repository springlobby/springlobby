/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
#define SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H

#include <wx/aui/aui.h>
class AuiManagerContainer
{
public:
	AuiManagerContainer()
	    : manager(NULL)
	{
	}

	wxAuiManager* manager;
};

AuiManagerContainer& GetAui();


#endif // SPRINGLOBBY_HEADERGUARD_AUIMANAGER_H
