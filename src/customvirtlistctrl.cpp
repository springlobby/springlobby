
#include <wx/colour.h>
#include <wx/log.h>

#include "nonportable.h" //pulls in the SL_DUMMY_COL define if applicable
#include "settings.h"
#include "iconimagelist.h"
#include "utils/customdialogs.h"
#include "uiutils.h"
#include "utils/sltipwin.h"
#include "utils/math.h"
#include "utils/controls.h"

#include <algorithm>


BEGIN_EVENT_TABLE_TEMPLATE2(CustomVirtListCtrl, ListBaseType, T,L)
#if wxUSE_TIPWINDOW
  EVT_MOTION(CustomVirtListCtrl::OnMouseMotion)
  EVT_TIMER(IDD_TIP_TIMER, CustomVirtListCtrl::OnTimer)
#endif
  EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, CustomVirtListCtrl::OnStartResizeCol)
  EVT_LIST_COL_END_DRAG(wxID_ANY, CustomVirtListCtrl::OnEndResizeCol)
  EVT_LEAVE_WINDOW(CustomVirtListCtrl::noOp)
  EVT_LIST_ITEM_SELECTED   ( wxID_ANY, CustomVirtListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( wxID_ANY, CustomVirtListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( wxID_ANY, CustomVirtListCtrl::OnDeselected )
  EVT_LIST_COL_CLICK       ( wxID_ANY, CustomVirtListCtrl::OnColClick )
END_EVENT_TABLE()


template < class T, class L >
CustomVirtListCtrl<T,L>::CustomVirtListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,
                long style, const wxString& name, unsigned int column_count_in, unsigned int sort_criteria_count,
                CompareFunction func, bool highlight, UserActions::ActionType hlaction, bool periodic_sort, unsigned int periodic_sort_interval  )
    : ListBaseType(parent, id, pt, sz, style | wxLC_VIRTUAL),
    m_tiptimer(this, IDD_TIP_TIMER),
    m_sort_timer(this, IDD_SORT_TIMER),
    m_tiptext(_T("")),
#if wxUSE_TIPWINDOW
    m_tipwindow( 0 ),
    m_controlPointer( 0 ),
#endif
#ifndef SL_DUMMY_COL
    m_columnCount( column_count_in ),
#else
    m_columnCount( column_count_in + 1 ),
#endif
    m_selected_index(-1),
    m_prev_selected_index(-1),
    m_last_mouse_pos( wxPoint(-1,-1) ),
    m_name(name),
    m_highlight(highlight),
    m_highlightAction(hlaction),
    m_bg_color( GetBackgroundColour() ),
    m_dirty_sort(false),
    m_sort_criteria_count( sort_criteria_count ),
    m_comparator( m_sortorder, func ),
    m_periodic_sort_timer_id( wxNewId() ),
    m_periodic_sort_timer( this, m_periodic_sort_timer_id ),
    m_periodic_sort( periodic_sort ),
    m_periodic_sort_interval( periodic_sort_interval )
{
    //dummy init , will later be replaced with loading from settings
    for ( unsigned int i = 0; i < m_columnCount; ++i) {
        m_column_map[i] = i;

    }

    SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
    SetImageList( &icons(), wxIMAGE_LIST_SMALL );
    SetImageList( &icons(), wxIMAGE_LIST_STATE );
    m_sortorder = sett().GetSortOrder( name );
    #ifdef SL_DUMMY_COL //a little duplication here, but the AddColumn method will be altered when this is defined
        ListBaseType::InsertColumn( 0, wxEmptyString, wxLIST_FORMAT_LEFT, 0 );
        ListBaseType::SetColumnWidth( 0, 0 );
        colInfo temp( 0, wxEmptyString, wxEmptyString, false, 0 );
        m_colinfovec.push_back( temp );
    #endif

    if ( m_periodic_sort )
    {
        wxTimerEvent a;
        wxEventType evt_type = a.GetEventType();
        Connect( m_periodic_sort_timer_id, evt_type,   wxTimerEventHandler( ThisType::OnPeriodicSort ) );
        bool started = m_periodic_sort_timer.Start( m_periodic_sort_interval );
        assert( started );
    }

}

template < class T, class L >
CustomVirtListCtrl<T,L>::~CustomVirtListCtrl()
{
    Disconnect( m_periodic_sort_timer_id, wxTimerEvent().GetEventType(),   wxTimerEventHandler( ThisType::OnPeriodicSort ) );
    sett().SetSortOrder( m_name, m_sortorder );
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::AddColumn(long i, int width, const wxString& label, const wxString& tip, bool modifiable)
{
    #ifdef SL_DUMMY_COL
        i++;
    #endif
    ListBaseType::InsertColumn( i, label, wxLIST_FORMAT_LEFT, width);
    SetColumnWidth( i, width );
    colInfo temp( i, label, tip, modifiable, width );
    m_colinfovec.push_back( temp );
}



template < class T, class L >
void CustomVirtListCtrl<T,L>::SaveSelection()
{
    ResetSelection();

    long item = -1;
    while ( true )
    {
        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;
        m_selected_data.push_back( m_data[item] );
    }

}

template < class T, class L >
void CustomVirtListCtrl<T,L>::RestoreSelection()
{
    while ( m_selected_data.size() > 0 )
    {
        SelectedDataType data = m_selected_data.back();
        m_selected_data.pop_back();
        int idx = GetIndexFromData( data );
        SetItemState( idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    }
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::ResetSelection()
{
    m_selected_data.clear();
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnSelected( wxListEvent& event )
{
  m_selected_index = event.GetIndex();
  event.Skip();
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnDeselected( wxListEvent& event )
{
    if ( m_selected_index == event.GetIndex() )
        m_selected_index = -1;
}

template < class T, class L >
long CustomVirtListCtrl<T,L>::GetSelectedIndex()
{
  return m_selected_index ;
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::SelectAll()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_SELECTED, -1  );
  }
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::SelectNone()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_DONTCARE, -1 );
  }
}
template < class T, class L >
void CustomVirtListCtrl<T,L>::SelectInverse()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    int state = GetItemState( i, -1 );
    state = ( state == wxLIST_STATE_DONTCARE ? wxLIST_STATE_SELECTED : wxLIST_STATE_DONTCARE );
    SetItemState( i, state, -1 );
  }
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::SetSelectedIndex(const long newindex)
{
    m_selected_index = newindex;
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::RefreshVisibleItems()
{
#ifndef __WXMSW__
    long topItemIndex = GetTopItem();
    long range = topItemIndex + GetCountPerPage() -1;
    //RefreshItems( topItemIndex,  clamp( range, topItemIndex, (long) m_data.size() ) );
    RefreshItems( topItemIndex,  range );
#else
    RefreshItems( 0,  m_data.size() -1 );
#endif
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnTimer(wxTimerEvent& /*unused*/ )
{
#if wxUSE_TIPWINDOW

  if (!m_tiptext.empty())
  {
      m_tipwindow = new SLTipWindow(this, m_tiptext);
      m_controlPointer = &m_tipwindow;
      m_tipwindow->SetTipWindowPtr((wxTipWindow**)m_controlPointer);
#ifndef __WXMSW__
      m_tipwindow->SetBoundingRect(wxRect(1,1,50,50));
#endif
      m_tiptext = wxEmptyString;
      m_tiptimer.Start(m_tooltip_duration, wxTIMER_ONE_SHOT);
  }
  else
  {
      m_tiptext = wxEmptyString;
      m_tiptimer.Stop();
      if (m_controlPointer!= 0 && *m_controlPointer!= 0)
      {
          m_tipwindow->Close();
          m_tipwindow = 0;
      }
  }

#endif
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnMouseMotion(wxMouseEvent& event)
{
    m_sort_timer.Stop();
    m_sort_timer.Start( m_sort_block_time  , wxTIMER_ONE_SHOT );
#if wxUSE_TIPWINDOW
  //we don't want to display the tooltip again until mouse has moved
  if ( m_last_mouse_pos == event.GetPosition() )
    return;

  m_last_mouse_pos = event.GetPosition();

  if (event.Leaving())
  {
    m_tiptext = _T("");
    if (m_tipwindow)
    {
      m_tipwindow->Close();
      m_tipwindow = 0;
    }
    m_tiptimer.Stop();
  }
  else
  {
    if ( m_tiptimer.IsRunning() )
    {
      m_tiptimer.Stop();
    }

    wxPoint position = event.GetPosition();

    int flag = wxLIST_HITTEST_ONITEM;

#ifdef HAVE_WX28
    long subItem;
    long item_hit = HitTest(position, flag, &subItem);
#else
    long item_hit = HitTest(position, flag);
#endif
    if (item_hit != wxNOT_FOUND && item_hit>=0 && item_hit<GetItemCount())
    {
        // we don't really need to recover from this if it fails
      try
      {
        SetTipWindowText(item_hit,m_last_mouse_pos);
        m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
      }
      catch ( ... ) {
        wxLogWarning( _T("Exception setting tooltip") );
      }
    }
  }
#endif
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::SetTipWindowText( const long /*unused*/ , const wxPoint& position)
{
  int column = getColumnFromPosition(position);
  #ifdef SL_DUMMY_COL
    column++;
  #endif
  if (column >= int(m_colinfovec.size()) || column < 0)
  {
    m_tiptext = _T("");
  }
  else
  {
    m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
    m_tiptext = TE(m_colinfovec[column].tip);
  }
}

template < class T, class L >
int CustomVirtListCtrl<T,L>::getColumnFromPosition(wxPoint pos)
{
    int x_pos = 0;
    for (int i = 0; i < int(m_colinfovec.size());++i)
    {
        x_pos += GetColumnWidth(i);
        if (pos.x < x_pos)
        #ifdef SL_DUMMY_COL
            return i-1;
        #else
            return i;
        #endif
    }
    return -1;
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnStartResizeCol(wxListEvent& event)
{
    if (!m_colinfovec[event.GetColumn()].can_resize)
        event.Veto();
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnEndResizeCol(wxListEvent& event)
{
    int column = event.GetColumn();
    int new_size = GetColumnWidth( column );
    sett().SetColumnWidth( m_name, column, new_size );
    sett().SaveSettings();

    //let the event go further
    event.Skip();
}

template < class T, class L >
bool CustomVirtListCtrl<T,L>::SetColumnWidth(int col, int& width)
{
    if ( sett().GetColumnWidth( m_name, col) != Settings::columnWidthUnset)
    {
        width = sett().GetColumnWidth( m_name, col);
        return ListBaseType::SetColumnWidth( col, width );
    }
    else
    {
        sett().SetColumnWidth( m_name, col, width );
        return ListBaseType::SetColumnWidth( col, width );
    }
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::noOp(wxMouseEvent& event)
{
    m_tiptext = wxEmptyString;
//            m_tiptimer.Stop();
//            if (m_controlPointer!= 0 && *m_controlPointer!= 0)
//            {
//                m_tipwindow->Close();
//                m_tipwindow = 0;
//            }
    event.Skip();
}

template < class T, class L >
wxListItemAttr* CustomVirtListCtrl<T,L>::HighlightItemUser( const wxString& name ) const
{
    static wxListItemAttr att;
  if ( m_highlight && useractions().DoActionOnUser( m_highlightAction, name ) ) {
        att.SetBackgroundColour( sett().GetGroupHLColor( useractions().GetGroupOfUser( name ) ) );
        return &att;
  }
  else
    return NULL;
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::SetHighLightAction( UserActions::ActionType action )
{
    m_highlightAction = action;
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::MarkDirtySort()
{
  m_dirty_sort = true;
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::CancelTooltipTimer()
{
    m_tiptimer.Stop();
}

template < class T, class L >
bool CustomVirtListCtrl<T,L>::PopupMenu(wxMenu* menu, const wxPoint& pos )
{
    CancelTooltipTimer();
    return ListBaseType::PopupMenu( menu, pos );
}

template < class T, class L >
wxMutex CustomVirtListCtrl<T,L>::s_mutexProtectingTheGlobalData;


template < class T, class L >
void CustomVirtListCtrl<T,L>::SortList( bool force )
{
    if ( ( m_sort_timer.IsRunning() ||  !m_dirty_sort ) && !force )
        return;
    wxMutexLocker lock(s_mutexProtectingTheGlobalData);

    SelectionSaver<ThisType>(*this);
    Freeze();
    Sort();
    Thaw();
    m_dirty_sort = false;
    RefreshVisibleItems();
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::Clear()
{
    wxMutexLocker lock(s_mutexProtectingTheGlobalData);
    m_data.clear();
    SetItemCount( 0 );
    ResetSelection();
    RefreshVisibleItems();
}

template < class T, class L >
typename CustomVirtListCtrl<T,L>::DataType CustomVirtListCtrl<T,L>::GetDataFromIndex ( const  long index )
{
    return m_data[index];
}

template < class T, class L >
const typename CustomVirtListCtrl<T,L>::DataType CustomVirtListCtrl<T,L>::GetDataFromIndex ( const  long index ) const
{
    return m_data[index];
}

template < class T, class L >
typename CustomVirtListCtrl<T,L>::DataType CustomVirtListCtrl<T,L>::GetSelectedData()
{
    return GetDataFromIndex( m_selected_index );
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::ResetColumnSizes()
{
    typename colInfoVec::const_iterator it = m_colinfovec.begin();
    for ( ; it != m_colinfovec.end(); ++it ) {
        int width = it->size;
        SetColumnWidth( it->col_num, width );
    }
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnColClick( wxListEvent& event )
{
    if ( event.GetColumn() == -1 )
        return;

    #ifdef SL_DUMMY_COL
        if ( event.GetColumn() == 0 )
            return;
        const int evt_col = event.GetColumn()-1;
    #else
        const int evt_col = event.GetColumn();
    #endif

    m_sort_timer.Stop();//otherwise sorting will be way delayed

    int old_sort_col = m_sortorder[0].col;

    wxListItem col;
    GetColumn( m_sortorder[0].col, col );
    col.SetImage( icons().ICON_NONE );
    SetColumn( m_sortorder[0].col, col );

    unsigned int i = 0;
    SortOrder::const_iterator it = m_sortorder.begin();
    for ( ; it != m_sortorder.begin(); ++i, ++it ) {
        if ( m_sortorder[i].col == evt_col )
            break;
    }

//    for ( ; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}

    i = clamp( i, (unsigned int)0, m_sort_criteria_count );

    for ( ; i > 0; i--) {
        m_sortorder[i] = m_sortorder[i-1];
    }

    m_sortorder[0].col = evt_col;
    m_sortorder[0].direction *= -1;


    GetColumn( m_sortorder[0].col, col );
    //col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
    col.SetImage( ( m_sortorder[0].direction > 0 )?icons().ICON_UP:icons().ICON_DOWN );
    SetColumn( m_sortorder[0].col, col );

    if ( old_sort_col != m_sortorder[0].col )
        SortList( true );
    else { // O(n) instead of guaranteed worst case O(n*n)
        ReverseOrder();
    }
}

template < class T, class L >
bool CustomVirtListCtrl<T,L>::GetColumn(int col, wxListItem& item) const
{
    #ifdef SL_DUMMY_COL
        col++;
    #endif
    return ListBaseType::GetColumn( col, item );
}

template < class T, class L >
bool CustomVirtListCtrl<T,L>::SetColumn(int col, wxListItem& item)
{
    #ifdef SL_DUMMY_COL
        col++;
    #endif
    return ListBaseType::SetColumn( col, item );
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::ReverseOrder()
{
    SaveSelection();
    std::reverse( m_data.begin(), m_data.end() );
    RefreshVisibleItems();
    RestoreSelection();
}

template < class T, class L >
bool CustomVirtListCtrl<T,L>::AddItem( const T item )
{
    if ( GetIndexFromData( item ) != -1 )
        return false;

    wxMutexLocker lock(s_mutexProtectingTheGlobalData);
    m_data.push_back( item );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() - 1 );
    //SetColumnWidth( 5, wxLIST_AUTOSIZE ); //! TODO does this really work?
    MarkDirtySort();
    return true;
}

template < class T, class L >
bool CustomVirtListCtrl<T,L>::RemoveItem( const T item )
{
    int index = GetIndexFromData( item );

    if ( index != -1 ) {
        wxMutexLocker lock(s_mutexProtectingTheGlobalData);
        SelectionSaver<ThisType>(*this);
        m_data.erase( m_data.begin() + index );
        SetItemCount( m_data.size() );
        RefreshItems( index, m_data.size() -1 );
        return true;
    }
    return false;
}

template < class T, class L >
wxString CustomVirtListCtrl<T,L>::OnGetItemText(long item, long column) const
{
    #ifdef SL_DUMMY_COL
    if ( column < 1 )
        return wxEmptyString;
    column--;
    #endif
    return asImp().GetItemText(item, column);
}

template < class T, class L >
int CustomVirtListCtrl<T,L>::OnGetItemColumnImage(long item, long column) const
{
    #ifdef SL_DUMMY_COL
    if ( column < 1 )
        return -1;
    column--;
    #endif
    return asImp().GetItemColumnImage(item, column);
}

template < class T, class L >
wxListItemAttr* CustomVirtListCtrl<T,L>::OnGetItemAttr(long item) const
{
    return asImp().GetItemAttr(item);
}

template < class T, class L >
void CustomVirtListCtrl<T,L>::OnPeriodicSort( wxTimerEvent& /*unused*/  )
{
    SortList();
}
