#ifndef CUSTOMVIRTLISTITEM_H_
#define CUSTOMVIRTLISTITEM_H_

#ifndef __WXMSW__
    #include <wx/listctrl.h>
    typedef wxListCtrl ListBaseType;
#else
//disabled until further fixes
//    #include "Helper/listctrl.h"
//    typedef SL_Extern::wxGenericListCtrl ListBaseType;
    #include <wx/listctrl.h>
    typedef wxListCtrl ListBaseType;
#endif

#include <wx/timer.h>
#define IDD_TIP_TIMER 696
#define IDD_SORT_TIMER 697

#include <vector>

#include <utility>
#include <map>

#include "useractions.h"
#include "Helper/sortutil.h"

class SLTipWindow;


/** \brief Used as base class for some ListCtrls throughout SL
 * Provides generic functionality, such as column tooltips, possiblity to prohibit column resizing and selection modifiers. \n
 * Some of the provided functionality only makes sense for single-select lists (see grouping) \n
 * Note: the second template param is actually just a dummy to ensure the compiler generates distinct code in case we have different listctrls with same datatype
 * Note: Tooltips are a bitch and anyone should feel free to revise them (koshi)
 * \tparam the type of stored data
 */
template < class DataImp, class ListCtrlImp >
class CustomVirtListCtrl : public ListBaseType
{
public:
    typedef DataImp
        DataType;

protected:
    typedef UserActions::ActionType ActionType;
    //! used to display tooltips for a certain amount of time
    wxTimer m_tiptimer;
    //! used to block sorting while mouse is moving
    wxTimer m_sort_timer;
    //! always set to the currrently displayed tooltip text
    wxString m_tiptext;
    #if wxUSE_TIPWINDOW
    //! some wx implementations do not support this yet
    SLTipWindow* m_tipwindow;
    SLTipWindow** m_controlPointer;
    #endif
    unsigned int m_columnCount;

    struct colInfo {
        colInfo(int n, wxString l, wxString t, bool c, int s):
            col_num(n),label(l),tip(t),can_resize(c),size(s) {}
        colInfo(){}

        int col_num;
        wxString label;
        wxString tip;
        bool can_resize;
        int size;
    };

    typedef std::vector<colInfo> colInfoVec;

    //! maps outward column index to internal
    typedef std::map<unsigned int,unsigned int> ColumnMap;

    /** global Tooltip thingies (ms)
     */
    static const unsigned int m_tooltip_delay    = 1000;
    static const unsigned int m_tooltip_duration = 2000;
    static const unsigned int m_sort_block_time  = 1500;

/*** these are only meaningful in single selection lists ***/
    //! index of curently selected data
    long m_selected_index;

    //! index of previously selected data
    long m_prev_selected_index;
/***********************************************************/

    //! stores info about the columns (wxString name,bool isResizable) - pairs
    colInfoVec m_colinfovec;
    //! primarily used to get coulumn index in mousevents (from cur. mouse pos)
    int getColumnFromPosition(wxPoint pos);

    //! map: index in visible list <--> index in data vector
    typedef std::map<int,int> VisibilityMap;
    typedef VisibilityMap::iterator VisibilityMapIter;
    /** \brief list indexes of not-filtered items
     * use like this: when adding items set identity mapping \n
         m_visible_idxs[m_data.size() -1] = ( m_data.size() -1 ); \n
       when filtering clear the map, iterate thru data and only set mapping for matching items in data \n
       when acessing data (getColoumText etc.) always access data's index thru this map
     **/
    VisibilityMap m_visible_idxs;


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

    /** @name Sort functionality
     *
     * @{
     */

    static SortOrder m_sortorder;
    unsigned int m_sort_criteria_count;

    /** generic comparator that gets it's real functionality
     * in derived classes via comapre callbakc func that
     * performs the actual comparison of two items **/
    template < class ObjImp >
    struct ItemComparator
    {
        typedef ObjImp ObjType;
        SortOrder& m_order;
        typedef int (*CmpFunc)  ( ObjType u1, ObjType u2, int, int );
        CmpFunc m_cmp_func;
        const unsigned int m_num_criteria;

        /** \param order SortOrder map that defines which columns should be sorted in what directions
         * \param func the comparison callback func. Should return -1,0,1 for less,equal,greater
         * \param num_criteria set to 1,2 to limit sub-ordering
         * \todo make order const reference to eliminate assumption about existence of entries
         */
        ItemComparator( SortOrder& order,CmpFunc func, const unsigned int num_criteria = 3 )
            :m_order(order),
            m_cmp_func(func),
            m_num_criteria(num_criteria)
        {}

