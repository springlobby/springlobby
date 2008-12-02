#include "customvirtlistctrl.h"
#include "utils.h"
#include "settings.h"
#include <wx/colour.h>
#include <wx/log.h>
#include "iconimagelist.h"
#include "settings++/custom_dialogs.h"
#include "uiutils.h"


BEGIN_EVENT_TABLE(CustomVirtListCtrl, ListBaseType)
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
END_EVENT_TABLE()


//wxTipWindow* CustomVirtListCtrl::m_tipwindow = 0;
CustomVirtListCtrl::CustomVirtListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,long style,wxString name,
                               unsigned int column_count, bool highlight, UserActions::ActionType hlaction ):
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
  m_dirty_sort(false)
{
    //dummy init , will later be replaced with loading from settings
    for ( unsigned int i = 0; i < m_coloumnCount; ++i) {
        m_column_map[i] = i;

    }

    SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
    SetImageList( &icons(), wxIMAGE_LIST_SMALL );
    SetImageList( &icons(), wxIMAGE_LIST_STATE );

    //make sure we have at least one (empty) sort criteria
    m_sortorder[0].direction = 0;
    m_sortorder[0].col = 0;
}

void CustomVirtListCtrl::InsertColumn(long i, wxListItem item, wxString tip, bool modifiable)
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

void CustomVirtListCtrl::AddColumn(long i, int width, const wxString& label, const wxString& tip, bool modifiable)
{
    ListBaseType::InsertColumn( i, label, wxLIST_FORMAT_LEFT, width);
    colInfo temp(tip,modifiable);
    m_colinfovec.push_back(temp);
}

void CustomVirtListCtrl::SetSelectionRestorePoint()
{
    m_prev_selected_index = m_selected_index;
}

void CustomVirtListCtrl::RestoreSelection()
{
    if ( m_prev_selected_index> -1)
    {
        SetItemState( m_prev_selected_index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
    }
}

void CustomVirtListCtrl::ResetSelection()
{
    m_prev_selected_index = m_selected_index = -1;
}

void CustomVirtListCtrl::OnSelected( wxListEvent& event )
{
  m_selected_index = event.GetIndex();
  event.Skip();
}


void CustomVirtListCtrl::OnDeselected( wxListEvent& event )
{
    if ( m_selected_index == event.GetIndex() )
        m_selected_index = -1;
}


long CustomVirtListCtrl::GetSelectedIndex()
{
  return m_selected_index ;
}

void CustomVirtListCtrl::SelectAll()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_SELECTED, -1  );
  }
}

void CustomVirtListCtrl::SelectNone()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_DONTCARE, -1 );
  }
}

void CustomVirtListCtrl::SelectInverse()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    int state = GetItemState( i, -1 );
    state = ( state == wxLIST_STATE_DONTCARE ? wxLIST_STATE_SELECTED : wxLIST_STATE_DONTCARE );
    SetItemState( i, state, -1 );
  }
}

void CustomVirtListCtrl::SetSelectedIndex(const long newindex)
{
    m_selected_index = newindex;
}


void CustomVirtListCtrl::OnTimer(wxTimerEvent& event)
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

void CustomVirtListCtrl::OnMouseMotion(wxMouseEvent& event)
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

void CustomVirtListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
  int coloumn = getColoumnFromPosition(position);
  if (coloumn >= int(m_colinfovec.size()) || coloumn < 0)
  {
    m_tiptext = _T("");
  }
  else
  {
    m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
    m_tiptext = TE(m_colinfovec[coloumn].first);
  }
}

int CustomVirtListCtrl::getColoumnFromPosition(wxPoint pos)
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

void CustomVirtListCtrl::OnStartResizeCol(wxListEvent& event)
{
    if (!m_colinfovec[event.GetColumn()].second)
        event.Veto();
}

void CustomVirtListCtrl::OnEndResizeCol(wxListEvent& event)
{
    int column = event.GetColumn();
    int new_size = GetColumnWidth( column );
    sett().SetColumnWidth( m_name, column, new_size );

    //let the event go further
    event.Skip();
}

bool CustomVirtListCtrl::SetColumnWidth(int col, int width)
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

void CustomVirtListCtrl::noOp(wxMouseEvent& event)
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

void CustomVirtListCtrl::UpdateHighlights()
{
  Freeze();
  try {
      long item = -1;
      while ( true ) {
        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
        if ( item == -1 )
          break;
        HighlightItem( item );
      }
  } catch(...) {}
  Thaw();
}

void CustomVirtListCtrl::HighlightItemUser( long item, const wxString& name )
{
   if ( m_highlight && useractions().DoActionOnUser( m_highlightAction, name ) ) {
        wxColor c = sett().GetGroupHLColor( useractions().GetGroupOfUser( name ) );
        SetItemBackgroundColour( item, c );
  }
  else
    SetItemBackgroundColour( item, m_bg_color );
}

void CustomVirtListCtrl::SetHighLightAction( UserActions::ActionType action )
{
    m_highlightAction = action;
}

void CustomVirtListCtrl::MarkDirtySort()
{
  m_dirty_sort = true;
}

void CustomVirtListCtrl::CancelTooltipTimer()
{
    m_tiptimer.Stop();
}

bool CustomVirtListCtrl::PopupMenu(wxMenu* menu, const wxPoint& pos )
{
    CancelTooltipTimer();
    return ListBaseType::PopupMenu( menu, pos );
}

void CustomVirtListCtrl::SortList( bool force )
{
    if ( !m_dirty_sort && !force )
        return;

    SetSelectionRestorePoint();
    Freeze();
    Sort();
    Thaw();
    m_dirty_sort = false;
    RestoreSelection();
    long topItemIndex = GetTopItem();
    RefreshItems( topItemIndex, topItemIndex + GetCountPerPage() );
}
