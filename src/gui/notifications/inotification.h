/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef INOTIFICATION_H
#define INOTIFICATION_H

#include <cstddef>
#include "utils/uievents.h"
class wxWindow;
class wxBitmap;

class INotification
{
public:
	INotification()
	{
	}
	virtual ~INotification()
	{
	}
	virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data) = 0;
};

#endif // INOTIFICATION_H
