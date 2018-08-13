/* This file is part of the Springlobby (GPL v2 or later), see COPYING */
#ifndef SPRINGLOBBY_LIBNOTIFY_H
#define SPRINGLOBBY_LIBNOTIFY_H

#ifdef HAVE_LIBNOTIFY
#include "libnotify.h"

#include <libnotify/notify.h>
#include <wx/bitmap.h>
#include <wx/log.h>

#include "settings.h"
#include "utils/version.h"

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
	n = notify_notification_new(GetSpringlobbyName().c_str(), data.second.mb_str(), NULL, NULL);
#else
	n = notify_notification_new(GetSpringlobbyName().c_str(), data.second.mb_str(), NULL);
#endif
	notify_notification_set_timeout(n, sett().GetNotificationPopupDisplayTime() * 1000);

#ifndef __APPLE__ //FIXME: apple has no icon.GetPixbuf(), see #258
	notify_notification_set_icon_from_pixbuf(n, icon.GetPixbuf());
#endif
	if (!notify_notification_show(n, NULL)) {
		wxLogWarning(_T("Failed to send notification."));
	}
	g_object_unref(G_OBJECT(n));
}

#endif // HAVE_LIBNOTIFY
#endif // SPRINGLOBBY_LIBNOTIFY_H
