/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include <wx/dialog.h>
#include <wx/utils.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/dcclient.h>
#include <wx/region.h>
#include <wx/dcbuffer.h>
#include <wx/frame.h>
#include <wx/log.h>

#include "ToasterBoxWindow.h"
#include "images/notif_bg.png.h"
#include "gui/uiutils.h"

typedef wxClientDC DCType;

long ToasterBoxWindow::count = 0;

//BEGIN_EVENT_TABLE (ToasterBoxWindow, wxFrame)
//   EVT_KILL_FOCUS (ToasterBoxWindow::RejectFocus)
//   EVT_SET_FOCUS (ToasterBoxWindow::RejectFocus)
//END_EVENT_TABLE ()

ToasterBoxWindow::ToasterBoxWindow(wxWindow* parent, wxTimer* _parent2)
    : startTime(wxGetLocalTime())
    , parent2(_parent2)
    , sleepTime(10)
    , pauseTime(1700)
    , textColor(*wxWHITE)
    , headerTextColor(*wxBLUE)
    , popupText(_T("Change Me!"))
    , shrink(false)
{
	Create(parent);
	SetWindowStyleFlag(wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR);
	count++;
	//the size we want the dialog to be
	wxSize dialogSize(150, 170);
	bottomRight = wxPoint(wxGetDisplaySize().GetWidth(), wxGetDisplaySize().GetHeight());
	SetSize(bottomRight.x, bottomRight.y, dialogSize.GetWidth(), dialogSize.GetHeight());
//	ToasterBase::Connect(wxEVT_PAINT, (wxObjectEventFunction)&ToasterBoxWindow::OnPaint);
	SetBackgroundBitmap(charArr2wxBitmap(notif_bg_png, sizeof(notif_bg_png)));
}

ToasterBoxWindow::~ToasterBoxWindow()
{
}


void ToasterBoxWindow::SetPopupBitmap(wxBitmap& bitmap)
{
	sbm.Create(this, 0, bitmap, wxPoint(0, 0));
}

void ToasterBoxWindow::SetPopupBitmap(wxString _bitmapFile)
{
	if (!_bitmapFile.IsEmpty()) {
		wxBitmap bm = wxBitmap();
		bm.LoadFile(_bitmapFile, wxBITMAP_TYPE_BMP);
		if (bm.Ok())
			sbm.Create(this, 0, bm, wxPoint(0, 0));
	}
	//wxBitmap bm2 = wxBitmap();
	//bm2.LoadFile("back2.bmp", wxBITMAP_TYPE_BMP);
	//wxRegion reg(bm2);
	//wxLogDebug("%i", SetShape(*reg));
}


void ToasterBoxWindow::SetPopupSize(int x, int y)
{
	SetSize(bottomRight.x, bottomRight.y,
		x, y);
}

void ToasterBoxWindow::SetPopupPosition(int x, int y)
{
	dialogTop = wxPoint(x, y);
	bottomRight = wxPoint(dialogTop.x + GetSize().GetWidth(),
			      dialogTop.y + GetSize().GetHeight());
}


//_shrink indicates if we should to transformations on
//the text to make it "pretty"
void ToasterBoxWindow::SetPopupText(const wxString& _text, bool _shrink)
{
	popupText = _text;
	shrink = _shrink;
}

//activate the frame, raise it and set the timer
bool ToasterBoxWindow::Play()
{
	//do some checks to make sure this window is valid
	if (bottomRight.x < 1 || bottomRight.y < 1)
		return false;
	if (GetSize().GetWidth() < 50 || GetSize().GetWidth() < 50) {
		//toasterbox launches into a endless loop for some reason
		//when you try to make the window too small.
		return false;
	}
	ScrollUp();
	Start(pauseTime, true);
	return true;
}

//return to the user if their text all fit or not
bool ToasterBoxWindow::DoesTextFit()
{
	wxCoord w, h;
	wxClientDC dc(this);
	dc.GetMultiLineTextExtent(popupText, &w, &h);
	if (w > GetSize().GetWidth() || h > GetSize().GetHeight())
		return false;
	return true;
}

void ToasterBoxWindow::SetPopupBackgroundColor(int r, int g, int b)
{
	if (wxColor(r, g, b).Ok())
		SetBackgroundColour(wxColor(r, g, b));
	else
		wxLogDebug(_T("%s: %d,%d,%d"), _T("invalid color"), r, g, b);
}

void ToasterBoxWindow::SetPopupTextColor(int r, int g, int b)
{
	if (wxColor(r, g, b).Ok())
		textColor = wxColor(r, g, b);
}

