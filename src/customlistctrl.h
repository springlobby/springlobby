#ifndef CUSTOMLISTITEM_H_
#define CUSTOMLISTITEM_H_

#include <wx/window.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#if wxUSE_TIPWINDOW
#include <wx/tipwin.h>
#endif
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
    #if wxUSE_TIPWINDOW
    wxTipWindow *m_tipwindow;
    #endif
    int coloumnCount;
    long m_selected;
    long m_selected_index;
    long m_prev_selected;
    long m_prev_selected_index;

    colInfoVec m_colinfovec;
    int getColoumnFromPosition(wxPoint pos);

public:
	customListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style);

    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    long GetSelectedIndex();
    void SetSelectedIndex(const long newindex);
    long GetSelectedData();
    long GetIndexFromData( const unsigned long data );

	void InsertColumn(long i, wxListItem item, wxString tip, bool = true);
    void OnTimer(wxTimerEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnStartResizeCol(wxListEvent& event) ;
    void noOp(wxMouseEvent& event);

    void RestoreSelection();
    void SetSelectionRestorePoint();

    void SelectAll();
    void SelectNone();
    void SelectInverse();


    DECLARE_EVENT_TABLE()
};

#endif /*CUSTOMLISTITEM_H_*/
