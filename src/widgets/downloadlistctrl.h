#ifndef WIDGETDOWNLOADLISTCTRL_H
#define WIDGETDOWNLOADLISTCTRL_H

#include "../customvirtlistctrl.h"
#include "widget.h"


class WidgetDownloadListctrl : public CustomVirtListCtrl<Widget,WidgetDownloadListctrl >
{
    public:
        WidgetDownloadListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("WidgetDownloadListCtrl"),
            long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize);
        virtual ~WidgetDownloadListctrl();

        void AddWidget( const Widget widget );
        Widget& GetSelectedWidget();

        //these are overloaded to use list in virtual style
        wxString GetItemText(long item, long column) const;
        int GetItemImage(long item) const;
        int GetItemColumnImage(long item, long column) const;
        wxListItemAttr* GetItemAttr(long item) const {return 0;}

        void Sort();

    protected:
        static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;

        void HighlightItem( long item );
        void OnActivateItem( wxListEvent& event );

        int GetIndexFromData( const DataType& data ) const;

    public:
        enum {
            WIDGETLISTCTRL_ID
        };

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // WIDGETDOWNLOADLISTCTRL_H
