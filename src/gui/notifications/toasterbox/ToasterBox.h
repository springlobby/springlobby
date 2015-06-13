/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SL_TOASTERBOX_HH
#define SL_TOASTERBOX_HH

#include "ToasterBoxWindow.h"
#include "ToasterBoxWindowList.h"
#include "utils/mixins.h"
#include "utils/uievents.h"
/*
  The toasterbox class should stay resident in memory.
  It creates and displays popups and handles the
  "stacking".
*/

class ToasterNotification;

class ToasterBox : public wxTimer
{
public:
	~ToasterBox();
	void SetPopupData(const UiEvents::NotficationData& data, bool /*_shrink*/ = false)
	{
		notificationType = data.first;
		popupText = data.second;
	}
	void SetPopupSize(int x, int y)
	{
		popupSize = wxSize(x, y);
	}
	void SetPopupPosition(int x, int y);
	void SetPopupPosition(int pos);
	void SetPopupPauseTime(int milliseconds)
	{
		pauseTime = milliseconds;
	}
	void SetPopupBitmap(wxString _bitmapFile)
	{
		bitmapFile = _bitmapFile;
	}
	void SetPopupBitmap(const wxBitmap& bitmap)
	{
		m_bitmap = bitmap;
	}
	void SetPopupBackgroundColor(int r, int g, int b);
	void SetPopupTextColor(int r, int g, int b);
	void SetPopupScrollSpeed(int _sleepTime)
	{
		sleepTime = _sleepTime;
	}
	void MoveAbove(ToasterBoxWindow* tb);
	wxString GetPopupText()
	{
		return popupText;
	}
	void Play();
	bool DoesTextFit();
	void Notify();
	void CleanList();
	void StartAll(bool start = true);
	enum StackDirection {
		StackUp = -1,
		StackDown = 1
	};
	void SetStackDirection(StackDirection dir);

private:
	friend class ToasterNotification;
	ToasterBox(wxWindow* _parent = (wxWindow*)NULL);

	UiEvents::EventType notificationType; //used to determine type of notification to show

	wxWindow* parent;
	int sleepTime;
	//how long the box hangs around for
	int pauseTime;
	wxString popupText;
	wxPoint bottomRight, popupTop, popupPosition;
	wxSize popupSize;
	wxStaticBitmap sbm;
	wxColour colFg, colBg;
	wxString bitmapFile;
	ToasterBoxWindowList* winList;
	wxBitmap m_bitmap;
	//should we attempt to shrink the text
	//if it's too big for the popup?
	//    bool shrink;
	StackDirection m_stack_direction;
};

#endif //SL_TOASTERBOX_HH
