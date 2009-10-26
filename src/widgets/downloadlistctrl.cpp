#include "downloadlistctrl.h"

#include "../utils/conversion.h"

template<> SortOrder WidgetDownloadListctrl::BaseType::m_sortorder = SortOrder();

const unsigned int column_count = 4;

BEGIN_EVENT_TABLE( WidgetDownloadListctrl, WidgetDownloadListctrl::BaseType )
  EVT_LIST_ITEM_ACTIVATED( WIDGETLISTCTRL_ID, WidgetDownloadListctrl::OnActivateItem )
  EVT_LIST_COL_CLICK( WIDGETLISTCTRL_ID, WidgetDownloadListctrl::OnColClick )
END_EVENT_TABLE()

WidgetDownloadListctrl::WidgetDownloadListctrl(wxWindow* parent, wxWindowID /*unused*/, const wxString& /*unused*/,
                    long /*unused*/, const wxPoint&/*unused*/, const wxSize& /*unused*/)
    : WidgetDownloadListctrl::BaseType(parent, WIDGETLISTCTRL_ID, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("WidgetDownloadListCtrl"), column_count, 3, &CompareOneCrit)
{
    const int as = wxLIST_AUTOSIZE;
#if defined(__WXMSW__)
    const int widths [column_count] = { as, as, as, as };
#elif defined(__WXMAC__)
    const int widths [column_count] = { as, as, as, as };
#else
    const int widths [column_count] = { as, as, as, as };
#endif

    AddColumn( 0, widths[0], _("Name"), _T("Name") );
    AddColumn( 1, widths[1], _T("Author"), _T("Author") );
    AddColumn( 2, widths[2], _T("Mods"), _T("Compatible mods") );
    AddColumn( 3, widths[3], _T("Downloads"), _T("Downloads") );

    if ( m_sortorder.size() == 0 ) {
        m_sortorder[2].col = 2;
        m_sortorder[2].direction = 1;
        m_sortorder[0].col = 0;
        m_sortorder[0].direction = 1;
        m_sortorder[1].col = 1;
        m_sortorder[1].direction = 1;
    }
}

WidgetDownloadListctrl::~WidgetDownloadListctrl()
{
    //dtor
}

int WidgetDownloadListctrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    switch ( col ) {
        case 0: return dir * u1.name.CmpNoCase( u2.name );
//        case 1: return dir * u1.short_description.CmpNoCase( u2.short_description );
        case 1: return dir * u1.author.CmpNoCase( u2.author );
        case 2: return dir * u1.mods.CmpNoCase( u2.mods );
        case 3: return dir * compareSimple( u1.num_downloads, u2.num_downloads );
//        case 5: return dir * u1.date.CmpNoCase( u2.date );
        default: return 0;
    }
}

void WidgetDownloadListctrl::AddWidget( const Widget widget )
{
    if ( AddItem( widget ) )
        return;

    wxLogWarning( _T("Widget already in list.") );
}

wxString WidgetDownloadListctrl::GetItemText(long item, long column) const
{
    if ( item > long(m_data.size()) || item < 0 )
        return wxEmptyString;

    const Widget& widget = m_data[item];
    switch ( column ) {
        default: return wxEmptyString;
        case 0: return widget.name;
//        case 1: return widget.short_description;
        case 1: return widget.author;
        case 2: return widget.mods;
        case 3: return TowxString( widget.num_downloads );
//        case 5: return widget.date;
    }

}

int WidgetDownloadListctrl::GetItemImage(long /*unused*/) const
{
    return -1;
}

int WidgetDownloadListctrl::GetItemColumnImage(long /*unused*/, long /*unused*/) const
{
    return -1;
}

void WidgetDownloadListctrl::HighlightItem( long /*unused*/ )
{

}

void WidgetDownloadListctrl::Sort()
{
    if ( m_data.size() > 0 )
    {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}

int WidgetDownloadListctrl::GetIndexFromData( const DataType& data ) const
{
    static long seekpos;
    seekpos = clamp( seekpos, 0l , (long)m_data.size() );
    int index = seekpos;

    for ( DataCIter f_idx = m_data.begin() + seekpos; f_idx != m_data.end() ; ++f_idx )
    {
        if ( data.Equals( *f_idx ) )
        {
            seekpos = index;
            return seekpos;
        }
        index++;
    }
    //it's ok to init with seekpos, if it had changed this would not be reached
    int r_index = seekpos - 1;
    for ( DataRevCIter r_idx = m_data.rbegin() + ( m_data.size() - seekpos ); r_idx != m_data.rend() ; ++r_idx )
    {
        if ( data.Equals( *r_idx ) )
        {
            seekpos = r_index;
            return seekpos;
        }
        r_index--;
    }

    return -1;
}

Widget& WidgetDownloadListctrl::GetSelectedWidget()
{
    return m_data[m_selected_index];
}

void WidgetDownloadListctrl::OnActivateItem( wxListEvent& event )
{
    int index = event.GetIndex();
    if ( index == -1 ) return;
//    WidgetInfoDialog* dl = new WidgetInfoDialog( m_data[index], (wxWindow*)this, -1, _("stuff") );
//    dl->Show(true);
}
