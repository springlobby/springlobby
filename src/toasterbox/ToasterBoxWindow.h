/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef TOASTERBOXWINDOW_HH_INCLUDED
#define TOASTERBOXWINDOW_HH_INCLUDED

#include <wx/timer.h>
#include "../utils/mixins.hh"
#include <wx/statbmp.h>

#if wxUSE_POPUPWIN
	#include <wx/popupwin.h>
	typedef wxPopupWindow
		ToasterBase;
#else
	#include <wx/popupwin.h>
	typedef wxWindow
		ToasterBase;
#endif

class ToasterBoxWindow: public ToasterBase, public wxTimer , public SL::NonCopyable
{
  public:
    ToasterBoxWindow(wxWindow* parent, wxTimer *_parent2);
    void SetPopupText(wxString _text, bool _shrink = false);
    void SetPopupSize(int x, int y);
    void SetPopupPosition(int x, int y);
    void SetPopupPauseTime(int milliseconds){pauseTime = milliseconds;}
    void SetPopupBitmap(wxString bitmapFile);
    void SetPopupBitmap(wxBitmap& bitmap);
    void SetPopupBackgroundColor(int r, int g, int b);
    void SetPopupTextColor(int r, int g, int b);
    void SetPopupScrollSpeed(int _sleepTime){sleepTime = _sleepTime;}
    void PrintInfo();
    wxString GetPopupText(){return popupText;}
    bool Play();
    bool DoesTextFit();
    static long count;

	virtual bool AcceptsFocus() const { return false; }
  private:
    void ScrollUp();
    void ScrollDown();
    void DrawText();
    void Notify();

	long startTime;
	wxTimer *parent2;
	int sleepTime;
    //how long the box hangs around for
    int pauseTime;
	wxColour textColor;
	wxString popupText;
	wxBitmap m_background_bitmap;

	wxStaticBitmap sbm;
	wxString bitmapFile;
	wxPoint bottomRight, dialogTop;

    //should we attempt to shrink the text
    //if it's too big for the popup?
    bool shrink;

    //!two dummy functions that do nothing currently
    void OnPaint( wxPaintEvent& event );
    void OnEraseBackground( wxEraseEvent& event );

};

#endif //#define TOASTERBOXWINDOW_HH_INCLUDED
