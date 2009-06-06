
#include <wx/colour.h>
#include <wx/log.h>

#include "utils.h"
#include "settings.h"
#include "iconimagelist.h"
#include "settings++/custom_dialogs.h"
#include "uiutils.h"

#include <algorithm>


BEGIN_EVENT_TABLE_TEMPLATE1(CustomVirtListCtrl, ListBaseType, T)
#if wxUSE_TIPWINDOW
  EVT_MOTION(CustomVirtListCtrl<T>::OnMouseMotion)
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


template < class T >
CustomVirtListCtrl<T>::CustomVirtListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,
                long style, const wxString& name, unsigned int column_count, unsigned int sort_criteria_count,
                CompareFunction func, bool highlight, UserActions::ActionType hlaction ):
  ListBaseType(parent, id, pt, sz, style | wxLC_VIRTUAL),
  m_tiptimer(this, IDD_TIP_TIMER),
  m_tiptext(_T("")),
#if wxUSE_TIPWINDOW
  m_tipwindow( 0 ),
  m_controlPointer( 0 ),
#endif
  m_coloumnCount( column_count ),
  m_selected_index(-1),
  m_prev_selected_index(-1),
  m_last_mouse_pos( wxPoint(-1,-1) ),
  m_name(name),
  m_highlight(highlight),
  m_highlightAction(hlaction),
  m_bg_color( GetBackgroundColour() ),
  m_dirty_sort(false),
  m_sort_criteria_count( sort_criteria_count ),
  m_comparator( m_sortorder, func )
{
    //dummy init , will later be replaced with loading from settings
    for ( unsigned int i = 0; i < m_coloumnCount; ++i) {
        m_column_map[i] = i;

    }

    SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
    SetImageList( &icons(), wxIMAGE_LIST_SMALL );
    SetImageList( &icons(), wxIMAGE_LIST_STATE );
    m_sortorder = sett().GetSortOrder( name );

}

template < class T >
CustomVirtListCtrl<T>::~CustomVirtListCtrl()
{
    sett().SetSortOrder( m_name, m_sortorder );
}

template < class T >
void CustomVirtListCtrl<T>::InsertColumn(long i, wxListItem item, wxString tip, bool modifiable)
{
//#ifdef __WXMSW__ //this fixes header text misalignement
//    item.m_mask = wxLIST_MASK_FORMAT | wxLIST_MASK_TEXT;
//    if ( item.m_image != icons().ICON_EMPTY || item.m_image != -1 )
//        item.m_mask = item.m_mask | wxLIST_MASK_IMAGE;
//
//    item.m_format = wxLIST_FORMAT_LEFT;
//#endif
    ListBaseType::InsertColumn(i,item);
    colInfo temp(tip,modifiable);
    m_colinfovec.push_back(temp);
}

template < class T >
void CustomVirtListCtrl<T>::AddColumn(long i, int width, const wxString& label, const wxString& tip, bool modifiable)
{
    ListBaseType::InsertColumn( i, label, wxLIST_FORMAT_LEFT, width);
    SetColumnWidth( i, width );
    colInfo temp( i, label, tip, modifiable, width );
    m_colinfovec.push_back( temp );
}



template < class T >
void CustomVirtListCtrl<T>::SaveSelection()
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

template < class T >
void CustomVirtListCtrl<T>::RestoreSelection()
{
    while ( m_selected_data.size() > 0 )
    {
        SelectedDataType data = m_selected_data.back();
        m_selected_data.pop_back();
        int idx = GetIndexFromData( data );
        SetItemState( idx, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    }
}

template < class T >
void CustomVirtListCtrl<T>::ResetSelection()
{
    m_selected_data.clear();
}

template < class T >
void CustomVirtListCtrl<T>::OnSelected( wxListEvent& event )
{
  m_selected_index = event.GetIndex();
  event.Skip();
}

template < class T >
void CustomVirtListCtrl<T>::OnDeselected( wxListEvent& event )
{
    if ( m_selected_index == event.GetIndex() )
        m_selected_index = -1;
}

template < class T >
long CustomVirtListCtrl<T>::GetSelectedIndex()
{
  return m_selected_index ;
}

template < class T >
void CustomVirtListCtrl<T>::SelectAll()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_SELECTED, -1  );
  }
}

template < class T >
void CustomVirtListCtrl<T>::SelectNone()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_DONTCARE, -1 );
  }
}
template < class T >
void CustomVirtListCtrl<T>::SelectInverse()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    int state = GetItemState( i, -1 );
    state = ( state == wxLIST_STATE_DONTCARE ? wxLIST_STATE_SELECTED : wxLIST_STATE_DONTCARE );
    SetItemState( i, state, -1 );
  }
}

template < class T >
void CustomVirtListCtrl<T>::SetSelectedIndex(const long newindex)
{
    m_selected_index = newindex;
}

template < class T >
void CustomVirtListCtrl<T>::RefreshVisibleItems()
{
    long topItemIndex = GetTopItem();
    long range = topItemIndex + GetCountPerPage();
    //RefreshItems( topItemIndex,  clamp( range, topItemIndex, (long) m_data.size() ) );
    RefreshItems( topItemIndex,  range );
}

template < class T >
void CustomVirtListCtrl<T>::OnTimer(wxTimerEvent& event)
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

