#ifndef CUSTOMLISTITEM_H_
#define CUSTOMLISTITEM_H_

#include <wx/window.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/tipwin.h>
#include <wx/timer.h>
#define IDD_TIP_TIMER 666
#include <vector>
#include <utility>

typedef std::pair<wxString,bool> colInfo;
typedef std::vector<colInfo> colInfoVec;

class customListCtrl : public wxListCtrl
{
protected:
    wxTimer     tipTimer;
    wxString    m_tiptext;
    wxTipWindow *m_tipwindow;
    int coloumnCount;
   
    colInfoVec m_colinfovec;
    int getColoumnFromPosition(wxPoint pos);
public:
	customListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, 
                    const wxSize& sz,long style);
	
	void InsertColumn(long i, wxListItem item, wxString tip, bool = true);
    void OnTimer(wxTimerEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnStartResizeCol(wxListEvent& event) ;
    void noOp(wxMouseEvent& event);
    
    DECLARE_EVENT_TABLE()
};
 
#endif /*CUSTOMLISTITEM_H_*/
