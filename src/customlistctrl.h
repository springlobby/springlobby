#ifndef CUSTOMLISTITEM_H_
#define CUSTOMLISTITEM_H_

#ifndef __WXMSW__
    #include <wx/listctrl.h>
    typedef wxListCtrl ListBaseType;
#else
//disabled until further fixes
    #include <wx/msw/winundef.h>
    #include "Helper/listctrl.h"
    typedef SL_Extern::wxGenericListCtrl ListBaseType;
//    #include <wx/listctrl.h>
//    typedef wxListCtrl ListBaseType;
#endif

#include <wx/timer.h>
#define IDD_TIP_TIMER 696

#include <vector>
#include <utility>
#include <map>

#include "useractions.h"

class SLTipWindow;


/** \brief Used as base class for all ListCtrls throughout SL
 * Provides generic functionality, such as column tooltips, possiblity to prohibit coloumn resizing and selection modifiers. \n
 * Some of the provided functionality only makes sense for single-select lists (see grouping) \n
 * Note: Tooltips are a bitch and anyone shoudl feel to revise them (koshi)
 */
class CustomListCtrl : public ListBaseType
{
protected:
    typedef UserActions::ActionType ActionType;
    //! used to display tooltips for a certain amount of time
    wxTimer m_tiptimer;
    //! always set to the currrently displayed tooltip text
    wxString m_tiptext;
    #if wxUSE_TIPWINDOW
    //! some wx implementations do not support this yet
    SLTipWindow* m_tipwindow;
    SLTipWindow** m_controlPointer;
    #endif
    unsigned int m_coloumnCount;

    typedef std::pair<wxString,bool> colInfo;
    typedef std::vector<colInfo> colInfoVec;

    //! maps outward column index to internal
    typedef std::map<unsigned int,unsigned int> ColumnMap;

    /** global Tooltip thingies (ms)
     */
    static const unsigned int m_tooltip_delay    = 1000;
    static const unsigned int m_tooltip_duration = 2000;

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

    wxPoint m_last_mouse_pos;

    //! used as label for saving column widths
    wxString m_name;

    //!controls if highlighting should be considered
    bool m_highlight;

    //! which action should be considered?
    ActionType m_highlightAction;

    const wxColor m_bg_color;

    //! list should be sorted
    bool m_dirty_sort;

    virtual void SetTipWindowText( const long item_hit, const wxPoint position);

    ColumnMap m_column_map;

    struct SortOrderItem {
        int col;
        int direction;
    };
    typedef std::map<int,SortOrderItem> SortOrder;
    SortOrder m_sortorder;

public:
    CustomListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style, wxString name, unsigned int column_count, bool highlight = true,
                    UserActions::ActionType hlaction = UserActions::ActHighlight);

    virtual ~CustomListCtrl(){}

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
    void ResetSelection();
    //! and this afterwards
    void RestoreSelection();
    /** @}
     */

    //! intermediate function to add info to m_colinfovec after calling base class function
    void InsertColumn(long i, wxListItem item, wxString tip, bool = true);
    void AddColumn(long i, int width, const wxString& label, const wxString& tip, bool = true);
    //! this event is triggered when delay timer (set in mousemotion) ended
    virtual void OnTimer(wxTimerEvent& event);
    //! prohibits resizin if so set in columnInfo
    void OnStartResizeCol(wxListEvent& event);
    //! we use this to automatically save column width after resizin
    virtual void OnEndResizeCol(wxListEvent& event);
    //! starts timer, sets tooltiptext
    virtual void OnMouseMotion(wxMouseEvent& event);
    //! stop timer (before displaying popup f.e.)
    void CancelTooltipTimer();
    //!Override to have tooltip timer cancelled automatically
    bool PopupMenu(wxMenu* menu, const wxPoint& pos = wxDefaultPosition);
    //! does nothing
    void noOp(wxMouseEvent& event);
    //! automatically get saved column width if already saved, otherwise use parameter and save new width
    virtual bool SetColumnWidth(int col, int width);

    // funcs that should make things easier for group highlighting
    ///all that needs to be implemented in child class for UpdateHighlights to work
    virtual void HighlightItem( long item ) = 0;
    void HighlightItemUser( long item, const wxString& name );
    void UpdateHighlights();
    void SetHighLightAction( UserActions::ActionType action );


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

    //! marks the items in the control to be sorted
    void MarkDirtySort();

    DECLARE_EVENT_TABLE()
};


#endif /*CUSTOMLISTITEM_H_*/

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-09

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

