#include "ToasterBoxWindow.h"
#include "ToasterBoxWindowList.h"
/*
  The toasterbox class should stay resident in memory.
  It creates and displays popups and handles the
  "stacking".
*/

class ToasterBox : public wxTimer
{
  public:
    ToasterBox(wxWindow* _parent=(wxWindow *)NULL);
    ~ToasterBox();
    void SetPopupText(wxString _text, bool _shrink = false){popupText = _text;}
    void SetPopupSize(int x, int y){popupSize = wxSize(x, y);}
    void SetPopupPosition(int x, int y);
    void SetPopupPosition(int pos);
    void SetPopupPauseTime(int milliseconds){pauseTime = milliseconds;}
    void SetPopupBitmap(wxString _bitmapFile){bitmapFile = _bitmapFile;}
    void SetPopupBackgroundColor(int r, int g, int b);
    void SetPopupTextColor(int r, int g, int b);
    void SetPopupScrollSpeed(int _sleepTime){sleepTime = _sleepTime;}
    void MoveAbove(ToasterBoxWindow *tb);
    wxString GetPopupText(){return popupText;}
    void Play();
    bool DoesTextFit();
    void Notify();
    void CleanList();
    void StartAll(bool start = true);
    
  
  private:
    wxPoint bottomRight, popupTop, popupPosition;
    wxSize popupSize;
    int sleepTime;
    //how long the box hangs around for
    int pauseTime;    
    wxStaticBitmap sbm;
    wxColour colFg, colBg;
    wxString popupText, bitmapFile;
    wxWindow *parent;
    ToasterBoxWindowList *winList;
    //should we attempt to shrink the text
    //if it's too big for the popup?
    bool shrink;
};