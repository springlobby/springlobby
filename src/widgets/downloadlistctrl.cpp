#include "downloadlistctrl.h"

WidgetDownloadListctrl::WidgetDownloadListctrl(wxWindow* parent, wxWindowID id, const wxString& name,
                    long style, const wxPoint& pt, const wxSize& sz)
    :CustomVirtListCtrl<Widget>(parent, 4239, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("WidgetDownloadListCtrl"), 3, &CompareOneCrit)
{

}

WidgetDownloadListctrl::~WidgetDownloadListctrl()
{
    //dtor
}

int WidgetDownloadListctrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    return 0;
}
