/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef TOASTERNOTIFICATION_H
#define TOASTERNOTIFICATION_H

#include "inotification.h"
class ToasterBox;

class ToasterNotification : public INotification
{
public:
	explicit ToasterNotification(wxWindow* parent);
	virtual ~ToasterNotification();
	virtual void Show(const wxBitmap& icon, const size_t pos, const UiEvents::NotficationData& data);

private:
	void SetPopupPosition(const size_t pos);
	ToasterBox* m_toasterbox;
	int m_width, m_height, m_x_offset, m_y_offset;
};

#endif // TOASTERNOTIFICATION_H
