/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SNARLNOTIFICATION_H
#define SNARLNOTIFICATION_H

#ifdef __WXMSW__

#include "inotification.h"
#include "snarl/SnarlInterface.h"

class ToasterNotification;

class SnarlNotification : public INotification
{
	public:
		SnarlNotification(wxWindow* parent);
		virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data );

	protected:
		ToasterNotification* m_fallback_wrapper;
		Snarl::V39::SnarlInterface m_snarl_interface;

};

#endif // __WXMSW__
#endif // SNARLNOTIFICATION_H

