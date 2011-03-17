#ifndef SPRINGLOBBY_LIBNOTIFY_H
#define SPRINGLOBBY_LIBNOTIFY_H

#include "inotification.h"

class LibnotifyNotification : public INotification
{
	public:
		LibnotifyNotification(wxWindow* parent);
		virtual ~LibnotifyNotification();
		virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data );
};


#include <libnotify/notify.h>

#ifndef NOTIFY_CHECK_VERSION
	#ifndef NOTIFY_VERSION_MAJOR
		#define NOTIFY_CHECK_VERSION(major,minor,micro) 0
	#else
		#define NOTIFY_CHECK_VERSION(major,minor,micro) \
			(NOTIFY_VERSION_MAJOR > (major) || \
			(NOTIFY_VERSION_MAJOR == (major) && NOTIFY_VERSION_MINOR > (minor)) || \
			(NOTIFY_VERSION_MAJOR == (major) && NOTIFY_VERSION_MINOR == (minor) && \
			NOTIFY_VERSION_MICRO >= (micro)))
	#endif
#endif

LibnotifyNotification::LibnotifyNotification(wxWindow* ){}
LibnotifyNotification::~LibnotifyNotification(){}

void LibnotifyNotification::Show(const wxBitmap& icon, const size_t /*pos*/, const UiEvents::NotficationData& data )
{
	NotifyNotification *n;
	notify_init("Test");
	#if NOTIFY_CHECK_VERSION(0,7,0)
		n = notify_notification_new ( GetAppName().mb_str(),data.second.mb_str(), NULL );
	#else
		n = notify_notification_new ( GetAppName().mb_str(),data.second.mb_str(), NULL, NULL );
	#endif
	notify_notification_set_timeout (n, sett().GetNotificationPopupDisplayTime()*1000);

	notify_notification_set_icon_from_pixbuf(n,icon.GetPixbuf() );
	if (!notify_notification_show (n, NULL)) {
//	   g_error("Failed to send notification.\n");

	}
	g_object_unref(G_OBJECT(n));
}

#endif // SPRINGLOBBY_LIBNOTIFY_H

/**
	This file is part of SpringLobby,
	Copyright (C) 2007-2010

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

