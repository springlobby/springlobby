/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "inotification.h"
class LibnotifyNotification : public INotification
{
public:
	LibnotifyNotification(wxWindow* parent);
	virtual ~LibnotifyNotification();
	virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data);
};