        bool operator () ( ObjType u1, ObjType u2 ) const
        {
            int res = m_cmp_func( u1, u2, m_order[0].col, m_order[0].direction );
            if ( res != 0 )
                return res < 0;

            if ( m_num_criteria > 1 ) {
                res = m_cmp_func( u1, u2, m_order[1].col, m_order[1].direction );
                if ( res != 0 )
                    return res < 0;

                if ( m_num_criteria > 2 ) {
                    res = m_cmp_func( u1, u2, m_order[2].col, m_order[2].direction );
                    if ( res != 0 )
                        return res < 0;
                }
            }
            return false;
        }
    };

    typedef typename ItemComparator<DataImp>::CmpFunc CompareFunction;

    //! compare func usable for types with well-defined ordering (and implemented ops <,>)
    template < typename Type >
    static inline int compareSimple( Type o1, Type o2 ) {
        if ( o1 < o2 )
            return -1;
        else if ( o1 > o2 )
            return 1;
        return 0;
    }

    //! must be implemented in derived classes, should call the actual sorting on data and refreshitems
    virtual void Sort( ) = 0;

public:
    /** only sorts if data is marked dirty, or force is true
     * calls Freeze(), Sort(), Thaw() */
    void SortList( bool force = false );
    /** @}
     */

public:
    CustomVirtListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt,
                    const wxSize& sz,long style, const wxString& name, unsigned int column_count, unsigned int sort_criteria_count, CompareFunction func, bool highlight = true,
                    UserActions::ActionType hlaction = UserActions::ActHighlight);

    virtual ~CustomVirtListCtrl();

    void OnSelected( wxListEvent& event );
    void OnDeselected( wxListEvent& event );
    /** @name Single Selection methods
     * using these funcs in a multi selection list is meaningless at best, harmful in the worst case
     * \todo insert debug asserts to catch that
     * @{
     */
    long GetSelectedIndex();
    void SetSelectedIndex(const long newindex);
    DataType GetDataFromIndex ( const  long index );
    const DataType GetDataFromIndex ( const  long index ) const;
    DataType GetSelectedData();
    /** @}
     */

    /** @name Multi Selection methods
     * call this before example before sorting, inserting, etc
     */
    void SaveSelection();
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
    //! reset columns with current set size (only effects columns with auto-size)
    void ResetColumnSizes();

    // funcs that should make things easier for group highlighting
    ///all that needs to be implemented in child class for UpdateHighlights to work

    wxListItemAttr* HighlightItemUser( const wxString& name ) const;

    void SetHighLightAction( UserActions::ActionType action );
    void RefreshVisibleItems();

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

     /** @name overloaded wxFunctions
      * these are used to display items in virtual lists
      * @{
     */
    virtual wxString OnGetItemText(long item, long column) const = 0;
    virtual int OnGetItemColumnImage(long item, long column) const = 0;
    virtual int OnGetItemImage(long item) const =0;
    /** @}
     */

     //! delete all data, selections, and whatnot
     virtual void Clear();

     //! handle sort order updates
     void OnColClick( wxListEvent& event );

     virtual int GetIndexFromData( const DataType& data ) const = 0;

     void ReverseOrder();

protected:
    typedef CustomVirtListCtrl< DataImp, ListCtrlImp >
        BaseType;
    typedef std::vector< DataImp >
        DataVector;
    typedef typename DataVector::iterator
        DataIter;
    typedef typename DataVector::const_iterator
        DataCIter;
    typedef typename DataVector::reverse_iterator
        DataRevIter;
    typedef typename DataVector::const_reverse_iterator
        DataRevCIter;
    DataVector m_data;

    typedef DataType
        SelectedDataType;
    typedef std::vector< SelectedDataType >
        SelectedDataVector;
    SelectedDataVector m_selected_data;

    //! the Comparator object passed to the SLInsertionSort function
    ItemComparator<DataType> m_comparator;

    bool RemoveItem( const DataImp item );
    bool AddItem( const DataImp item );

public:
    DECLARE_EVENT_TABLE()

private:
    typedef BaseType
        ThisType;
};

template < class ListCtrlType >
class SelectionSaver {
    ListCtrlType* m_list;

public:
    SelectionSaver( ListCtrlType* list  )
        : m_list( list )
    { m_list->SaveSelection(); }

    ~SelectionSaver()
    { m_list->RestoreSelection(); }
};

#include "customvirtlistctrl.cpp"

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

