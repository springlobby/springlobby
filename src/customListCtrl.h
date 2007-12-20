#ifndef CUSTOMLISTITEM_H_
#define CUSTOMLISTITEM_H_

#include <wx/window.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/tipwin.h>
#include <wx/timer.h>
#define IDD_TIP_TIMER 666

/*class customListItem : public wxListItem
{
private:
	wxString m_tiptext;
public:

	void SetToolTip(wxString tip)
	{
		m_tiptext = tip;
	}
	
};*/

class wxMyTipWindow : public wxTipWindow
{
    public:
    wxMyTipWindow(wxWindow* parent, const wxString& text);
    void OnKeyDown(wxKeyEvent& event);
    DECLARE_EVENT_TABLE()
};
 
class customListCtrl : public wxListCtrl
{
private:
    wxTimer     tipTimer;
    wxString    text;
    wxTipWindow *tw;
    int coloumnCount;
    wxString* m_tooltips;
    int getColoumnFromPosition(wxPoint pos);
public:
	customListCtrl(int coloumnCount_,wxWindow* parent, wxWindowID id, const wxPoint& pt, 
                    const wxSize& sz,long style);
	
	void InsertColumn(long i, wxListItem item, wxString tip);
    void OnTimer(wxTimerEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    
    DECLARE_EVENT_TABLE()
};
 
#endif /*CUSTOMLISTITEM_H_*/
