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
/** global delay (ms)
 * \todo make this definable per child class
 */
#define TOOLTIP_DELAY 1000

typedef std::pair<wxString,bool> colInfo;
typedef std::vector<colInfo> colInfoVec;

/** \brief Used as base class for all ListCtrls throughout SL
 * Provides generic functionality, such as column tooltips, possiblity to prohibit coloumn resizing and selection modifiers. \n
 * Some of the provided functionality only makes sense for single-select lists (see grouping) \n
 * Note: Tooltips are a bitch and anyone shoudl feel to revise them (koshi)
 */
class customListCtrl : public wxListCtrl
{
protected:
    //! used to display tooltips for a certain amount of time
    wxTimer     tipTimer;
    //! always set to the currrently displayed tooltip text
    wxString    m_tiptext;
    #if wxUSE_TIPWINDOW
    //! some wx implementations do not support this yet
    wxTipWindow *m_tipwindow;
    #endif
    int coloumnCount;

/*** these are only meaningful in single selection lists ***/
    //! curently selected data
    long m_selected;
    //! index of curently selected data
    long m_selected_index;
    //! previously selected data
    long m_prev_selected;
    //! index of previously selected data
    long m_prev_selected_index;
/***********************************************************/

    //! stores info about the columns (wxString name,bool isResizable) - pairs
    colInfoVec m_colinfovec;
    //! primarily used to get coulumn index in mousevents (from cur. mouse pos)
    int getColoumnFromPosition(wxPoint pos);

public:
	customListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style);

    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    /** @name Single Selection methods
     * using these funcs in a multi selection list is meaingless at best, harmful in the worst case
     * \todo insert debug asserts to catch that
     * @{
     */
    long GetSelectedIndex();
    void SetSelectedIndex(const long newindex);
    long GetSelectedData();
    long GetIndexFromData( const unsigned long data );
    //! call this before example before sorting, inserting, etc
    void SetSelectionRestorePoint();
    //! and this afterwards
    void RestoreSelection();
    /** @}
     */

    //! intermediate function to add info to m_colinfovec after calling base class function
	void InsertColumn(long i, wxListItem item, wxString tip, bool = true);
	//! this event is triggered when delay timer (set in mousemotion) ended
    void OnTimer(wxTimerEvent& event);
    //! starts timer, sets tooltiptext
    void OnMouseMotion(wxMouseEvent& event);
    //! prohibits resizin if so set in columnInfo
    void OnStartResizeCol(wxListEvent& event) ;
    //! does nothing
    void noOp(wxMouseEvent& event);

    /** @name Multi Selection methods
     * using these funcs in a single selection list is meaingless at best, harmful in the worst case
     * \todo insert debug asserts to catch that
     * @{
     */
    void SelectAll();
    void SelectInverse();
    /** @}
     */

    //! sets selected index to -1
    void SelectNone();


    DECLARE_EVENT_TABLE()
};

#endif /*CUSTOMLISTITEM_H_*/
