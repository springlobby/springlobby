/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SPRINGLOBBY_LIBNOTIFY_H
#define SPRINGLOBBY_LIBNOTIFY_H

#ifdef HAVE_LIBNOTIFY

#include "inotification.h"
#include "settings.h"
#include "utils/version.h"

class LibnotifyNotification : public INotification
{
public:
	LibnotifyNotification(wxWindow* parent);
	virtual ~LibnotifyNotification();
	virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data);
};


#include <libnotify/notify.h>

LibnotifyNotification::LibnotifyNotification(wxWindow*)
{
}
LibnotifyNotification::~LibnotifyNotification()
{
}

void LibnotifyNotification::Show(const wxBitmap& icon, const size_t /*pos*/, const UiEvents::NotficationData& data)
{
	NotifyNotification* n;
	notify_init("Test");
#if !defined(NOTIFY_VERSION_MINOR) || (NOTIFY_VERSION_MAJOR == 0 && NOTIFY_VERSION_MINOR < 7)
	n = notify_notification_new(getSpringlobbyName().c_str(), data.second.mb_str(), NULL, NULL);
#else
	n = notify_notification_new(getSpringlobbyName().c_str(), data.second.mb_str(), NULL);
#endif
	notify_notification_set_timeout(n, sett().GetNotificationPopupDisplayTime() * 1000);

#ifndef __APPLE__ //FIXME: apple has no icon.GetPixbuf(), see #258
	notify_notification_set_icon_from_pixbuf(n, icon.GetPixbuf());
#endif
	if (!notify_notification_show(n, NULL)) {
		//	   g_error("Failed to send notification.\n");
	}
	g_object_unref(G_OBJECT(n));
}

#endif // HAVE_LIBNOTIFY
#endif // SPRINGLOBBY_LIBNOTIFY_H
