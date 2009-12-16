#ifndef TOASTERBOXWINDOW_HH_INCLUDED
#define TOASTERBOXWINDOW_HH_INCLUDED
#include <wx/wx.h>
class ToasterBoxWindow: public wxFrame, public wxTimer
{
  public:
    ToasterBoxWindow(wxWindow* parent, wxTimer *_parent2);
    void SetPopupText(wxString _text, bool _shrink = false);
    void SetPopupSize(int x, int y);
    void SetPopupPosition(int x, int y);
    void SetPopupPauseTime(int milliseconds){pauseTime = milliseconds;}
    void SetPopupBitmap(wxString bitmapFile);
    void SetPopupBackgroundColor(int r, int g, int b);
    void SetPopupTextColor(int r, int g, int b);
    void SetPopupScrollSpeed(int _sleepTime){sleepTime = _sleepTime;}
    void PrintInfo();
    wxString GetPopupText(){return popupText;}
    bool Play();
    bool DoesTextFit();
    static long count;


  private:
    void ScrollUp();
    void ScrollDown( wxTimerEvent& event );
    void DrawText();
    void Notify();

    wxPoint bottomRight, dialogTop;
    int sleepTime, step;
    //how long the box hangs around for
    int pauseTime;
    wxStaticBitmap sbm;
    wxColour textColor;
    wxPanel *pn;
    wxString popupText, bitmapFile;
    wxTimer *parent2;
    long startTime;
    //should we attempt to shrink the text
    //if it's too big for the popup?
    bool shrink;
    /// wxEVT_PAINT event handler for ID_WXGRADIENTBUTTON
		void OnPaint( wxPaintEvent& event );

/// wxEVT_ERASE_BACKGROUND event handler for ID_WXGRADIENTBUTTON
		void OnEraseBackground( wxEraseEvent& event );

//    wxTimer m_timer;
//    DECLARE_EVENT_TABLE()
};

#endif //#define TOASTERBOXWINDOW_HH_INCLUDED
