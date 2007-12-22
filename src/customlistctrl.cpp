#include "customlistctrl.h"
  
#define TOOLTIP_DELAY 1000

customListCtrl::customListCtrl(int coloumnCount_,wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,long style):
					wxListCtrl (parent, id, pt, sz, style),tipTimer(this, IDD_TIP_TIMER)
{
	tw = NULL;
	text = _T("BIBKJBKJB");
	SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
}

void customListCtrl::InsertColumn(long i, wxListItem item, wxString tip, bool modifiable)
{
	wxListCtrl::InsertColumn(i,item);
	colInfo temp(tip,modifiable);
	m_colinfovec.push_back(temp);
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
	if (event.Leaving())
	{
		text = _T("");
		tipTimer.Stop();
	}
	else
	{
	    if (tipTimer.IsRunning() == true)
	    {
	        tipTimer.Stop();
	    }
	    
	    wxPoint position = event.GetPosition();
	
	    int flag = wxLIST_HITTEST_ONITEM;
	    long *ptrSubItem = new long;
	    long item = HitTest(position, flag, ptrSubItem);
	    if (item != wxNOT_FOUND)
	    {
	        
	        int coloumn = getColoumnFromPosition(position);
	        if (coloumn >= m_colinfovec.size() || coloumn < 0)
	        {
	        	text = _T("");
	        }
	        else
	        {	
	        	tipTimer.Start(TOOLTIP_DELAY, wxTIMER_ONE_SHOT);
	        	text = m_colinfovec[coloumn].first;
	        }
	    }
	}
}

int customListCtrl::getColoumnFromPosition(wxPoint pos)
{
	int x_pos = 0;
	for (int i = 0; i <m_colinfovec.size();++i)
	{
		x_pos += GetColumnWidth(i);
		if (pos.x < x_pos)
			return i;
	}
	return -1;
}

void customListCtrl::OnStartResizeCol(wxListEvent& event) 
{
	if (!m_colinfovec[event.GetColumn()].second)
		event.Veto();
}

void customListCtrl::noOp(wxMouseEvent& event)
{
	text = _T("");
}
 
BEGIN_EVENT_TABLE(customListCtrl, wxListCtrl)
	#ifndef __WXMSW__ 
    	EVT_MOTION(customListCtrl::OnMouseMotion)
    	EVT_TIMER(IDD_TIP_TIMER, customListCtrl::OnTimer)
    #endif
    	EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, customListCtrl::OnStartResizeCol) 
    	//EVT_KILL_FOCUS(customListCtrl::noOp)
    	EVT_LEAVE_WINDOW(customListCtrl::noOp)
END_EVENT_TABLE()
