#ifndef WIDGETDOWNLOADLISTCTRL_H
#define WIDGETDOWNLOADLISTCTRL_H

#include "../customvirtlistctrl.h"
#include "widget.h"


class WidgetDownloadListctrl : public CustomVirtListCtrl<Widget>
{
    public:
        WidgetDownloadListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("WidgetDownloadListCtrl"),
            long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize);
        virtual ~WidgetDownloadListctrl();
    protected:
        static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;

};

#endif // WIDGETDOWNLOADLISTCTRL_H
