#include "customListCtrl.h"
  
#define TOOLTIP_DELAY 200
//TODO maybe test why this needed
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
        tw = new wxMyTipWindow(this, text);
        tw->SetBoundingRect(wxRect(1,1,50,50));
    }
}

void customListCtrl::OnMouseMotion(wxMouseEvent& event)
{
    if (tipTimer.IsRunning() == true)
        tipTimer.Stop();
    wxPoint position = event.GetPosition();
    
    int flag = 0;
    long *ptrSubItem;
    long item = HitTest(position, flag, ptrSubItem);
    if (item != wxNOT_FOUND)
    {
        tipTimer.Start(TOOLTIP_DELAY, wxTIMER_ONE_SHOT);
        text = m_tooltips[getColoumnFromPosition(position)];
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
    EVT_MOTION(customListCtrl::OnMouseMotion)
    EVT_TIMER(IDD_TIP_TIMER, customListCtrl::OnTimer)
END_EVENT_TABLE()
