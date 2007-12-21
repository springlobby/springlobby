#include "customlistctrl.h"
  
#define TOOLTIP_DELAY 1000

//TODO maybe test why this needed
//http://wxforum.shadonet.com/viewtopic.php?t=14672&highlight=wxtipwindow
wxMyTipWindow::wxMyTipWindow(wxWindow* parent, const wxString& text):
								wxTipWindow(parent, text)
{
		
}
	
void wxMyTipWindow::OnKeyDown(wxKeyEvent& event)
{
	
}

 
BEGIN_EVENT_TABLE(wxMyTipWindow, wxTipWindow)
    EVT_KEY_DOWN(wxMyTipWindow::OnKeyDown)
END_EVENT_TABLE()
 
customListCtrl::customListCtrl(int coloumnCount_,wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,long style):
					wxListCtrl (parent, id, pt, sz, style),tipTimer(this, IDD_TIP_TIMER)
{
	tw = NULL;
	text = _T("BIBKJBKJB");
	coloumnCount  = coloumnCount_;
	m_tooltips = new wxString[coloumnCount];
}

void customListCtrl::InsertColumn(long i, wxListItem item, wxString tip)
{
	wxListCtrl::InsertColumn(i,item);
	m_tooltips[i] = tip;
}

void customListCtrl::OnTimer(wxTimerEvent& event)
{
	 if (!text.empty())
		{
		    tw = new wxTipWindow(this, text);
#ifndef __WXMSW__ 
		    tw->SetBoundingRect(wxRect(1,1,50,50));
#endif
		}

}

//TODO http://www.wxwidgets.org/manuals/stable/wx_wxtipwindow.html#wxtipwindowsettipwindowptr
// must have sth to do with crash on windows
//if to tootips are displayed
void customListCtrl::OnMouseMotion(wxMouseEvent& event)
{
	//shouldnt be necessary .Start() will actually stop if running and then start aka restart
    if (tipTimer.IsRunning() == true)
    {
        tipTimer.Stop();

    }
    wxPoint position = event.GetPosition();

    int flag = 0;
    long *ptrSubItem = new long;
    long item = HitTest(position, flag, ptrSubItem);
    if (item != wxNOT_FOUND)
    {
        tipTimer.Start(TOOLTIP_DELAY, wxTIMER_ONE_SHOT);
        int coloumn = getColoumnFromPosition(position);
        if (coloumn >= coloumnCount || coloumn < 0)
        {
        	text = _T("");
        }
        else
        {
        	text = m_tooltips[getColoumnFromPosition(position)];
        }
    }
}

int customListCtrl::getColoumnFromPosition(wxPoint pos)
{
	int x_pos = 0;
	for (int i = 0; i <coloumnCount;++i)
	{
		x_pos += GetColumnWidth(i);
		if (pos.x < x_pos)
			return i;
	}
	return -1;
}
 
BEGIN_EVENT_TABLE(customListCtrl, wxListCtrl)
	#ifndef __WXMSW__ 
    	EVT_MOTION(customListCtrl::OnMouseMotion)
    	EVT_TIMER(IDD_TIP_TIMER, customListCtrl::OnTimer)
    #endif
END_EVENT_TABLE()