template < class T >
void CustomVirtListCtrl<T>::OnMouseMotion(wxMouseEvent& event)
{
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

template < class T >
void CustomVirtListCtrl<T>::SetTipWindowText( const long item_hit, const wxPoint position)
{
  int coloumn = getColoumnFromPosition(position);
  if (coloumn >= int(m_colinfovec.size()) || coloumn < 0)
  {
    m_tiptext = _T("");
  }
  else
  {
    m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
    m_tiptext = TE(m_colinfovec[coloumn].tip);
  }
}

template < class T >
int CustomVirtListCtrl<T>::getColoumnFromPosition(wxPoint pos)
{
    int x_pos = 0;
    for (int i = 0; i < int(m_colinfovec.size());++i)
    {
        x_pos += GetColumnWidth(i);
        if (pos.x < x_pos)
            return i;
    }
    return -1;
}

template < class T >
void CustomVirtListCtrl<T>::OnStartResizeCol(wxListEvent& event)
{
    if (!m_colinfovec[event.GetColumn()].can_resize)
        event.Veto();
}

template < class T >
void CustomVirtListCtrl<T>::OnEndResizeCol(wxListEvent& event)
{
    int column = event.GetColumn();
    int new_size = GetColumnWidth( column );
    sett().SetColumnWidth( m_name, column, new_size );

    //let the event go further
    event.Skip();
}

template < class T >
bool CustomVirtListCtrl<T>::SetColumnWidth(int col, int width)
{
    if ( sett().GetColumnWidth( m_name, col) != Settings::columnWidthUnset)
    {
        return ListBaseType::SetColumnWidth( col, sett().GetColumnWidth( m_name, col) );
    }
    else
    {
        sett().SetColumnWidth( m_name, col, width );
        return ListBaseType::SetColumnWidth( col, width );
    }
}

template < class T >
void CustomVirtListCtrl<T>::noOp(wxMouseEvent& event)
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

//template < class T >
//void CustomVirtListCtrl<T>::UpdateHighlights()
//{
//  Freeze();
//  try {
//      long item = -1;
//      while ( true ) {
//        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
//        if ( item == -1 )
//          break;
//        HighlightItem( item );
//      }
//  } catch(...) {}
//  Thaw();
//}

template < class T >
wxListItemAttr* CustomVirtListCtrl<T>::HighlightItemUser( const wxString& name ) const
{
    static wxListItemAttr att;
  if ( m_highlight && useractions().DoActionOnUser( m_highlightAction, name ) ) {
        att.SetBackgroundColour( sett().GetGroupHLColor( useractions().GetGroupOfUser( name ) ) );
        return &att;
  }
  else
    return NULL;
}

template < class T >
void CustomVirtListCtrl<T>::SetHighLightAction( UserActions::ActionType action )
{
    m_highlightAction = action;
}

template < class T >
void CustomVirtListCtrl<T>::MarkDirtySort()
{
  m_dirty_sort = true;
}

template < class T >
void CustomVirtListCtrl<T>::CancelTooltipTimer()
{
    m_tiptimer.Stop();
}

template < class T >
bool CustomVirtListCtrl<T>::PopupMenu(wxMenu* menu, const wxPoint& pos )
{
    CancelTooltipTimer();
    return ListBaseType::PopupMenu( menu, pos );
}

template < class T >
void CustomVirtListCtrl<T>::SortList( bool force )
{
    if ( !m_dirty_sort && !force )
        return;
    SelectionSaver<ThisType>(this);
    Freeze();
    Sort();
    Thaw();
    m_dirty_sort = false;
    RefreshVisibleItems();
}

template < class T >
void CustomVirtListCtrl<T>::Clear()
{
    m_data.clear();
    SetItemCount( 0 );
    ResetSelection();
    RefreshVisibleItems();
}

template < class T >
typename CustomVirtListCtrl<T>::DataType CustomVirtListCtrl<T>::GetDataFromIndex ( const  long index )
{
    return m_data[index];
}

template < class T >
const typename CustomVirtListCtrl<T>::DataType CustomVirtListCtrl<T>::GetDataFromIndex ( const  long index ) const
{
    return m_data[index];
}

template < class T >
typename CustomVirtListCtrl<T>::DataType CustomVirtListCtrl<T>::GetSelectedData()
{
    return GetDataFromIndex( m_selected_index );
}

template < class T >
void CustomVirtListCtrl<T>::ResetColumnSizes()
{
    typename colInfoVec::const_iterator it = m_colinfovec.begin();
    for ( ; it != m_colinfovec.end(); ++it )
        SetColumnWidth( it->col_num, it->size );
}

template < class T >
void CustomVirtListCtrl<T>::OnColClick( wxListEvent& event )
{
    if ( event.GetColumn() == -1 )
        return;

    int old_sort_col = m_sortorder[0].col;

    wxListItem col;
    GetColumn( m_sortorder[0].col, col );
    col.SetImage( icons().ICON_NONE );
    SetColumn( m_sortorder[0].col, col );
    int evt_col = event.GetColumn();

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

    m_sortorder[0].col = event.GetColumn();
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

template < class T >
void CustomVirtListCtrl<T>::ReverseOrder()
{
    SaveSelection();
    std::reverse( m_data.begin(), m_data.end() );
    RefreshVisibleItems();
    RestoreSelection();
}