//private functions
void ToasterBoxWindow::ScrollUp()
{
	wxLogDebug(_T("%s"), _T("Raising"));
	SetSize(dialogTop.x, dialogTop.y, GetSize().GetWidth(), GetSize().GetHeight());
	Show(true);
	//
	//
	//  //walk the Y value up in a raise motion
	//  int windowSize = 0;
	//  for(int i = bottomRight.y; i > dialogTop.y; i -= step)
	//  {
	//    if(i < dialogTop.y)
	//      i = dialogTop.y;
	//    windowSize += step;
	//    SetSize(dialogTop.x, i, GetSize().GetWidth(),
	//      windowSize);
	//	//Update();
	//	  wxMilliSleep(sleepTime);
	//  }

	//
	Update();
	DrawText();
	Update();
}

void ToasterBoxWindow::ScrollDown()
{
	wxLogDebug(_T("%s"), _T("Lowering"));
	//
	//  //walk down the Y value
	//  int windowSize = GetSize().GetHeight();
	//  for(int i = dialogTop.y; i < bottomRight.y; i += step)
	//  {
	//    if(i > bottomRight.y)
	//      i = bottomRight.y;
	//    windowSize -= step;
	//    SetSize(dialogTop.x, i, GetSize().GetWidth(),
	//      windowSize);
	//    //Update();
	//	  wxMilliSleep(sleepTime);
	//
	//  }
	Hide();
	if (parent2)
		parent2->Notify();
	else
		wxLogDebug(_T("%s"), _T("not valid parent"));
}

void ToasterBoxWindow::DrawText()
{
	DCType dc(this);
	//width and height of text
	wxCoord w = 0, h = 0;
	//where we will set the text
	wxCoord x = 0, y = 0;
	//border from sides and top to text (in pixels)
	int border_right = 7;
	int border_left = 102;
//	if (sbm.GetBitmap().IsOk()) {
//		border_left += sbm.GetBitmap().GetWidth();
//	}
	//how much space between text lines
	int textPadding = 4;
	//how much can we have on a line?
	int textColumns = 0;
	//how many lines can we fit vertically
	float textLines = 1;
	//the popupText after our transformations (if we do any)
	wxString pText = GetPopupText();
	wxFont outFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	dc.SetFont(outFont);

	dc.GetMultiLineTextExtent(pText, &w, &h);

	//shrink the text to fit in the popup box
	if (shrink) {
		while ((w + (border_right + border_left)) > GetSize().GetWidth()) {
			pText = pText.Left(pText.Length() - 1);
			dc.GetMultiLineTextExtent(pText, &w, &h);
		}
		textColumns = pText.Length();
		//figure out how many lines of text we have
		textLines = (GetPopupText().Length() / textColumns) + .9;
	}

	bool haveHeader = false;
	if (popupHeaderText.IsEmpty() == false) {
		textLines++;
		haveHeader = true;
	}
	//center the text
	//  if(w < GetSize().GetWidth())
	//    x = (GetSize().GetWidth() - w) / 2;
	//  if(h < GetSize().GetHeight())
	//    y = (GetSize().GetHeight() - h) / 2;


	//  //if we have multiple lines
	if (textLines > 1) {
		if (haveHeader) {
			//Draw header text with a shadow effect
			dc.SetTextForeground(*wxGREEN);
			//Try to center header text
			auto textSize = dc.GetTextExtent(popupHeaderText);
			int headerX = (GetSize().GetWidth() / 2) - (textSize.GetWidth() / 2);
			const int headerY = 3;
			dc.DrawText(popupHeaderText, headerX, headerY);
			dc.SetTextForeground(headerTextColor);
			dc.DrawText(popupHeaderText, headerX + 1, headerY + 1); //This makes "shadow" effect
		}
		dc.SetTextForeground(textColor);
		//how many lines we can fit in the height available
		float howManyLinesFit = GetSize().GetHeight() / (h + textPadding);
		if (textLines < howManyLinesFit)
			howManyLinesFit = textLines;
		y = (GetSize().GetHeight() - ((h + textPadding) * howManyLinesFit)) / 2;
		//Move text down a bit to get place for header
		if (haveHeader) {
			y += 15;
		}
		for (int i = 0; i < howManyLinesFit; i++) {
			dc.DrawText(GetPopupText().Mid((i * textColumns), textColumns), x + border_left, y);
			//move the text down a line
			y += h + textPadding;
		}
	} else {
		dc.SetTextForeground(textColor);
		dc.DrawText(pText, x, y);
	}
}

void ToasterBoxWindow::Notify()
{
	wxLogDebug(_T("Been up for: %d"), (int)(wxGetLocalTime() - startTime));
	ScrollDown();
}

void ToasterBoxWindow::PrintInfo()
{
	wxLogDebug(_T("brX:%i brY:%i szW:%i szH:%i"),
		   bottomRight.x, bottomRight.y, GetSize().GetWidth(),
		   GetSize().GetHeight());
}

void ToasterBoxWindow::OnPaint(wxPaintEvent& /*event*/)
{
	DrawText();
}

void ToasterBoxWindow::SetPopupHeaderText(const wxString& _header)
{
	popupHeaderText = _header;
}
